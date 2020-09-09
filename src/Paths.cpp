#include "Path.h"
#include "TSP.h"

typedef std::vector<std::map<SystemName, std::vector<ModuleGrade>>> PathMap;

ModuleGrade::ModuleGrade()
  : m_data(0)
{

}

ModuleGrade::ModuleGrade(ModuleName name, uint8_t grade)
  : m_data(0)
{
  SetName(name);
  SetGrade(grade);
}

void ModuleGrade::SetName(ModuleName name)
{
  m_data = (m_data & 0xFF) | (uint16_t(name) << 8);
}

void ModuleGrade::SetGrade(uint8_t grade)
{
  m_data = (m_data & 0xFF00) | (uint16_t(grade));
}

ModuleName ModuleGrade::Name() const
{
  return ModuleName(m_data >> 8);
}

uint8_t ModuleGrade::Grade() const
{
  return uint8_t(m_data & 0xFF);
}

struct ModuleData
{
  ModuleData()
    : grade(0)
    , givenGrade(0)
  {
  };

  uint32_t givenGrade;
  uint32_t grade;
  std::vector<EngineerName> engineers;
};

static void Scrub(EngineerName e, std::map<ModuleName, ModuleData> &modules, std::vector<EngineerName> &engineers)
{
  for (auto & kv : modules)
  {
    for (size_t i = 0; i < kv.second.engineers.size(); i++)
    {
      if (kv.second.engineers[i] == e)
      {
        kv.second.engineers[i] = kv.second.engineers.back();
        kv.second.engineers.pop_back();
        break;
      }
    }
  }

  for (size_t i = 0; i < engineers.size(); i++)
  {
    if (engineers[i] == e)
    {
      engineers[i] = engineers.back();
      engineers.pop_back();
      break;
    }
  }
}

static bool IsPrimary(EngineerName e, std::map<ModuleName, ModuleData> const &modules)
{
  for (auto const & kv : modules)
  {
    if (kv.second.engineers.size() == 1 && kv.second.engineers[0] == e)
      return true;
  }
  return false;
}


static uint64_t FindContributions(EngineerName eIn, std::map<ModuleName, ModuleData> const &modules)
{
  uint64_t result = 0;
  for (auto const & kv : modules)
  {
    for (auto e : kv.second.engineers)
    {
      if (e == eIn)
      {
        result |= (1ull << kv.first);
        break;
      }
    }
  }
  return result;
}

static std::map<ModuleName, ModuleData> GetSelectedModules()
{
  std::map<ModuleName, ModuleData> result;
  for (size_t i = 0; i < sizeof(g_GameData.selectedModules) * CHAR_BIT; i++)
  {
    if ((g_GameData.selectedModules & (1ull << i)) != 0)
      result.insert(std::pair<ModuleName, ModuleData>(ModuleName(i), ModuleData()));
  }
  return result;
}

static std::vector<EngineerName> GetSelectedEngineers()
{
  std::vector<EngineerName> result;
  for (size_t i = 0; i < sizeof(g_GameData.selectedEngineers) * CHAR_BIT; i++)
  {
    if ((g_GameData.selectedEngineers & (1ull << i)) != 0)
      result.push_back(EngineerName(i));
  }
  return result;
}

// Some engineers may not be needed. For example, consider the engineers A, B and modules M, N:
//   - Engineer A can upgrade both module M and N to grade 5.
//   - Engineer B can upgrade module N to grade 5.
//   - We must visit engineer A to upgrade M, but can also upgrade N while we are there, therefore
//     engineer B offers us nothing.
static void RemoveUnusedEngineers(std::vector<EngineerName> &engineers, 
                                  std::map<ModuleName, ModuleData> &modules)
{
  for (EngineerName e : engineers)
  {
    if (IsPrimary(e, modules))
    {
      for (auto & kv : modules)
      {
        if (g_GameData.engineer[e].moduleGrade[kv.first] > kv.second.givenGrade)
          kv.second.givenGrade = g_GameData.engineer[e].moduleGrade[kv.first];
      }
    }
  }

  for (EngineerName e : engineers)
  {
    if (IsPrimary(e, modules))
      continue;

    uint64_t contributions = FindContributions(EngineerName(e), modules);

    bool needed = false;
    if (contributions != 0)
    {
      for (auto & kv : modules)
      {
        if ((contributions & (1ull << kv.first)) == 0)
          continue;

        if (kv.second.givenGrade < g_GameData.engineer[e].moduleGrade[kv.first])
        {
          needed = true;
          break;
        }
      }
    }

    if (!needed)
      Scrub(e, modules, engineers);
  }
}

