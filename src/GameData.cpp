
#include <cstring>

#include "GameData.h"

#undef ITEM1
#define ITEM1(a) #a,

static char const * s_Region[SystemCOUNT] =
{
  UNROLL_REGIONS
};

static char const * s_ModuleClass[ModuleClassCOUNT] =
{
  UNROLL_MODULECLASSES
};

#undef ITEM2
#undef ITEM3
#define ITEM2(a, c) #a,
#define ITEM3(a, c, str) str,

static char const * s_ModuleName[ModuleCOUNT] =
{
  UNROLL_MODULES
};

static char const * s_EngineerName[EngineerCOUNT] =
{
  UNROLL_ENGINEERS
};

static char const * s_SystemName[SystemCOUNT] =
{
  UNROLL_SYSTEMS
};

GameData g_GameData;

GameData::GameData()
  : selectedEngineers(0)
  , selectedModules(0)
  , engineerPriorities{}
  , moduleUpgradeLevel{}
  , startSystem(sShinrartaDezhra)
  , engineer{}
  , system{}
{
  for (uint32_t e = 0; e < EngineerCOUNT; e++)
    engineerPriorities[e] = uint8_t(1);

  for (uint32_t m = 0; m < ModuleCOUNT; m++)
    moduleUpgradeLevel[m] = 0;

  //--------------------------------------------------------------------
  // Engineers
  engineer[eElviraMartuuk].system = sKhun;
  engineer[eElviraMartuuk].moduleGrade[mFrameShiftDrive] = 5;
  engineer[eElviraMartuuk].moduleGrade[mShieldGenerator] = 3;
  engineer[eElviraMartuuk].moduleGrade[mThrusters] = 2;
  engineer[eElviraMartuuk].moduleGrade[mShieldCellBank] = 1;

  engineer[eTheDweller].system = sWyrd;
  engineer[eTheDweller].moduleGrade[mPowerDistributor] = 5;
  engineer[eTheDweller].moduleGrade[mPulseLaser] = 4;
  engineer[eTheDweller].moduleGrade[mBurstLaser] = 3;
  engineer[eTheDweller].moduleGrade[mBeamLaser] = 3;

  engineer[eFelicityFarseer].system = sDeciat;
  engineer[eFelicityFarseer].moduleGrade[mFrameShiftDrive] = 5;
  engineer[eFelicityFarseer].moduleGrade[mThrusters] = 3;
  engineer[eFelicityFarseer].moduleGrade[mSensors] = 3;
  engineer[eFelicityFarseer].moduleGrade[mDetailedSurfaceScanner] = 3;
  engineer[eFelicityFarseer].moduleGrade[mShieldBooster] = 1;
  engineer[eFelicityFarseer].moduleGrade[mFSDInterdictor] = 1;
  engineer[eFelicityFarseer].moduleGrade[mPowerPlant] = 1;

  engineer[eTodMcquinn].system = sWolf397;
  engineer[eTodMcquinn].moduleGrade[mMultiCannon] = 5;
  engineer[eTodMcquinn].moduleGrade[mRailGun] = 5;
  engineer[eTodMcquinn].moduleGrade[mFragmentCannon] = 3;
  engineer[eTodMcquinn].moduleGrade[mCannon] = 2;

  engineer[eLizRyder].system = sEurybia;
  engineer[eLizRyder].moduleGrade[mSeekerMissileRack] = 5;
  engineer[eLizRyder].moduleGrade[mTorpedoPylon] = 5;
  engineer[eLizRyder].moduleGrade[mMissileRack] = 5;
  engineer[eLizRyder].moduleGrade[mMineLauncher] = 3;
  engineer[eLizRyder].moduleGrade[mHullReinforcementPackage] = 1;
  engineer[eLizRyder].moduleGrade[mArmour] = 1;

  engineer[eMelBrandon].system = sLuchtaine;
  engineer[eMelBrandon].moduleGrade[mFrameShiftDrive] = 5;
  engineer[eMelBrandon].moduleGrade[mThrusters] = 5;
  engineer[eMelBrandon].moduleGrade[mShieldGenerator] = 5;
  engineer[eMelBrandon].moduleGrade[mBurstLaser] = 5;
  engineer[eMelBrandon].moduleGrade[mPulseLaser] = 5;
  engineer[eMelBrandon].moduleGrade[mBeamLaser] = 5;
  engineer[eMelBrandon].moduleGrade[mFSDInterdictor] = 5;
  engineer[eMelBrandon].moduleGrade[mShieldBooster] = 5;
  engineer[eMelBrandon].moduleGrade[mShieldCellBank] = 4;

  engineer[eMashaHicks].system = sTir;
  engineer[eMashaHicks].moduleGrade[mCollectorLimpetColtroller] = 5;
  engineer[eMashaHicks].moduleGrade[mProspectorLimpetController] = 5;
  engineer[eMashaHicks].moduleGrade[mRefinery] = 5;
  engineer[eMashaHicks].moduleGrade[mFuelScoop] = 5;
  engineer[eMashaHicks].moduleGrade[mMultiCannon] = 5;
  engineer[eMashaHicks].moduleGrade[mFuelTransferLimpetController] = 4;
  engineer[eMashaHicks].moduleGrade[mHatchBreakerLimpetController] = 4;
  engineer[eMashaHicks].moduleGrade[mCannon] = 4;
  engineer[eMashaHicks].moduleGrade[mFragmentCannon] = 4;

  engineer[eJuriIshmaak].system = sGiryak;
  engineer[eJuriIshmaak].moduleGrade[mMineLauncher] = 5;
  engineer[eJuriIshmaak].moduleGrade[mSensors] = 5;
  engineer[eJuriIshmaak].moduleGrade[mDetailedSurfaceScanner] = 5;
  engineer[eJuriIshmaak].moduleGrade[mTorpedoPylon] = 3;
  engineer[eJuriIshmaak].moduleGrade[mSeekerMissileRack] = 3;
  engineer[eJuriIshmaak].moduleGrade[mMissileRack] = 3;
  engineer[eJuriIshmaak].moduleGrade[mManifestScanner] = 3;
  engineer[eJuriIshmaak].moduleGrade[mFrameShiftWakeScanner] = 3;
  engineer[eJuriIshmaak].moduleGrade[mKillWarrentScanner] = 3;

  engineer[ePetraOlmanova].system = sAsura;
  engineer[ePetraOlmanova].moduleGrade[mHullReinforcementPackage] = 5;
  engineer[ePetraOlmanova].moduleGrade[mSeekerMissileRack] = 5;
  engineer[ePetraOlmanova].moduleGrade[mArmour] = 5;
  engineer[ePetraOlmanova].moduleGrade[mMissileRack] = 5;
  engineer[ePetraOlmanova].moduleGrade[mHeatSinkLauncher] = 5;
  engineer[ePetraOlmanova].moduleGrade[mPointDefence] = 5;
  engineer[ePetraOlmanova].moduleGrade[mMineLauncher] = 4;
  engineer[ePetraOlmanova].moduleGrade[mTorpedoPylon] = 4;
  engineer[ePetraOlmanova].moduleGrade[mChaffLauncher] = 4;
  engineer[ePetraOlmanova].moduleGrade[mElectronicCounterMeasures] = 4;
  engineer[ePetraOlmanova].moduleGrade[mAutoFieldMaintenanceUnit] = 4;

  engineer[eEtienneDorn].system = sLos;
  engineer[eEtienneDorn].moduleGrade[mPlasmaAccelerator] = 5;
  engineer[eEtienneDorn].moduleGrade[mSensors] = 5;
  engineer[eEtienneDorn].moduleGrade[mLifeSupport] = 5;
  engineer[eEtienneDorn].moduleGrade[mPowerPlant] = 5;
  engineer[eEtienneDorn].moduleGrade[mPowerDistributor] = 5;
  engineer[eEtienneDorn].moduleGrade[mFrameShiftWakeScanner] = 5;
  engineer[eEtienneDorn].moduleGrade[mRailGun] = 5;
  engineer[eEtienneDorn].moduleGrade[mDetailedSurfaceScanner] = 4;
  engineer[eEtienneDorn].moduleGrade[mKillWarrentScanner] = 4;
  engineer[eEtienneDorn].moduleGrade[mManifestScanner] = 4;

  engineer[eZacariahNemo].system = sYoru;
  engineer[eZacariahNemo].moduleGrade[mFragmentCannon] = 5;
  engineer[eZacariahNemo].moduleGrade[mMultiCannon] = 3;
  engineer[eZacariahNemo].moduleGrade[mPlasmaAccelerator] = 2;

  engineer[eLeiChung].system = sLaksak;
  engineer[eLeiChung].moduleGrade[mShieldGenerator] = 5;
  engineer[eLeiChung].moduleGrade[mSensors] = 5;
  engineer[eLeiChung].moduleGrade[mDetailedSurfaceScanner] = 5;
  engineer[eLeiChung].moduleGrade[mShieldBooster] = 3;

  engineer[eColonelBrisDecker].system = sSol;
  engineer[eColonelBrisDecker].moduleGrade[mFSDInterdictor] = 4;
  engineer[eColonelBrisDecker].moduleGrade[mFrameShiftDrive] = 4;

  engineer[eSeleneJean].system = sKuk;
  engineer[eSeleneJean].moduleGrade[mHullReinforcementPackage] = 5;
  engineer[eSeleneJean].moduleGrade[mArmour] = 5;

  engineer[eHeraTani].system = sKuwemaki;
  engineer[eHeraTani].moduleGrade[mPowerPlant] = 5;
  engineer[eHeraTani].moduleGrade[mDetailedSurfaceScanner] = 5;
  engineer[eHeraTani].moduleGrade[mSensors] = 3;
  engineer[eHeraTani].moduleGrade[mPowerDistributor] = 3;

  engineer[eMarcoQwent].system = sSirius;
  engineer[eMarcoQwent].moduleGrade[mPowerPlant] = 4;
  engineer[eMarcoQwent].moduleGrade[mPowerDistributor] = 3;

  engineer[eRamTah].system = sMeene;
  engineer[eRamTah].moduleGrade[mElectronicCounterMeasures] = 5;
  engineer[eRamTah].moduleGrade[mPointDefence] = 5;
  engineer[eRamTah].moduleGrade[mHeatSinkLauncher] = 5;
  engineer[eRamTah].moduleGrade[mChaffLauncher] = 5;
  engineer[eRamTah].moduleGrade[mCollectorLimpetColtroller] = 4;
  engineer[eRamTah].moduleGrade[mFuelTransferLimpetController] = 4;
  engineer[eRamTah].moduleGrade[mProspectorLimpetController] = 4;
  engineer[eRamTah].moduleGrade[mHatchBreakerLimpetController] = 3;

  engineer[eTheSarge].system = sBeta3Tucani;
  engineer[eTheSarge].moduleGrade[mCollectorLimpetColtroller] = 5;
  engineer[eTheSarge].moduleGrade[mFuelTransferLimpetController] = 5;
  engineer[eTheSarge].moduleGrade[mHatchBreakerLimpetController] = 5;
  engineer[eTheSarge].moduleGrade[mProspectorLimpetController] = 5;
  engineer[eTheSarge].moduleGrade[mCannon] = 5;
  engineer[eTheSarge].moduleGrade[mRailGun] = 3;

  engineer[eDidiVatermann].system = sLeesti;
  engineer[eDidiVatermann].moduleGrade[mShieldBooster] = 5;
  engineer[eDidiVatermann].moduleGrade[mShieldGenerator] = 3;

  engineer[eBrooTarquin].system = sMuang;
  engineer[eBrooTarquin].moduleGrade[mBurstLaser] = 5;
  engineer[eBrooTarquin].moduleGrade[mPulseLaser] = 5;
  engineer[eBrooTarquin].moduleGrade[mBeamLaser] = 5;

  engineer[eProfessorPalin].system = sArque;
  engineer[eProfessorPalin].moduleGrade[mThrusters] = 5;
  engineer[eProfessorPalin].moduleGrade[mFrameShiftDrive] = 3;

  engineer[eLoriJameson].system = sShinrartaDezhra;
  engineer[eLoriJameson].moduleGrade[mSensors] = 5;
  engineer[eLoriJameson].moduleGrade[mDetailedSurfaceScanner] = 5;
  engineer[eLoriJameson].moduleGrade[mRefinery] = 4;
  engineer[eLoriJameson].moduleGrade[mFuelScoop] = 4;
  engineer[eLoriJameson].moduleGrade[mAutoFieldMaintenanceUnit] = 4;
  engineer[eLoriJameson].moduleGrade[mLifeSupport] = 4;
  engineer[eLoriJameson].moduleGrade[mFrameShiftWakeScanner] = 3;
  engineer[eLoriJameson].moduleGrade[mKillWarrentScanner] = 3;
  engineer[eLoriJameson].moduleGrade[mManifestScanner] = 3;
  engineer[eLoriJameson].moduleGrade[mShieldCellBank] = 3;

  engineer[eChloeSedesi].system = sShenve;
  engineer[eChloeSedesi].moduleGrade[mThrusters] = 5;
  engineer[eChloeSedesi].moduleGrade[mFrameShiftDrive] = 3;

  engineer[eBillTurner].system = sAlioth;
  engineer[eBillTurner].moduleGrade[mPlasmaAccelerator] = 5;
  engineer[eBillTurner].moduleGrade[mSensors] = 5;
  engineer[eBillTurner].moduleGrade[mDetailedSurfaceScanner] = 5;
  engineer[eBillTurner].moduleGrade[mLifeSupport] = 3;
  engineer[eBillTurner].moduleGrade[mRefinery] = 3;
  engineer[eBillTurner].moduleGrade[mAutoFieldMaintenanceUnit] = 3;
  engineer[eBillTurner].moduleGrade[mFuelScoop] = 3;
  engineer[eBillTurner].moduleGrade[mFrameShiftDrive] = 3;
  engineer[eBillTurner].moduleGrade[mKillWarrentScanner] = 3;
  engineer[eBillTurner].moduleGrade[mManifestScanner] = 3;

  engineer[eTianaFortune].system = sAchenar;
  engineer[eTianaFortune].moduleGrade[mFrameShiftWakeScanner] = 5;
  engineer[eTianaFortune].moduleGrade[mKillWarrentScanner] = 5;
  engineer[eTianaFortune].moduleGrade[mManifestScanner] = 5;
  engineer[eTianaFortune].moduleGrade[mCollectorLimpetColtroller] = 5;
  engineer[eTianaFortune].moduleGrade[mFuelTransferLimpetController] = 5;
  engineer[eTianaFortune].moduleGrade[mHatchBreakerLimpetController] = 5;
  engineer[eTianaFortune].moduleGrade[mProspectorLimpetController] = 5;
  engineer[eTianaFortune].moduleGrade[mSensors] = 5;
  engineer[eTianaFortune].moduleGrade[mFSDInterdictor] = 3;
  engineer[eTianaFortune].moduleGrade[mDetailedSurfaceScanner] = 3;

  //--------------------------------------------------------------------
  // Systems
  system[sKhun].position.Set(-171.59375f, 19.96875f, -56.96875f);
  system[sWyrd].position.Set(-11.625f, 31.53125f, -3.9375f);
  system[sDeciat].position.Set(122.625f, -0.8125f, -47.28125f);
  system[sWolf397].position.Set(40.0f, 79.21875f, -10.40625f);
  system[sEurybia].position.Set(51.40625f, -54.40625f, -30.5f);
  system[sLuchtaine].position.Set(-9523.3125f, -914.46875f, 19825.90625);
  system[sTir].position.Set(-9532.9375f, -923.4375f, 19799.125);
  system[sGiryak].position.Set(14.6875f, 27.65625f, 108.65625);
  system[sAsura].position.Set(-9550.28125f, -916.65625f, 19816.1875);
  system[sLos].position.Set(-9509.34375f, -886.3125f, 19820.125);
  system[sYoru].position.Set(97.875f, -86.90625f, 64.125);
  system[sLaksak].position.Set(-21.53125f, -6.3125f, 116.03125);
  system[sSol].position.Set(0.0f, 0.0f, 0.0f);
  system[sKuk].position.Set(-21.28125f, 69.09375f, -16.3125);
  system[sKuwemaki].position.Set(134.65625f, -226.90625f, -7.8125);
  system[sSirius].position.Set(6.25f, -1.28125f, -5.75);
  system[sMeene].position.Set(118.78125f, -56.4375f, -97.1875);
  system[sBeta3Tucani].position.Set(32.25f, -55.1875f, 23.875);
  system[sLeesti].position.Set(72.75f, 48.75f, 68.25);
  system[sMuang].position.Set(17.03125f, -172.78125f, -3.46875);
  system[sArque].position.Set(66.5f, 38.0625f, 61.125);
  system[sShinrartaDezhra].position.Set(55.71875f, 17.59375f, 27.15625);
  system[sShenve].position.Set(351.96875f, -373.46875f, -711.09375);
  system[sAlioth].position.Set(-33.65625f, 72.46875f, -20.65625);
  system[sAchenar].position.Set(67.5f, -119.46875f, 24.84375);
}

