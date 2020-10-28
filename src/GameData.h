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
  std::string moduleClass;
};

struct EngineerData
{
  std::string system;
  std::map<std::string, int> moduleGrades;
};

typedef std::map<std::string, SystemData> SystemMap;
typedef std::map<std::string, ModuleData> ModuleMap;
typedef std::map<std::string, EngineerData> EngineerMap;

enum class Region
{
  Bubble,
  Colonial,
  Unknown
};

struct ModuleSelect
{
  int gradeTarget;
  int place; // 0 = no place
};

struct GameData
{
  GameData();

  bool Load(std::wstring const & filePath);
  const std::vector<std::string> * GetParsingMessages() const;

  std::string GetEngineer(std::string const & system) const;
  Region GetRegion(std::string const & system) const;

  static uint8_t const MinPriority = 1;
  static uint8_t const MaxPriority = 9;

  std::set<std::string> selectedEngineers;
  std::map<std::string, ModuleSelect> selectedModules;

  std::string startSystem;

  SystemMap systems;
  ModuleMap modules;
  EngineerMap engineers;
};

extern GameData g_GameData;

char const * ToString(Region);

#endif