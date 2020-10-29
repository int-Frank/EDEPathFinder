#ifndef PATH_H
#define PATH_H

#include <vector>

#include "GameData.h"
#include "utils.h"

struct ModuleNode
{
  std::string moduleName;
  int grade;
};

struct SystemNode
{
  std::string name;
  std::vector<ModuleNode> modules;
};

std::vector<SystemNode> FindBestRoute();
float PathDistance(std::vector<SystemNode> const &);

#endif