ModuleClass GetModuleClass(ModuleName name)
{
  static ModuleClass moduleClass[ModuleCOUNT] = 
  {
#undef ITEM2
#undef ITEM3
#define ITEM2(a, c) mc##c,
#define ITEM3(a, c, str) mc##c,

    UNROLL_MODULES
  };

  return moduleClass[name];
}

SystemName GetSystem(EngineerName name)
{
  static SystemName system[EngineerCOUNT] =
  {
#undef ITEM2
#undef ITEM3
#define ITEM2(a, sys) s##sys,
#define ITEM3(a, sys, str) s##sys,

    UNROLL_ENGINEERS
  };

  return system[name];
}

EngineerName GetEngineer(SystemName name)
{
  for (uint32_t i = 0; i < EngineerCOUNT; ++i)
  {
    if (GetSystem(EngineerName(i)) == name)
      return EngineerName(i);
  }
  return EngineerCOUNT; //ERROR!
}

Region GetRegion(SystemName name)
{
  static Region systemRegion[SystemCOUNT] =
  {
#undef ITEM2
#undef ITEM3
#define ITEM2(a, reg) r##reg,
#define ITEM3(a, reg, str) r##reg,

    UNROLL_SYSTEMS
  };

  return systemRegion[name];
}

char const * ToString(Region name)
{
  return s_Region[name];
}

