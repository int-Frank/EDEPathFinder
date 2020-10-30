#include "Path.h"
#include "TSP.h"

#define GET_SYSTEM(engineerName) g_GameData.engineers.at(engineerName).system

// bool : visited
typedef std::map<std::string, bool> EngineerList;
typedef std::map<std::string, std::vector<ModuleNode>> EngineerModuleMap;

struct ModuleItem
{
  std::string name;
  int counter;
  int grade;
  int gradeTarget;
  int priority;
  EngineerList engineers;
};

typedef std::list<ModuleItem> ModuleItemList;

static std::list<ModuleItem>::iterator Find(ModuleItemList & moduleList, std::string const & name)
{
  auto it = moduleList.begin();
  for (; it != moduleList.end(); it++)
  {
    if (it->name == name)
      break;
  }
  return it;
}

static ModuleItemList GetSelectedModules()
{
  ModuleItemList result;
  for (auto const & kv : g_GameData.selectedModules)
  {
    ModuleItem item;
    item.grade = 0;
    item.gradeTarget = kv.second.gradeTarget;
    item.name = kv.first;
    item.counter = 0;
    result.push_back(item);
  }
  return result;
}

// Find all the engineers who can upgrade each module to the required grade, or the
// engineer(s) who can upgrade the module closest to the required grade.
static void AssignEngineersToModules(ModuleItemList & modules)
{
  for (auto const & engineerName : g_GameData.selectedEngineers)
  {
    for (auto const & kvmg : g_GameData.engineers.at(engineerName).moduleGrades)
    {
      auto it = Find(modules, kvmg.first);
      if (it == modules.end())
        continue;

      if (kvmg.second >= it->gradeTarget)
      {
        if (it->grade < it->gradeTarget)
        {
          it->engineers.clear();
          it->grade = it->gradeTarget;
        }

        it->engineers.insert(std::pair<std::string, bool>(engineerName, false));
      }
      else if (kvmg.second > it->grade)
      {
        it->engineers.clear();
        it->engineers.insert(std::pair<std::string, bool>(engineerName, false));
        it->grade = kvmg.second;
      }
      else if (kvmg.second == it->grade)
      {
        it->engineers.insert(std::pair<std::string, bool>(engineerName, false));
      }
    }
  }
}

static void SortByEngineerCount(ModuleItemList & modules)
{
  modules.sort([](const ModuleItem & a, const ModuleItem & b)
    {
      return a.engineers.size() < b.engineers.size();
    });
}

static void RemoveDeadModules(std::list<ModuleItem> & modules)
{
  for (auto it = modules.begin(); it != modules.end();)
  {
    if (it->engineers.size() == 0)
      it = modules.erase(it);
    else
      it++;
  }
}

//To find the minimum path from a point through all other points, we add 2 dummy points a, b such that:
// a is distance 0 from the start point and inf to all others
// b distance 0 to everything
void SortToMinimumDistance(std::vector<SystemNode> & systems)
{
  Matrix<float> distanceMatrix(systems.size() + 2, systems.size() + 2, 0.0);
  for (size_t i = 0; i < systems.size(); i++)
  {
    Float3 pi = g_GameData.systems.at(systems[i].name).position;
    for (size_t j = i; j < systems.size(); j++)
    {
      Float3 pj = g_GameData.systems.at(systems[j].name).position;

      float d = (pi - pj).Length();
      distanceMatrix(i + 2, j + 2) = d;
      distanceMatrix(j + 2, i + 2) = d;
    }
  }

  for (size_t i = 0; i < systems.size() + 2; i++)
  {
    distanceMatrix(1, i) = std::numeric_limits<float>::infinity();
    distanceMatrix(i, 1) = std::numeric_limits<float>::infinity();
  }

  for (size_t i = 0; i < systems.size() + 2; i++)
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
  std::vector<SystemNode> systemsOld = systems;
  systems.clear();
  systems.push_back(systemsOld[0]);

  for (size_t i = 0; i < solver.getTour().size(); i++)
  {
    it--;

    if (*it < 3)
      continue;

    systems.push_back(systemsOld[*it - 2]);
  }
}

static bool InitModule(ModuleItemList & modules,
                       ModuleItemList::iterator & module_it,
                       EngineerList::iterator & engineer_it);

static bool InitEngineer(ModuleItemList & modules,
                         ModuleItemList::iterator & module_it,
                         EngineerList::iterator & engineer_it)
{
  if (engineer_it == module_it->engineers.end())
  {
    module_it++;
    return InitModule(modules, module_it, engineer_it);
  }

  if (engineer_it->second) // If this engineer has already been visited, move to the next
  {
    engineer_it++;
    return InitEngineer(modules, module_it, engineer_it);
  }

  return true;
}

static bool InitModule(ModuleItemList & modules,
                       ModuleItemList::iterator & module_it,
                       EngineerList::iterator & engineer_it)
{
  if (module_it == modules.end())
    return false;

  if (module_it->counter != 0)
  {
    module_it++;
    return InitModule(modules, module_it, engineer_it);
  }

  engineer_it = module_it->engineers.begin();
  return InitEngineer(modules, module_it, engineer_it);
}

