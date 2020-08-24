workspace "Emerald"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"DebugServer",
		"ReleaseServer"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

flags{"MultiProcessorCompile"}

project "Emerald"
	location "Emerald"
	kind "ConsoleApp"
	language "C++"
	staticruntime "off"
	
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	
	files
	{
		"%{prj.name}/**.h",
		"%{prj.name}/**.hpp",
		"%{prj.name}/**.cpp",
		"%{prj.name}/**.vert",
		"%{prj.name}/**.frag",
		"%{prj.name}/**.geom",
		"%{prj.name}/**.tese",
		"%{prj.name}/**.tesc",
		"%{prj.name}/**.incl",
		"%{prj.name}/**.rc"
	}

	filter { "files:Emerald/vendor/**.cpp" }
		flags "NoPCH"	
	filter {}
	
	pchheader "stdafx.h"
	pchsource "Emerald/src/stdafx.cpp"
	
	includedirs
	{
		"%{prj.name}/vendor/AssImp/include",
		"%{prj.name}/vendor/AssImp/build/include",
		"%{prj.name}/vendor/GLFW/include",
		"%{prj.name}/vendor/GLEW/include",
		"%{prj.name}/vendor/imgui",
		"%{prj.name}/vendor/tracy",
		"%{prj.name}/vendor",
		"%{prj.name}",
		"%{prj.name}/src"
	}
	
	libdirs  
	{ 
		"$(SolutionDir)%{prj.name}/vendor/GLFW/lib-vc2015",
		"$(SolutionDir)%{prj.name}/vendor/AssImp",
		"$(SolutionDir)%{prj.name}/vendor/GLEW/lib/Release/x64"
	}

	links
	{
		"glfw3",
		"glew32s",
		"opengl32"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"GLEW_STATIC"
		}
		
	configuration "vs*"
		defines     { "_CRT_SECURE_NO_WARNINGS" }

	filter "configurations:Debug"
		defines "EE_DEBUG"
		runtime "Debug"
		symbols "On"

			links
			{
				"Emerald/vendor/assimp/bin/Debug/assimp-vc141-mtd.lib",
				"Emerald/vendor/assimp/bin/Debug/zlibstatic"
			}
			
			postbuildcommands 
			{
				'{COPY} "../Emerald/vendor/assimp/bin/Debug/assimp-vc141-mtd.dll" "%{cfg.targetdir}"'
			}
			
	filter "configurations:Release"
		defines "EE_RELEASE"
			flags{"LinkTimeOptimization"}
		runtime "Release"
		optimize "On"
		
			links
			{
				"Emerald/vendor/assimp/bin/Release/assimp-vc141-mt.lib",
				"Emerald/vendor/assimp/bin/Release/zlibstatic"
			}
			
			postbuildcommands 
			{
				'{COPY} "../Emerald/vendor/assimp/bin/Release/assimp-vc141-mt.dll" "%{cfg.targetdir}"'
			}


	filter "configurations:DebugServer"
		defines "EE_DEBUG"
		debugargs {"server"}
		runtime "Debug"
		symbols "On"

			links
			{
				"Emerald/vendor/assimp/bin/Debug/assimp-vc141-mtd.lib",
				"Emerald/vendor/assimp/bin/Debug/zlibstatic"
			}
			
			postbuildcommands 
			{
				'{COPY} "../Emerald/vendor/assimp/bin/Debug/assimp-vc141-mtd.dll" "%{cfg.targetdir}"'
			}
			
	filter "configurations:ReleaseServer"
		defines "EE_RELEASE"
		flags{"LinkTimeOptimization"}
		debugargs {"server"}
		runtime "Release"
		optimize "On"
		
			links
			{
				"Emerald/vendor/assimp/bin/Release/assimp-vc141-mt.lib",
				"Emerald/vendor/assimp/bin/Release/zlibstatic"
			}
			
			postbuildcommands 
			{
				'{COPY} "../Emerald/vendor/assimp/bin/Release/assimp-vc141-mt.dll" "%{cfg.targetdir}"'
			}