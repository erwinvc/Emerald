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
		"**.hpp",
		"**.vert",
		"**.frag",
		"**.geom",
		"**.tese",
		"**.tesc"
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
		libdirs {
			"dependencies/assimp/lib/Debug",
			"dependencies/zlib/lib/Debug",
		}
		links
		{
			"assimp-vc143-mtd.lib",
			"zlibstaticd.lib"
		}
		postbuildcommands {
			'{COPY} "../engine/dependencies/assimp/lib/Debug/assimp-vc143-mtd.pdb" "%{cfg.targetdir}"',
			'{COPY} "../engine/dependencies/zlib/lib/Debug/zlibstaticd.pdb" "%{cfg.targetdir}"'
		}
		
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
		postbuildcommands {
			'{COPY} "../engine/dependencies/assimp/lib/Release/assimp-vc143-mt.pdb" "%{cfg.targetdir}"',
			'{COPY} "../engine/dependencies/zlib/lib/Release/zlibstatic.pdb" "%{cfg.targetdir}"'
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
		postbuildcommands {
			'{COPY} "../engine/dependencies/assimp/lib/Release/assimp-vc143-mt.pdb" "%{cfg.targetdir}"',
			'{COPY} "../engine/dependencies/zlib/lib/Release/zlibstatic.pdb" "%{cfg.targetdir}"'
		}