static void ProcessBuild(std::vector<SystemNode> & currentBuild,
                         std::vector<SystemNode> & currentBest)
{
  SortToMinimumDistance(currentBuild);
  if (PathDistance(currentBuild) < PathDistance(currentBuild))
    currentBest = currentBuild;
}

static void VisitEngineer(ModuleItemList & modules, 
                          std::string const & engineerName)
{
  for (auto & module : modules)
  {
    for (auto & engineer : module.engineers)
    {
      if (engineer.first == engineerName)
      {
        engineer.second = true;
        module.counter++;
        break;
      }
    }
  }
}

static void UnvisitEngineer(ModuleItemList & modules, 
                            std::string const & engineerName)
{
  for (auto & module : modules)
  {
    for (auto & engineer : module.engineers)
    {
      if (engineer.first == engineerName)
      {
        engineer.second = false;
        module.counter--;
        break;
      }
    }
  }
}

static void RecursiveFindPath(EngineerModuleMap const & engineers,
                              ModuleItemList & modules,
                              ModuleItemList::iterator currentModule,
                              EngineerList::iterator currentEngineer,
                              std::vector<SystemNode> & currentBuild,
                              std::vector<SystemNode> & currentBest)
{
  if (!InitEngineer(modules, currentModule, currentEngineer))
  {
    ProcessBuild(currentBuild, currentBest);
    return;
  }
  
  SystemNode node;
  node.name = GET_SYSTEM(currentEngineer->first);
  node.modules = engineers.at(currentEngineer->first);
  currentBuild.push_back(node);

  VisitEngineer(modules, currentEngineer->first);

  ModuleItemList::iterator nextModule = currentModule;
  nextModule++;

  if (!InitModule(modules, currentModule, currentEngineer))
  {
    ProcessBuild(currentBuild, currentBest);
  }
  else
  {
    EngineerList::iterator nextEngineer = nextModule->engineers.begin();
    RecursiveFindPath(engineers, modules, nextModule, nextEngineer, currentBuild, currentBest);
  }

  currentBuild.pop_back();
  UnvisitEngineer(modules, currentEngineer->first);

  RecursiveFindPath(engineers, modules, currentModule, ++currentEngineer, currentBuild, currentBest);
}

static void AddSystem(ModuleItemList & modules, std::string const & systemName, std::vector<SystemNode> & out)
{
  SystemNode system;
  system.name = systemName;

  // Erase all modules containing the engineer that lives in systemName
  for (auto mit = modules.begin(); mit != modules.end();)
  {
    bool found = false;
    for (auto it = mit->engineers.begin(); it != mit->engineers.end(); it++)
    {
      if (systemName == GET_SYSTEM(it->first))
      {
        found = true;
        break;
      }
    }

    if (found)
    {
      system.modules.push_back({mit->name, mit->grade});
      mit = modules.erase(mit);
    }
    else
    {
      mit++;
    }
  }

  out.push_back(system);
}

static void FindShortestPath(EngineerModuleMap const & engineers, ModuleItemList & modules, std::vector<SystemNode> &out)
{
  if (modules.size() == 0)
    return;

  SortByEngineerCount(modules);

  std::vector<SystemNode> currentBuild;
  currentBuild.push_back(out.at(0));

  RecursiveFindPath(engineers, modules, modules.begin(), modules.begin()->engineers.begin(), currentBuild, out);
}

static std::set<int> ExtractPriorities(ModuleItemList const & modules)
{
  std::set<int> result;
  for (auto const & m : modules)
    result.insert(m.priority);
  return result;
}

static ModuleItemList ExtractModules(ModuleItemList & modules, int priority)
{
  ModuleItemList result;
  for (auto it = modules.begin(); it != modules.end();)
  {
    if (it->priority == priority)
    {
      result.push_back(*it);
      it = modules.erase(it);
    }
    else
    {
      it++;
    }
  }
  return result;
}

static EngineerModuleMap GetEngineerModuleMap(ModuleItemList const & modules)
{
  EngineerModuleMap result;
  for (auto const & module : modules)
  {
    for (auto const & kv: module.engineers)
    {
      ModuleNode node;
      node.moduleName = module.name;
      node.grade = module.grade;
      result[kv.first].push_back(node);
    }
  }
  return result;
}

static bool FindBestRoute(std::vector<SystemNode> & out)
{
  bool result = true;
  try
  {
    ModuleItemList modules = GetSelectedModules();

    AssignEngineersToModules(modules);
    RemoveDeadModules(modules);
    AddSystem(modules, g_GameData.startSystem, out);
    EngineerModuleMap engineers = GetEngineerModuleMap(modules);

    std::set<int> priorities = ExtractPriorities(modules);

    for (int priority : priorities)
    {
      ModuleItemList moduleSet = ExtractModules(modules, priority);
      std::vector<SystemNode> path;
      path.push_back(path.back());
      FindShortestPath(engineers, modules, path);
      out.insert(out.end(), ++path.begin(), path.end());
    }
  }
  catch (...)
  {
    result = false;
  }
  return result;
}

float PathDistance(std::vector<SystemNode> const & path)
{
  float dist = 0.f;
  for (size_t i = 0; (i + 1) < path.size(); i++)
  {
    Float3 s0 = g_GameData.systems.at(path[i].name).position;
    Float3 s1 = g_GameData.systems.at(path[i + 1].name).position;

    dist += (s1 - s0).Length();
  }
  return dist;
}