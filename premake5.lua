workspace "EDEngineerPaths"
  location ""
  architecture "x64"
  
  startproject "EDEngineerPaths"
  
  configurations
  {
    "Debug",
    "Release"
  }
  
projOutput = "%{wks.location}/build/%{prj.name}-%{cfg.buildcfg}"
projOutputInt = "%{wks.location}/build/intermediate/%{prj.name}-%{cfg.buildcfg}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["SDL2"] = "%{wks.location}/3rdParty/SDL2/include"

project "EDEngineerPaths"
  location "./"
  kind "WindowedApp"
  targetdir (projOutput)
  objdir (projOutputInt)
  systemversion "latest"
  language "C++"
  cppdialect "C++17"
  staticruntime "on"
  
  files 
  {
    "src/**.rc",
    "src/**.h",
    "src/**.inl",
    "src/**.cpp",
    "src/ImGui/**.h",
    "src/ImGui/**.cpp",
  }

  vpaths { ["Resources"] = {"src/*.rc", "src/resource.h"}}
  -- vpaths { ["group"] = "pattern(s)" }

  links
  {
    "3rdParty/SDL2/lib/x64/SDL2.lib",
    "3rdParty/SDL2/lib/x64/SDL2main.lib"
  }

  includedirs
  {
		"%{IncludeDir.SDL2}"
  }
  
  postbuildcommands 
  {
    "{COPY} %{wks.location}/3rdParty/SDL2/lib/x64/SDL2.dll %{cfg.targetdir}"
  }
  
  defines     { "_CRT_SECURE_NO_WARNINGS" }
  
  filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"