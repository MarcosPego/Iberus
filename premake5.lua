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
		"%{prj.name}/dependencies/imgui/*.cpp",
		"%{prj.name}/dependencies/imgui/backends/imgui_impl_glfw*.cpp",
		"%{prj.name}/dependencies/imgui/backends/imgui_impl_opengl3*.cpp",
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
		"%{prj.name}/dependencies/glfw/include",
		"%{prj.name}/dependencies/glfw/include/GLFW",
		"%{prj.name}/dependencies/glew/include/GL",
		"%{prj.name}/dependencies/stb_image",
		"%{prj.name}/dependencies/imgui",
		"%{prj.name}/dependencies/imgui/backends",
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
		"glfw3_mt"
	}

	-- ImGui sources must not use precompiled header
	filter "files:**/dependencies/imgui/**.cpp"
		flags { "NoPCH" }

	filter "system:windows"
		cppdialect "C++20"
		staticruntime "Off"
		systemversion "latest"

		defines {
			"IB_PLATFORM_WINDOWS",
			"IB_BUILD_DLL",
			"IB_DYNAMIC_LINK"
		}

		postbuildcommands {
			("if not exist \"$(SolutionDir)bin\\" .. outputdir .. "\\Sandbox\" mkdir \"$(SolutionDir)bin\\" .. outputdir .. "\\Sandbox\"")
		}
		postbuildcommands {
			("copy /Y \"$(TargetPath)\" \"$(SolutionDir)bin\\" .. outputdir .. "\\Sandbox\\\"")
		}
		postbuildcommands {
			("if exist \"$(ProjectDir)dependencies\\glew\\bin\\Release\\x64\\glew32.dll\" copy /Y \"$(ProjectDir)dependencies\\glew\\bin\\Release\\x64\\glew32.dll\" \"$(SolutionDir)bin\\" .. outputdir .. "\\Sandbox\\\"")
		}
		postbuildcommands {
			("if exist \"$(ProjectDir)dependencies\\glfw\\lib-vc2019\\glfw3.dll\" copy /Y \"$(ProjectDir)dependencies\\glfw\\lib-vc2019\\glfw3.dll\" \"$(SolutionDir)bin\\" .. outputdir .. "\\Sandbox\\\"")
		}

	filter "configurations:Debug"
		defines "IB_DEBUG"
		symbols "On"
		links {
			"FastNoiseD"
		}
		postbuildcommands {
			("if exist \"$(ProjectDir)dependencies\\FastNoise2\\bin\\FastNoiseD.dll\" copy /Y \"$(ProjectDir)dependencies\\FastNoise2\\bin\\FastNoiseD.dll\" \"$(SolutionDir)bin\\" .. outputdir .. "\\Sandbox\\\"")
		}

	filter "configurations:Release"
		defines "IB_RELEASE"
		optimize "On"
		links {
			"FastNoise"
		}
		postbuildcommands {
			("if exist \"$(ProjectDir)dependencies\\FastNoise2\\bin\\FastNoise.dll\" copy /Y \"$(ProjectDir)dependencies\\FastNoise2\\bin\\FastNoise.dll\" \"$(SolutionDir)bin\\" .. outputdir .. "\\Sandbox\\\"")
		}
		postbuildcommands {
			("if exist \"$(ProjectDir)dependencies\\glfw\\lib-vc2019\\glfw3.dll\" copy /Y \"$(ProjectDir)dependencies\\glfw\\lib-vc2019\\glfw3.dll\" \"$(SolutionDir)bin\\" .. outputdir .. "\\Sandbox\\\"")
		}
	
	filter "configurations:Dist"
		defines "IB_DIST" 
		optimize "On"
		links {
			"FastNoise"
		}
		postbuildcommands {
			("if exist \"$(ProjectDir)dependencies\\FastNoise2\\bin\\FastNoise.dll\" copy /Y \"$(ProjectDir)dependencies\\FastNoise2\\bin\\FastNoise.dll\" \"$(SolutionDir)bin\\" .. outputdir .. "\\Sandbox\\\"")
		}
		postbuildcommands {
			("if exist \"$(ProjectDir)dependencies\\glfw\\lib-vc2019\\glfw3.dll\" copy /Y \"$(ProjectDir)dependencies\\glfw\\lib-vc2019\\glfw3.dll\" \"$(SolutionDir)bin\\" .. outputdir .. "\\Sandbox\\\"")
		}

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
		"Iberus/src",
		"%{prj.name}/src/**",
		"%{prj.name}/src"
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
		"Iberus"
	}

	filter "system:windows"
		cppdialect "C++20"
		staticruntime "Off"
		systemversion "latest"

		defines {
			"IB_PLATFORM_WINDOWS",
			"IB_DYNAMIC_LINK"
		}

		postbuildcommands {
			("if exist \"$(SolutionDir)Iberus\\dependencies\\glew\\bin\\Release\\x64\\glew32.dll\" copy /Y \"$(SolutionDir)Iberus\\dependencies\\glew\\bin\\Release\\x64\\glew32.dll\" \"$(TargetDir)\"")
		}
		postbuildcommands {
			("if exist \"$(SolutionDir)Iberus\\dependencies\\glfw\\lib-vc2019\\glfw3.dll\" copy /Y \"$(SolutionDir)Iberus\\dependencies\\glfw\\lib-vc2019\\glfw3.dll\" \"$(TargetDir)\"")
		}
		postbuildcommands {
			("if exist \"$(SolutionDir)Sandbox\\assets\" xcopy /E /I /Y \"$(SolutionDir)Sandbox\\assets\" \"$(TargetDir)assets\\\"")
		}

	filter "configurations:Debug"
		defines "IB_DEBUG"
		symbols "On"
		links {
			"FastNoiseD"
		}
		postbuildcommands {
			("if exist \"$(SolutionDir)Iberus\\dependencies\\FastNoise2\\bin\\FastNoiseD.dll\" copy /Y \"$(SolutionDir)Iberus\\dependencies\\FastNoise2\\bin\\FastNoiseD.dll\" \"$(TargetDir)\"")
		}

	filter "configurations:Release"
		defines "IB_RELEASE"
		optimize "On"
		links {
			"FastNoise"
		}
		postbuildcommands {
			("if exist \"$(SolutionDir)Iberus\\dependencies\\FastNoise2\\bin\\FastNoise.dll\" copy /Y \"$(SolutionDir)Iberus\\dependencies\\FastNoise2\\bin\\FastNoise.dll\" \"$(TargetDir)\"")
		}
		postbuildcommands {
			("if exist \"$(SolutionDir)Iberus\\dependencies\\glfw\\lib-vc2019\\glfw3.dll\" copy /Y \"$(SolutionDir)Iberus\\dependencies\\glfw\\lib-vc2019\\glfw3.dll\" \"$(TargetDir)\"")
		}
	
	filter "configurations:Dist"
		defines "IB_DIST"
		optimize "On"
		links {
			"FastNoise"
		}
		postbuildcommands {
			("if exist \"$(SolutionDir)Iberus\\dependencies\\FastNoise2\\bin\\FastNoise.dll\" copy /Y \"$(SolutionDir)Iberus\\dependencies\\FastNoise2\\bin\\FastNoise.dll\" \"$(TargetDir)\"")
		}
		postbuildcommands {
			("if exist \"$(SolutionDir)Iberus\\dependencies\\glfw\\lib-vc2019\\glfw3.dll\" copy /Y \"$(SolutionDir)Iberus\\dependencies\\glfw\\lib-vc2019\\glfw3.dll\" \"$(TargetDir)\"")
		}