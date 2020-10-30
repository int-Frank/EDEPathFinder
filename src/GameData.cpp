
#include <fstream>

#include "GameData.h"

GameData g_GameData;

static std::vector<std::string> errorMessages = {};

#define PUSH_ERROR_MESSAGE(str, ...) do {char _buf[256] = {}; sprintf(_buf, str, __VA_ARGS__); errorMessages.push_back(std::string(_buf));} while (false)
#define KILL(str, ...) do { PUSH_ERROR_MESSAGE(str, __VA_ARGS__); return false;} while (false)

#define GET_NEXT(c) do {if (!ifs.get(c).good()) KILL("GET_NEXT failed to read a character");} while (false)
#define GET_NEXT_IGNORE_WS(c) do {ifs >> std::ws >> c; if (!ifs.good()) KILL("GET_NEXT_IGNORE_WS failed to read a character");} while (false)
#define CHECK(fn) do {if (!fn) KILL(#fn " failed" );} while(false)
#define PEEK(c) do {ifs >> std::ws; int intChar = ifs.peek(); if (intChar == EOF) KILL("PEEK failed to read a character"); c = char(intChar);} while (false)
#define ASSERT_NEXT(c)      do {char _tempChar = 0; PEEK(_tempChar); if (_tempChar != c) KILL("Invalid token. Expected '%c' got '%c'", c, _tempChar); if (!ifs.good()) KILL("ASSERT_NEXT failed to read character."); GET_NEXT_IGNORE_WS(_tempChar);} while (false)
#define DISCARD_NEXT_IF(c)  do {char _tempChar = 0; PEEK(_tempChar); if (_tempChar == c) GET_NEXT_IGNORE_WS(_tempChar);} while (false)
#define BREAK_ON(c) {char _tempChar = 0; PEEK(_tempChar); if (_tempChar == c) {GET_NEXT_IGNORE_WS(_tempChar); break;}}
#define READ_NUMBER(x) do {ifs >> x; if (!ifs.good()) KILL("Failed to read a number");} while (false)

GameData::GameData()
{
  
}

std::string GameData::GetEngineer(std::string const & system) const
{
  for (auto const & kv : engineers)
  {
    if (kv.second.system == system)
      return kv.first;
  }
  return "";
}

bool ReadString(std::ifstream & ifs, std::string & str)
{
  str.clear();
  char c = 0;

  ASSERT_NEXT('\"');

  while (true)
  {
    GET_NEXT(c);
    if (c == '\"')
      break;

    if (c == '\\')
      GET_NEXT(c);

    str.push_back(c);
  }
  return true;
}

bool ReadFloat3(std::ifstream & ifs, Float3 & v)
{
  char c = 0;

  ASSERT_NEXT('[');

  ifs >> v[0] >> c >> v[1] >> c >> v[2] >> c;

  return (ifs.good() && c == ']');
}

bool DiscardObject(std::ifstream & ifs)
{
  char c = 0;
  int level = 0;
  do
  {
    GET_NEXT_IGNORE_WS(c);

    if (c == '{')
      level++;
    else if (c == '}')
      level--;

  } while (level != 0);

  return true;
}

bool DiscardList(std::ifstream & ifs)
{
  char c = 0;
  do
  {
    GET_NEXT_IGNORE_WS(c);
  } while (c != ']');

  return true;
}

bool DiscardNextObject(std::ifstream & ifs)
{
  char c = 0;
  std::string str;

  PEEK(c);
  if (c == '[')
    CHECK(DiscardList(ifs));
  else if (c == '{')
    CHECK(DiscardObject(ifs));
  else if (c == '"')
    CHECK(ReadString(ifs, str));
  else
    return false;

  return true;
}

bool ReadSystem(std::ifstream & ifs, std::string & name, SystemData & data)
{
  ASSERT_NEXT('{');

  while (true)
  {
    BREAK_ON('}');

    std::string str;
    CHECK(ReadString(ifs, str));
    ASSERT_NEXT(':');

    if (str == "name")
    {
      CHECK(ReadString(ifs, name));
    }
    else if (str == "position")
    {
      CHECK(ReadFloat3(ifs, data.position));
    }
    else
    {
      PUSH_ERROR_MESSAGE("WARNING: Unknown tag found when reading a system: '%s'", str.c_str());
      DiscardNextObject(ifs);
    }

    DISCARD_NEXT_IF(',');
  };

  return true;
}

bool ReadSystems(std::ifstream & ifs, SystemMap & systemMap)
{
  ASSERT_NEXT('[');

  while (true)
  {
    BREAK_ON(']');

    SystemData data ={};
    std::string name;
    CHECK(ReadSystem(ifs, name, data));

    if (systemMap.find(name) != systemMap.end())
      PUSH_ERROR_MESSAGE("WARNING: Duplicate system found: '%s'", name.c_str());
    else
      systemMap[name] = data;

    DISCARD_NEXT_IF(',');
  }

  return true;
}

