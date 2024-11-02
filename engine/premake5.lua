project "Engine"
	kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    targetdir "binaries/%{cfg.buildcfg}"
    staticruntime "Off"
	targetname "EmeraldEngine"
	externalwarnings "Default"
	externalanglebrackets "On"
	
    files
	{
		"**.h",
		"**.cpp",
		"**.hpp"
	}
    
    includedirs
    {
	 	"src",
    }
	
	externalincludedirs
	{
		"dependencies/assimp/include",
	 	"dependencies/GLFW/include",
	 	"dependencies/GLAD/include",
		"dependencies"
	}
   
   	libdirs  
	{ 
		"dependencies/AssImp",
	}
	
	links
	{
		"GLFW",
		"GLAD",
		"opengl32",
	}

    targetdir ("../binaries/" .. OutputDir .. "/%{prj.name}")
    objdir ("../binaries/intermediates/" .. OutputDir .. "/%{prj.name}")
	
	pchheader "eepch.h"
	pchsource "src/eepch.cpp"
    
	filter "files:dependencies/**.cpp"
   	flags { "NoPCH" }
	
    filter "system:windows"
        systemversion "latest"
        defines 
		{
			"GLEW_STATIC"
		}
		ignoredefaultlibraries { "libcmt" }
    
    filter "configurations:Debug"
        defines { "EE_DEBUG" }
        runtime "Debug"
        symbols "On"
		libdirs { "dependencies/assimp/bin/Debug" }
		links
		{
			"assimp-vc141-mtd.lib",
			"zlibstatic"
		}
		postbuildcommands { '{COPY} "dependencies/assimp/bin/Debug/assimp-vc141-mtd.dll" "%{cfg.targetdir}"' }
    
    filter "configurations:Release"
        defines { "EE_RELEASE" }
        runtime "Release"
        optimize "On"
		symbols "On"
		libdirs {
			"dependencies/assimp/lib/Release",
			"dependencies/zlib/lib/Release",
		}
		links {
			"assimp-vc143-mt.lib",
			"zlibstatic.lib",
		}
    
    filter "configurations:Dist"
		defines { "EE_DIST" }
		runtime "Release"
		optimize "On"
		symbols "On"
		libdirs {
			"dependencies/assimp/lib/Release",
			"dependencies/zlib/lib/Release",
		}
		links {
			"assimp-vc143-mt.lib",
			"zlibstatic.lib",
		}