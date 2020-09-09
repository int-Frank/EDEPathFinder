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
IncludeDir["ImGui"] = "%{wks.location}/3rdParty/ImGui"

group "Depenencies"

project "ImGui"
    location "3rdParty/ImGui"
    kind "StaticLib"
    targetdir (projOutput)
    objdir (projOutputInt)
    systemversion "latest"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"
  
    files 
    {
      "3rdParty/ImGui/**.h",
      "3rdParty/ImGui/**.cpp"
    }
  
    filter "configurations:Debug"
		  runtime "Debug"
		  symbols "on"

	  filter "configurations:Release"
		  runtime "Release"
		  optimize "on"

group ""

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
    "src/**.h",
    "src/**.cpp",
  }

  links
  {
    "ImGui",
    "3rdParty/SDL2/lib/x64/SDL2.lib",
    "3rdParty/SDL2/lib/x64/SDL2main.lib"
  }

  includedirs
  {
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.SDL2}"
  }
  
  postbuildcommands 
  {
    "{COPY} %{wks.location}/3rdParty/SDL2/lib/x64/SDL2.dll %{cfg.targetdir}"
  }
  
  filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"