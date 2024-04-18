workspace "Iberus"
	architecture "x64"
	startproject "Sandbox"

	configurations {
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Iberus"
	location "Iberus"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "Enginepch.h"
	pchsource "Iberus/src/Core/Enginepch.cpp"

	files {
		"%{prj.name}/src/**/**/**/**.h",
		"%{prj.name}/src/**/**/**/**.cpp",
		"%{prj.name}/src/**/**/**.h",
		"%{prj.name}/src/**/**/**.cpp",
		"%{prj.name}/src/**/**.h",
		"%{prj.name}/src/**/**.cpp",
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/dependencies/glfw/include/GLFW/**.h",
		"%{prj.name}/dependencies/glew/include/GL/**.h",
		"%{prj.name}/dependencies/stb_image/**.h",
		"%{prj.name}/dependencies/imgui/**.h",
		"%{prj.name}/dependencies/FastNoise2/include/**",
		"%{prj.name}/dependencies/FastNoise2/include/**/**.h"
	}

	includedirs {
		"%{prj.name}/src/**/**/**/**",
		"%{prj.name}/src/**/**/**",
		"%{prj.name}/src/**/**",
		"%{prj.name}/src/**",
		"%{prj.name}/src",
		"%{prj.name}/dependencies/spdlog/include",
		"%{prj.name}/dependencies/glfw/include/GLFW",
		"%{prj.name}/dependencies/glew/include/GL",
		"%{prj.name}/dependencies/stb_image",
		"%{prj.name}/dependencies/imgui",
		"%{prj.name}/dependencies/FastNoise2/include/",
		"%{prj.name}/dependencies/FastNoise2/include/**"
	}

	libdirs {
		"%{prj.name}/dependencies/glfw/lib-vc2019",
		"%{prj.name}/dependencies/glew/lib/Release/x64",
		"%{prj.name}/dependencies/FastNoise2/lib"
	}

	links {
		"glew32",
		"opengl32",
		"glfw3_mt",
		"FastNoise",
		"FastNoiseD"
	}

	filter "system:windows"
		cppdialect "C++20"
		staticruntime "On"
		systemversion "latest"

		defines {
			"IB_PLATFORM_WINDOWS",
			"IB_BUILD_DLL",
			"IB_DYNAMIC_LINK"
		}

		postbuildcommands {
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
		}

	filter "configurations:Debug"
		defines "IB_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "IB_RELEASE"
		optimize "On"
	
	filter "configurations:Dist"
		defines "IB_DIST" 
		optimize "On"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs {
		"Iberus/dependencies/glfw/include/GLFW",
		"Iberus/dependencies/glew/include/GL",
		"Iberus/dependencies/glew/include/stb_image",
		"Iberus/dependencies/spdlog/include",
		"Iberus/dependencies/imgui",
		"Iberus/dependencies/FastNoise2/include",
		"Iberus/src/**",
		"Iberus/src"
	}

	libdirs {
		"Iberus/dependencies/glfw/lib-vc2019",
		"Iberus/dependencies/glew/lib/Release/x64",
		"Iberus/dependencies/FastNoise2/lib"
	}

	links {		
		"glew32",
		"opengl32",
		"glfw3_mt",
		"FastNoise",
		"Iberus"
	}

	filter "system:windows"
		cppdialect "C++20"
		staticruntime "On"
		systemversion "latest"

		defines {
			"IB_PLATFORM_WINDOWS",
			"IB_DYNAMIC_LINK"
		}

	filter "configurations:Debug"
		defines "IB_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "IB_RELEASE"
		optimize "On"
	
	filter "configurations:Dist"
		defines "IB_DIST"
		optimize "On"