bool ReadModule(std::ifstream & ifs, std::string & name, ModuleData & data)
{
  ASSERT_NEXT('{');

  while (true)
  {
    BREAK_ON('}');

    std::string str;
    CHECK(ReadString(ifs, str));
    ASSERT_NEXT(':');

    if (str == "name")
    {
      CHECK(ReadString(ifs, name));
    }
    else if (str == "class")
    {
      CHECK(ReadString(ifs, data.moduleClass));
    }
    else
    {
      PUSH_ERROR_MESSAGE("WARNING: Unknown tag found when reading a module: '%s'", str.c_str());
      DiscardNextObject(ifs);
    }

    DISCARD_NEXT_IF(',');
  };

  return true;
}

bool ReadModules(std::ifstream & ifs, ModuleMap & moduleMap)
{
  ASSERT_NEXT('[');

  while (true)
  {
    BREAK_ON(']');

    ModuleData data ={};
    std::string name;
    CHECK(ReadModule(ifs, name, data));

    if (moduleMap.find(name) != moduleMap.end())
      PUSH_ERROR_MESSAGE("WARNING: Duplicate module found: '%s'", name.c_str());
    else
      moduleMap[name] = data;

    DISCARD_NEXT_IF(',');
  }

  return true;
}

bool ReadEngineerModule(std::ifstream & ifs, std::string & name, int & grade)
{
  ASSERT_NEXT('{');

  while (true)
  {
    BREAK_ON('}');

    std::string str;
    CHECK(ReadString(ifs, str));
    ASSERT_NEXT(':');

    if (str == "name")
    {
      CHECK(ReadString(ifs, name));
    }
    else if (str == "grade")
    {
      int temp = -1;
      READ_NUMBER(grade);
      if (temp > 0 && temp <= 5)
        grade = temp;
    }
    else
    {
      PUSH_ERROR_MESSAGE("WARNING: Unknown tag found when reading a module: '%s'", str.c_str());
      DiscardNextObject(ifs);
    }

    DISCARD_NEXT_IF(',');
  };

  return true;
}

bool ReadEngineerModules(std::ifstream & ifs, std::map<std::string, int> & modules)
{
  ASSERT_NEXT('[');

  while (true)
  {
    BREAK_ON(']');

    int grade = -1;
    std::string name;
    CHECK(ReadEngineerModule(ifs, name, grade));

    if (modules.find(name) != modules.end())
      PUSH_ERROR_MESSAGE("WARNING: Duplicate module grade found: '%s'", name.c_str());
    else
      modules[name] = grade;

    DISCARD_NEXT_IF(',');
  }

  return true;
}

bool ReadEngineer(std::ifstream & ifs, std::string & name, EngineerData & data)
{
  ASSERT_NEXT('{');

  while (true)
  {
    BREAK_ON('}');

    std::string str;
    CHECK(ReadString(ifs, str));
    ASSERT_NEXT(':');

    if (str == "name")
    {
      CHECK(ReadString(ifs, name));
    }
    else if (str == "system")
    {
      CHECK(ReadString(ifs, data.system));
    }
    else if (str == "class")
    {
      CHECK(ReadString(ifs, data.Class));
    }
    else if (str == "modules")
    {
      CHECK(ReadEngineerModules(ifs, data.moduleGrades));
    }
    else
    {
      PUSH_ERROR_MESSAGE("WARNING: Unknown tag found when reading a module: '%s'", str.c_str());
      DiscardNextObject(ifs);
    }

    DISCARD_NEXT_IF(',');
  };

  return true;
}

bool ReadEngineers(std::ifstream & ifs, EngineerMap & engineerMap)
{
  ASSERT_NEXT('[');

  while (true)
  {
    BREAK_ON(']');

    EngineerData data ={};
    std::string name;
    CHECK(ReadEngineer(ifs, name, data));

    if (engineerMap.find(name) != engineerMap.end())
      PUSH_ERROR_MESSAGE("WARNING: Duplicate engineer found: '%s'", name.c_str());
    else
      engineerMap[name] = data;

    DISCARD_NEXT_IF(',');
  }

  return true;
}

bool GameData::Load(std::wstring const & filePath)
{
  std::ifstream ifs(filePath);

  if (!ifs.good())
    return false;

  char c;
  std::string str;

  ASSERT_NEXT('{');

  do
  {
    if (!ReadString(ifs, str))
      return false;

    ASSERT_NEXT(':');

    if (str == "systems")
    {
      CHECK(ReadSystems(ifs, systems));
    }
    else if (str == "modules")
    {
      CHECK(ReadModules(ifs, modules));
    }
    else if (str == "engineers")
    {
      CHECK(ReadEngineers(ifs, engineers));
    }
    else
    {
      PUSH_ERROR_MESSAGE("WARNING: Unknown tag found in root: '%s'", str.c_str());
      CHECK(DiscardNextObject(ifs));
    }

    DISCARD_NEXT_IF(',');
    PEEK(c);
  } while (c != '}');

  engineerClasses.clear();
  for (auto const & kv : engineers)
    engineerClasses.insert(kv.second.Class);

  return true;
}

const std::vector<std::string> * GameData::GetParsingMessages() const
{
  return &errorMessages;
}