//Find all the engineers who can upgrade each module to the highest grade.
static void AssignEngineersToModules(std::vector<EngineerName> const & engineers,
                                     std::map<ModuleName, ModuleData> & modules)
{
  for (EngineerName e : engineers)
  {
    for (auto & kv : modules)
    {
      uint32_t grade = g_GameData.engineer[e].moduleGrade[kv.first];
      if (grade > kv.second.grade)
      {
        kv.second.engineers.clear();
        kv.second.engineers.push_back(EngineerName(e));
        kv.second.grade = grade;
      }
      else if (grade > 0 && grade == kv.second.grade)
      {
        kv.second.engineers.push_back(EngineerName(e));
      }
    }
  }
}

static void AddSystemsAssociatedWithModule(std::map<ModuleName, ModuleData>::const_iterator it,
                                           std::vector<SystemName> &path,
                                           std::vector<std::vector<SystemName>> &output,
                                           std::map<ModuleName, ModuleData>::const_iterator itEnd)
{
  if (it == itEnd)
  {
    if (path.size() > 0)
      output.push_back(path);
    return;
  }

  auto itNext = it;
  itNext++;

  for (auto e : it->second.engineers)
  {
    bool hasThisEngineerSystem = false;
    for (auto s : path)
    {
      if (s == GetSystem(e))
      {
        hasThisEngineerSystem = true;
        break;
      }
    }

    if (!hasThisEngineerSystem)
    {
      path.push_back(GetSystem(e));
      AddSystemsAssociatedWithModule(itNext, path, output, itEnd);
      path.pop_back();
    }
    else
    {
      AddSystemsAssociatedWithModule(itNext, path, output, itEnd);
    }
  }
}

static std::vector<std::vector<SystemName>> FindAllPathCombinations(std::map<ModuleName, ModuleData> const &modules)
{
  std::vector<std::vector<SystemName>> output;
  std::vector<SystemName> path;
  AddSystemsAssociatedWithModule(modules.cbegin(), path, output, modules.cend());

  return output;
}

std::map<SystemName, std::vector<ModuleGrade>> FillModuleMap(std::vector<SystemName> const &path, std::map<ModuleName, ModuleData> const & modules)
{
  std::map<SystemName, std::vector<ModuleGrade>> result;

  for (auto s : path)
  {
    for (auto const & kv : modules)
    {
      for (auto e : kv.second.engineers)
      {
        if (GetSystem(e) == s)
          result[s].push_back(ModuleGrade(kv.first, kv.second.grade));
      }
    }
  }

  return result;
}

static void RemoveDeadModules(std::map<ModuleName, ModuleData> &modules)
{
  for (auto it = modules.begin(); it != modules.end();)
  {
    if (it->second.engineers.size() == 0)
      it = modules.erase(it);
    else
      it++;
  }
}

PathMap GenerateAllPossiblePaths()
{
  std::map<ModuleName, ModuleData> modules = GetSelectedModules();
  std::vector<EngineerName> engineers = GetSelectedEngineers();

  AssignEngineersToModules(engineers, modules);
  RemoveUnusedEngineers(engineers, modules);
  RemoveDeadModules(modules);

  std::vector<std::vector<SystemName>> allPaths = FindAllPathCombinations(modules);

  PathMap result;

  for (auto const & path : allPaths)
    result.push_back(FillModuleMap(path, modules));

  return result;
}

