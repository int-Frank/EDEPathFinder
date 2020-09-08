#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <stdint.h>

#include "utils.h"

// MSCV Macro overloading workaround retrieved from https://stackoverflow.com/questions/9183993/msvc-variadic-macro-expansion
#define GLUE(x, y) x y

#define RETURN_ARG_COUNT(_1_, _2_, _3, count, ...) count
#define EXPAND_ARGS(args) RETURN_ARG_COUNT args
#define COUNT_ARGS_MAX5(...) EXPAND_ARGS((__VA_ARGS__, 3, 2, 1, 0))

#define OVERLOAD_MACRO2(name, count) name##count
#define OVERLOAD_MACRO1(name, count) OVERLOAD_MACRO2(name, count)
#define OVERLOAD_MACRO(name, count) OVERLOAD_MACRO1(name, count)

#define CALL_OVERLOAD(name, ...) GLUE(OVERLOAD_MACRO(name, COUNT_ARGS_MAX5(__VA_ARGS__)), (__VA_ARGS__))

#define ITEM(...) CALL_OVERLOAD(ITEM, __VA_ARGS__)

#define UNROLL_MODULES \
ITEM(Armour,                        Core)\
ITEM(PowerPlant,                    Core,     "Power Plant")\
ITEM(Thrusters,                     Core)\
ITEM(FrameShiftDrive,               Core,     "Frame Shift Drive")\
ITEM(LifeSupport,                   Core,     "Life Support")\
ITEM(PowerDistributor,              Core,     "Power Distributor")\
ITEM(Sensors,                       Core)\
ITEM(ShieldGenerator,               Optional, "Shield Generator")\
ITEM(FSDInterdictor,                Optional, "Frame Shift Drive Interdictor")\
ITEM(ShieldCellBank,                Optional, "Shield Cell Bank")\
ITEM(CollectorLimpetColtroller,     Optional, "Collector Limpet Controller")\
ITEM(ProspectorLimpetController,    Optional, "Prospector Limpet Controller")\
ITEM(FuelTransferLimpetController,  Optional, "Fuel Transfer Limpet Controller")\
ITEM(HatchBreakerLimpetController,  Optional, "Hatch Breaker Limpet Controller")\
ITEM(AutoFieldMaintenanceUnit,      Optional, "Auto Field-Maintenance Unit")\
ITEM(Refinery,                      Optional)\
ITEM(FuelScoop,                     Optional, "Fuel Scoop")\
ITEM(DetailedSurfaceScanner,        Optional, "Detailed Surface Scanner")\
ITEM(HullReinforcementPackage,      Optional, "Hull Reinforcement Package")\
ITEM(BurstLaser,                    Weapon,   "Burst Laser")\
ITEM(PulseLaser,                    Weapon,   "Pulse Laser")\
ITEM(BeamLaser,                     Weapon,   "Beam Laser")\
ITEM(MultiCannon,                   Weapon,   "Multi-Cannon")\
ITEM(RailGun,                       Weapon,   "Rail Gun")\
ITEM(FragmentCannon,                Weapon,   "Fragment Cannon")\
ITEM(MineLauncher,                  Weapon,   "Mine Launcher")\
ITEM(Cannon,                        Weapon)\
ITEM(PlasmaAccelerator,             Weapon,   "Plasma Accelerator")\
ITEM(SeekerMissileRack,             Weapon,   "Seeker Missile Rack")\
ITEM(TorpedoPylon,                  Weapon,   "Torpedo Pylon")\
ITEM(MissileRack,                   Weapon,   "Missile Rack")\
ITEM(FrameShiftWakeScanner,         Utility,  "Frame Shift Wake Scanner")\
ITEM(KillWarrentScanner,            Utility,  "Kill Warrent Scanner")\
ITEM(ManifestScanner,               Utility,  "Manifest Scanner")\
ITEM(HeatSinkLauncher,              Utility,  "Heat Sink Launcher")\
ITEM(ElectronicCounterMeasures,     Utility,  "Electronic Counter Measures")\
ITEM(PointDefence,                  Utility,  "Point Defence")\
ITEM(ChaffLauncher,                 Utility,  "Chaff Launcher")\
ITEM(ShieldBooster,                 Utility,  "Shiled Booster")

