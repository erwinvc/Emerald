workspace "Emerald"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Emerald"
	location "Emerald"
	kind "ConsoleApp"
	language "C++"
	
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	
	pchheader "stdafx.h"
	pchsource "Emerald/stdafx.cpp"
   
	files
	{
		"%{prj.name}/**.h",
		"%{prj.name}/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/vendor/AssImp/include",
		"%{prj.name}/vendor/AssImp/build/include",
		"%{prj.name}/vendor/GLFW/include",
		"%{prj.name}/vendor/GLEW/include",
		"%{prj.name}/vendor/imgui",
		"%{prj.name}/src",
		"."
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
		staticruntime "off"
		systemversion "10.0.17134.0"

		defines
		{
			"GLEW_STATIC"
		}

	filter "configurations:Debug"
		defines "EE_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "EE_RELEASE"
		optimize "On"