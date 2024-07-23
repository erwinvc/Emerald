project "GLAD"
	kind "StaticLib"
	language "C"
	
    targetdir ("../../../binaries/" .. OutputDir .. "/%{prj.name}")
    objdir ("../../../binaries/intermediates/" .. OutputDir .. "/%{prj.name}")
	
	files
	{
		"include/**.h",
		"src/**.c"
	}
	
	includedirs
	{
		"include"
	}
	
	filter "system:windows"
		systemversion "latest"
		staticruntime "On"
		
	filter { "system:windows", "configurations:Release" }
		buildoptions "/MT"