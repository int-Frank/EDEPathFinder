#include "Path.h"
#include "TSP.h"

#define GET_SYSTEM(engineerName) g_GameData.engineers.at(engineerName).system

static struct ModuleItem
{
  std::string name;
  bool active;
  int grade;
  int gradeTarget;
  std::set<std::string> engineers;
};

typedef std::list<ModuleItem> ModuleItemList;
typedef std::set<std::string> EngineerList;

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
    result.push_back(item);
  }
  return result;
}

//static EngineerList GetEssentialEngineers(std::list<ModuleItem> const & modules)
//{
//  EngineerList result;
//
//  for (auto const & item : modules)
//  {
//    if (item.engineers.size() == 1)
//      result.insert(*item.engineers.begin());
//  }
//
//  return result;
//}

static EngineerList GetEngineerList(ModuleItemList const & modules)
{
  EngineerList result;

  for (auto const & item : modules)
    result.insert(item.engineers.begin(), item.engineers.end());

  return result;
}

// Some engineers may not be needed. For example, consider the engineers A, B and modules M, N:
//   - Engineer A can upgrade both module M and N to grade 5.
//   - Engineer B can upgrade module N to grade 5.
//   - We must visit engineer A to upgrade M, but can also upgrade N while we are there, therefore
//     engineer B ois redundant.
//static void RemoveUnusedEngineers(std::list<ModuleItem> &modules)
//{
//  std::set<std::string> essentialEngineers = GetEssentialEngineers(modules);
//  std::list<ModuleItem> modulesTemp = modules;
//
//  // Remove all modules containing the essential engineers. This will remove any
//  // unwanted engineers.
//  for (auto engineerName : essentialEngineers)
//  {
//    for (auto it = modulesTemp.begin(); it != modulesTemp.end();)
//    {
//      if (it->engineers.find(engineerName) != it->engineers.end())
//        it = modulesTemp.erase(it);
//      else
//        it++;
//    }
//  }
//  
//  std::set<std::string> remainingEngineers = GetEngineerList(modulesTemp);
//  remainingEngineers.insert(essentialEngineers.begin(), essentialEngineers.end());
//
//  // Remove any engineers from modules that are not in the remaining engineers list
//  for (auto & item : modules)
//  {
//    for (auto it = item.engineers.begin(); it != item.engineers.end();)
//    {
//      if (remainingEngineers.find(*it) == remainingEngineers.end())
//        it = item.engineers.erase(it);
//      else
//        it++;
//    }
//  }
//}

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

        it->engineers.insert(engineerName);
      }
      else if (kvmg.second > it->grade)
      {
        it->engineers.clear();
        it->engineers.insert(engineerName);
        it->grade = kvmg.second;
      }
      else if (kvmg.second == it->grade)
      {
        it->engineers.insert(engineerName);
      }
    }
  }

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
void DoMinimumLength(std::vector<SystemNode> const &subPath, std::vector<SystemNode> & out)
{
  Matrix<float> distanceMatrix(subPath.size() + 2, subPath.size() + 2, 0.0);
  for (size_t i = 0; i < subPath.size(); i++)
  {
    Float3 pi = g_GameData.system[subPath[i].name].position;
    for (size_t j = i; j < subPath.size(); j++)
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

// Init engineer
static bool InitEngineer(std::list<ModuleItem> & placedModules,
                      std::list<ModuleItem> & modules,
                      std::list<ModuleItem>::iterator & placedModule_it,
                      std::set<std::string>::iterator & placedEngineer_it,
                      std::list<ModuleItem>::iterator & module_it,
                      std::set<std::string>::iterator & engineer_it)
{
  if (engineer_it == module_it->engineers.end())
  {
    module_it++;
    return InitModule(placedModules, modules, placedModule_it, placedEngineer_it, module_it, engineer_it);
  }
  return true;
}

// Init module
static bool InitModule(std::list<ModuleItem> & placedModules,
                      std::list<ModuleItem> & modules,
                      std::list<ModuleItem>::iterator & placedModule_it,
                      std::set<std::string>::iterator & placedEngineer_it,
                      std::list<ModuleItem>::iterator & module_it,
                      std::set<std::string>::iterator & engineer_it)
{
  if (module_it == modules.end())
  {
    placedEngineer_it++;
    return InitPlacedEngineer(placedModules, modules, placedModule_it, placedEngineer_it, module_it, engineer_it);
  }
  engineer_it = module_it->engineers.begin();
  return InitEngineer(placedModules, modules, placedModule_it, placedEngineer_it, module_it, engineer_it);
}

// Init placedEngineer
static bool InitPlacedEngineer(std::list<ModuleItem> & placedModules,
                      std::list<ModuleItem> & modules,
                      std::list<ModuleItem>::iterator & placedModule_it,
                      std::set<std::string>::iterator & placedEngineer_it,
                      std::list<ModuleItem>::iterator & module_it,
                      std::set<std::string>::iterator & engineer_it)
{
  if (placedEngineer_it == placedModule_it->engineers.end())
  {
    placedModule_it++;
    return InitPlacedModule(placedModules, modules, placedModule_it, placedEngineer_it, module_it, engineer_it);
  }
  module_it = modules.begin();
  return InitModule(placedModules, modules, placedModule_it, placedEngineer_it, module_it, engineer_it);
}

// Init placedModule
static bool InitPlacedModule(std::list<ModuleItem> & placedModules,
                      std::list<ModuleItem> & modules,
                      std::list<ModuleItem>::iterator & placedModule_it,
                      std::set<std::string>::iterator & placedEngineer_it,
                      std::list<ModuleItem>::iterator & module_it,
                      std::set<std::string>::iterator & engineer_it)
{
  if (placedModule_it == placedModules.end())
    return false;
  placedEngineer_it = placedModule_it->engineers.begin();
  return InitPlacedEngineer(placedModules, modules, placedModule_it, placedEngineer_it, module_it, engineer_it);
}

static void RecursiveFindPath(SystemNode const & startSystem,
                              std::list<ModuleItem> placedModules,
                              std::list<ModuleItem>::iterator placedModule_it,
                              std::set<std::string>::iterator placedEngineer_it,
                              std::list<ModuleItem> & modules,
                              std::list<ModuleItem>::iterator & module_it,
                              std::set<std::string>::iterator engineer_it,
                              std::vector<SystemNode> & currentPath,
                              std::vector<SystemNode> & currentBest)
{

  std::vector<SystemNode> subPath;

  for (auto it = modules.begin(); it != modules.end();)
  {
    if (g_GameData.selectedEngineers == priority)
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

static void AddSystem(std::list<ModuleItem> & modules, std::string const & systemName, std::vector<SystemNode> & out)
{
  SystemNode system;
  system.name = systemName;

  // Erase all modules containing the engineer that lives in systemName
  for (auto mit = modules.begin(); mit != modules.end();)
  {
    bool found = false;
    for (auto it = mit->second.engineers.begin(); it != mit->second.engineers.end(); it++)
    {
      if (systemName == GET_SYSTEM(*it))
      {
        found = true;
        break;
      }
    }

    if (found)
    {
      system.modules.insert(std::pair<std::string, int>(mit->first, mit->second.grade));
      mit = modules.erase(mit);
    }
    else
    {
      mit++;
    }
  }

  out.push_back(system);
}

//static void ExtractPlacedModules(ModuleItemList & placedModules, ModuleItemList & modules, int place)
//{
//  for (auto mit = modules.begin(); mit != modules.end();)
//  {
//    auto it = g_GameData.selectedModules.find(mit->name);
//    if (it != g_GameData.selectedModules.end())
//    {
//      placedModules.push_back(*mit);
//      mit = modules.erase(mit);
//      ExtractPlacedModules(placedModules, modules, place + 1);
//      break;
//    }
//    else
//    {
//      mit++;
//    }
//  }
//}

static struct Data
{
  EngineerList placedEngineers;
  EngineerList engineers;
  ModuleItemList modules;
  std::vector<SystemNode> currentBest;
  std::vector<SystemNode> currentPath;
};

static void FindShortestPath(ModuleItemList & modules, std::vector<SystemNode> &out)
{
  // Make 2 lists:
  // Head = all placed modules (modules must remain in this order)
  // Tail = all others (can be sorted)

  AddSystem(modules, g_GameData.startSystem, out);

  ModuleItemList placedModules;

  std::vector<SystemNode> currentBest;
  std::vector<SystemNode> currentPath;

  EngineerList placedEngineers = GetEngineerList(placedModules);
  EngineerList engineers = GetEngineerList(modules);

  RecursiveFindPath(out[0], placedModules, modules, currentPath, currentBest);
}

static bool FindBestRoute(std::vector<SystemNode> & out)
{
  bool result = true;
  try
  {
    ModuleItemList modules = GetSelectedModules();

    AssignEngineersToModules(modules);
    RemoveDeadModules(modules);

    FindShortestPath(modules, out);
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
    dist += (path[i].system.second.position - path[i + 1].system.second.position).Length();
  }
  return dist;
}