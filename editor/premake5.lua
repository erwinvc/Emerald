project "Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	targetdir "binaries/%{cfg.buildcfg}"
	staticruntime "off"
	targetname "EmeraldEditor"
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
	
	-- Include Engine
		"../engine/src",
	}
	
	externalincludedirs
	{
		"../engine/dependencies/GLFW/include",
	 	"../engine/dependencies/GLAD/include",
	 	"../engine/dependencies/imgui",
		"../engine/dependencies"
	}
	
	links
	{
		"engine",
	}

	targetdir ("../binaries/" .. OutputDir .. "/%{prj.name}")
	objdir ("../binaries/intermediates/" .. OutputDir .. "/%{prj.name}")

	filter "system:windows"
		systemversion "latest"
        defines { "WINDOWS" }

	filter "configurations:Debug"
        defines { "EE_DEBUG" }
        runtime "Debug"
        symbols "On"
		kind ("ConsoleApp")
	    postbuildcommands { '{COPY} "../engine/dependencies/assimp/bin/Debug/assimp-vc141-mtd.dll" "%{cfg.targetdir}"' }

	filter "configurations:Release"
        defines { "EE_RELEASE" }
        runtime "Release"
        optimize "On"
		symbols "Off"
		kind ("ConsoleApp")
		postbuildcommands { '{COPY} "../engine/dependencies/assimp/bin/Release/assimp-vc141-mt.dll" "%{cfg.targetdir}"' }
		
    filter "configurations:Dist"
        defines { "EE_DIST" }
        runtime "Release"
        optimize "On"
        symbols "Off"
		kind ("WindowedApp")
		postbuildcommands { '{COPY} "../engine/dependencies/assimp/bin/Release/assimp-vc141-mt.dll" "%{cfg.targetdir}"' }