#ifndef PATH_H
#define PATH_H

#include <vector>
#include <map>

#include "GameData.h"
#include "utils.h"

class ModuleGrade
{
public:

  ModuleGrade();
  ModuleGrade(ModuleName, uint8_t grade);

  void SetName(ModuleName);
  void SetGrade(uint8_t);
  ModuleName Name() const;
  uint8_t Grade() const;

public:
  uint16_t m_data;
};

struct SystemNode
{
  SystemName name;
  std::vector<ModuleGrade> modules;
};

std::vector<SystemNode> FindShortestPath();
float PathDistance(std::vector<SystemNode> const &);

#endif