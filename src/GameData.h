#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <string>
#include <map>
#include <set>
#include <vector>

#include "utils.h"

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

struct GameData
{
  static int const MinPriority = 1;
  static int const MaxPriority = 9;

  SystemMap systems;
  ModuleMap modules;
  EngineerMap engineers;

  std::vector<std::string> engineerClasses;

  std::string moduleClassDesc;
  std::string engineerClassDesc;

  std::string startSystem;
  std::set<std::string> selectedEngineers;
  std::map<std::string, ModuleOptions> selectedModules;
};

extern GameData g_GameData;

bool LoadGameData(std::wstring const & filePath);
const std::vector<std::string> * GetParsingMessages();
std::string GetEngineerFromSystem(std::string const & system);

#endif