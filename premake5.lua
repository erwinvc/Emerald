workspace "Emerald"
	architecture "x64"
	configurations { "Debug", "Release", "Dist" }
	startproject "Editor"
	
	filter "system:windows"
		buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }
		
	flags { "MultiProcessorCompile" }

    filter "configurations:Release"
		linktimeoptimization "On"
	
OutputDir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

group "Engine"
	include "engine/dependencies/GLAD"
	include "engine/dependencies/GLFW"
	include "engine"
group ""

include "editor"
include "runtime"