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

	files {
		"%{prj.name}/src/**/**.h",
		"%{prj.name}/src/**/**.cpp",
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs {
		"%{prj.name}/src/**",
		"%{prj.name}/src",
		"%{prj.name}/dependencies/spdlog/include",
		"%{prj.name}/dependencies/glfw/include/GLFW",
		"%{prj.name}/dependencies/glew/include/GL"
	}

	libdirs {
		"%{prj.name}/dependencies/glfw/lib-vc2019",
		"%{prj.name}/dependencies/glew/lib/Release/x64"
	}

	links {
		"glfw3",
		"glew32"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines {
			"IB_PLATFORM_WINDOWS",
			"IB_BUILD_DLL",
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
		"Iberus/dependencies/spdlog/include",
		"Iberus/src/**",
		"Iberus/src"
	}

	links {
		"Iberus"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines {
			"IB_PLATFORM_WINDOWS",
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