//To find the minimum path from a point through all other points, we add 2 dummy points a, b such that:
// a is distance 0 from the start point and inf to all others
// b distance 0 to everything
void DoMinimumLength(std::vector<SystemNode> const &subPath, std::vector<SystemNode> & out)
{
  Matrix<float> distanceMatrix(subPath.size() + 2, subPath.size() + 2, 0.0);
  for (size_t i = 0; i < subPath.size(); i++)
  {
    Float3 pi = g_GameData.system[subPath[i].name].position;
    for (int j = i; j < subPath.size(); j++)
    {
      Float3 pj = g_GameData.system[subPath[j].name].position;

      float d = (pi - pj).Length();
      distanceMatrix(i + 2, j + 2) = d;
      distanceMatrix(j + 2, i + 2) = d;
    }
  }

  for (size_t i = 0; i < subPath.size() + 2; i++)
  {
    distanceMatrix(1, i) = std::numeric_limits<float>::infinity();
    distanceMatrix(i, 1) = std::numeric_limits<float>::infinity();
  }

  for (size_t i = 0; i < subPath.size() + 2; i++)
  {
    distanceMatrix(0, i) = 0.0f;
    distanceMatrix(i, 0) = 0.0f;
  }

  distanceMatrix(2, 1) = 0.0f;
  distanceMatrix(1, 2) = 0.0f;

  int startInd = 0;
  TSPSolver<float> solver(startInd, distanceMatrix);
  solver.solve();

  auto it = solver.getTour().end();
  for (size_t i = 0; i < solver.getTour().size(); i++)
  {
    it--;

    if (*it < 3)
      continue;

    out.push_back(subPath[*it - 2]);
  }
}

void AddSubPath(std::map<SystemName, std::vector<ModuleGrade>> & pathMap, std::vector<SystemNode> & out, int priority)
{
  if (pathMap.size() == 0 || priority < GameData::MinPriority)
    return;

  std::vector<SystemNode> subPath;

  for (auto it = pathMap.begin(); it != pathMap.end();)
  {
    if (g_GameData.engineerPriorities[GetEngineer(it->first)] == priority)
    {
      SystemNode node;
      node.name = it->first;
      node.modules = it->second;
      subPath.push_back(node);
      it = pathMap.erase(it);
    }
    else
    {
      it++;
    }
  }

  if (subPath.size() == 1)
  {
    out.push_back(subPath[0]);
  }
  else if (subPath.size() > 1)
  {
    subPath.push_back(subPath[0]);
    subPath[0] = out.back();
    DoMinimumLength(subPath, out);
  }

  AddSubPath(pathMap, out, priority - 1);
}

void FindShortestPath(std::map<SystemName, std::vector<ModuleGrade>> pathMap, std::vector<SystemNode> &out)
{
  SystemNode startSystem;
  startSystem.name = g_GameData.startSystem;
  auto it = pathMap.find(g_GameData.startSystem);
  if (it != pathMap.end())
  {
    startSystem.modules = it->second;
    pathMap.erase(it);
  }
  out.push_back(startSystem);

  AddSubPath(pathMap, out, GameData::MaxPriority);
}

std::vector<SystemNode> FindShortestPath()
{
  PathMap paths = GenerateAllPossiblePaths();

  float bestDist = FLT_MAX;
  std::vector<SystemNode> bestPath;
  std::vector<SystemNode> currentPath;

  for (auto const & path : paths)
  {
    currentPath.clear();
    FindShortestPath(path, currentPath);
    float dist = PathDistance(currentPath);
    {
      if (dist < bestDist)
      {
        bestDist = dist;
        bestPath = currentPath;
      }
    }
  }

  return bestPath;
}

float PathDistance(std::vector<SystemNode> const & path)
{
  float dist = 0.f;
  for (size_t i = 0; i < path.size() - 1; i++)
  {
    Float3 pi = g_GameData.system[path[i].name].position;
    Float3 pj = g_GameData.system[path[i + 1].name].position;

    dist += (pi - pj).Length();
  }
  return dist;
}