#define UNROLL_ENGINEERS \
ITEM(ElviraMartuuk,     Khun,             "Elvira Martuuk")\
ITEM(TheDweller,        Wyrd,             "The Dweller")\
ITEM(FelicityFarseer,   Deciat,           "Felicity Farseer")\
ITEM(TodMcquinn,        Wolf397,          "Tod \"The Blaster\" Mcquinn")\
ITEM(LizRyder,          Eurybia,          "Liz Ryder")\
ITEM(MelBrandon,        Luchtaine,        "Mel Brandon")\
ITEM(MashaHicks,        Tir,              "Masha Hicks")\
ITEM(JuriIshmaak,       Giryak,           "Juri Ishmaak")\
ITEM(PetraOlmanova,     Asura,            "Petra Olmanova")\
ITEM(EtienneDorn,       Los,              "Etienne Dorn")\
ITEM(ZacariahNemo,      Yoru,             "Zacariah Nemo")\
ITEM(LeiChung,          Laksak,           "Lei Chung")\
ITEM(ColonelBrisDecker, Sol,              "Colonel Bris Decker")\
ITEM(SeleneJean,        Kuk,              "Selene Jean")\
ITEM(HeraTani,          Kuwemaki,         "Hera Tani")\
ITEM(MarcoQwent,        Sirius,           "Marco Qwent")\
ITEM(RamTah,            Meene,            "Ram Tah")\
ITEM(TheSarge,          Beta3Tucani,      "The Sarge")\
ITEM(DidiVatermann,     Leesti,           "Didi Vatermann")\
ITEM(BrooTarquin,       Muang,            "Broo Tarquin")\
ITEM(ProfessorPalin,    Arque,            "Professor Palin")\
ITEM(LoriJameson,       ShinrartaDezhra,  "Lori Jameson")\
ITEM(ChloeSedesi,       Shenve,           "Chloe Sedesi")\
ITEM(BillTurner,        Alioth,           "Bill Turner")\
ITEM(TianaFortune,      Achenar,          "Tiana Fortune")

#define UNROLL_SYSTEMS \
ITEM(Khun,            Bubble)\
ITEM(Wyrd,            Bubble)\
ITEM(Deciat,          Bubble)\
ITEM(Wolf397,         Bubble, "Wolf 397")\
ITEM(Eurybia,         Bubble)\
ITEM(Luchtaine,       Colonia)\
ITEM(Tir,             Colonia)\
ITEM(Giryak,          Bubble)\
ITEM(Asura,           Colonia)\
ITEM(Los,             Colonia)\
ITEM(Yoru,            Bubble)\
ITEM(Laksak,          Bubble)\
ITEM(Sol,             Bubble)\
ITEM(Kuk,             Bubble)\
ITEM(Kuwemaki,        Bubble)\
ITEM(Sirius,          Bubble)\
ITEM(Meene,           Bubble)\
ITEM(Beta3Tucani,     Bubble, "Beta-3 Tucani")\
ITEM(Leesti,          Bubble)\
ITEM(Muang,           Bubble)\
ITEM(Arque,           Bubble)\
ITEM(ShinrartaDezhra, Bubble, "Shinrarta Dezhra")\
ITEM(Shenve,          Bubble)\
ITEM(Alioth,          Bubble)\
ITEM(Achenar,         Bubble)

#define UNROLL_MODULECLASSES \
ITEM(Core)\
ITEM(Optional)\
ITEM(Weapon)\
ITEM(Utility)

#define UNROLL_REGIONS \
ITEM(Bubble)\
ITEM(Colonia)

enum Region
{
#undef ITEM1
#define ITEM1(a) r ## a,
  UNROLL_REGIONS

  RegionCOUNT
};

enum ModuleClass
{
#undef ITEM1
#define ITEM1(a) mc ## a,
  UNROLL_MODULECLASSES

  ModuleClassCOUNT
};

enum ModuleName : uint32_t
{
#undef ITEM2
#undef ITEM3
#define ITEM2(a, c) m ## a,
#define ITEM3(a, c, str) m ## a,
  UNROLL_MODULES

  ModuleCOUNT
};

enum EngineerName : uint32_t
{
#undef ITEM2
#undef ITEM3
#define ITEM2(a, c) e ## a,
#define ITEM3(a, c, str) e ## a,
  UNROLL_ENGINEERS

  EngineerCOUNT
};

enum SystemName : uint32_t
{
#undef ITEM2
#undef ITEM3
#define ITEM2(a, c) s ## a,
#define ITEM3(a, c, str) s ## a,
  UNROLL_SYSTEMS

  SystemCOUNT
};

struct Engineer
{
  SystemName  system;
  uint32_t    moduleGrade[ModuleCOUNT];
};

struct System
{
  Float3 position;
};

struct GameData
{
  GameData();

  static uint8_t const MinPriority = 1;
  static uint8_t const MaxPriority = 9;

  uint64_t selectedEngineers;
  uint64_t selectedModules;
  uint8_t engineerPriorities[EngineerCOUNT];

  SystemName startSystem;

  Engineer engineer[EngineerCOUNT];
  System system[EngineerCOUNT];
};

extern GameData g_GameData;

ModuleClass GetModuleClass(ModuleName);
SystemName GetSystem(EngineerName);
EngineerName GetEngineer(SystemName);
Region GetRegion(SystemName);

char const * ToString(Region);
char const * ToString(ModuleClass);
char const * ToString(ModuleName);
char const * ToString(EngineerName);
char const * ToString(SystemName);

bool ToRegion(char const * pStr, Region & out);
bool ToModuleClass(char const * pStr, ModuleClass & out);
bool ToModuleName(char const * pStr, ModuleName & out);
bool ToEngineerName(char const * pStr, EngineerName & out);
bool ToSystemName(char const * pStr, SystemName & out);

#endif