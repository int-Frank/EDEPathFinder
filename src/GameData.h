#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <string>
#include <map>
#include <set>
#include <vector>

#include "utils.h"

#define UNKNOWN_REGION "Unkown"

struct SystemData
{
  Float3 position;
};

struct ModuleData
{
  std::string Class;
};

struct EngineerData
{
  std::string system;
  std::string Class;
  std::vector<std::pair<std::string, int>> moduleGrades; // Sorted by grade, highest to lowest
};

typedef std::map<std::string, SystemData> SystemMap;
typedef std::map<std::string, ModuleData> ModuleMap;
typedef std::map<std::string, EngineerData> EngineerMap;

struct ModuleOptions
{
  int gradeTarget;
  int priority;
};

class GameData
{
public:

  static int const MinPriority = 1;
  static int const MaxPriority = 9;

  GameData();

  bool Load(std::wstring const & filePath);
  const std::vector<std::string> * GetParsingMessages() const;

  std::string GetEngineer(std::string const & system) const;

  std::set<std::string> selectedEngineers;
  std::map<std::string, ModuleOptions> selectedModules;

  std::string startSystem;

  std::string moduleClassStr;
  std::string engineerClassStr;

  std::vector<std::string> engineerClasses;
  SystemMap systems;
  ModuleMap modules;
  EngineerMap engineers;
};

extern GameData g_GameData;

#endif