char const * ToString(ModuleClass name)
{
  return s_ModuleClass[name];
}

char const * ToString(ModuleName name)
{
  return s_ModuleName[name];
}

char const * ToString(EngineerName name)
{
  return s_EngineerName[name];
}

char const * ToString(SystemName name)
{
  return s_SystemName[name];
}

bool ToRegion(char const * pStr, Region & out)
{
  for (uint32_t i = 0; i < RegionCOUNT; i++)
  {
    if (strcmp(pStr, s_Region[i]) == 0)
    {
      out = static_cast<Region>(i);
      return true;
    }
  }
  return false;
}

bool ToModuleClass(char const * pStr, ModuleClass & out)
{
  for (uint32_t i = 0; i < ModuleClassCOUNT; i++)
  {
    if (strcmp(pStr, s_ModuleClass[i]) == 0)
    {
      out = static_cast<ModuleClass>(i);
      return true;
    }
  }
  return false;
}

bool ToModuleName(char const * pStr, ModuleName & out)
{
  for (uint32_t i = 0; i < ModuleCOUNT; i++)
  {
    if (strcmp(pStr, s_ModuleName[i]) == 0)
    {
      out = static_cast<ModuleName>(i);
      return true;
    }
  }
  return false;
}

bool ToEngineerName(char const * pStr, EngineerName & out)
{
  for (uint32_t i = 0; i < EngineerCOUNT; i++)
  {
    if (strcmp(pStr, s_EngineerName[i]) == 0)
    {
      out = static_cast<EngineerName>(i);
      return true;
    }
  }
  return false;
}

bool ToSystemName(char const * pStr, SystemName & out)
{
  for (uint32_t i = 0; i < SystemCOUNT; i++)
  {
    if (strcmp(pStr, s_SystemName[i]) == 0)
    {
      out = static_cast<SystemName>(i);
      return true;
    }
  }
  return false;
}