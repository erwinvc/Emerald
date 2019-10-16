workspace "Emerald"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release"
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
		"%{prj.name}/**.cpp",
		"%{prj.name}/**.vert",
		"%{prj.name}/**.frag",
		"%{prj.name}/**.tese",
		"%{prj.name}/**.tesc"
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
		"opengl32",
		"IrrXML",
		"zlib.lib",
		"zlibstatic",
		"assimp-vc140-mt"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"GLEW_STATIC"
		}

	filter "configurations:Debug"
		defines "EE_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "EE_RELEASE"
			flags{"LinkTimeOptimization"}
		runtime "Release"
		optimize "On"