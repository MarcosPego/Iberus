workspace "Iberus"
	architecture "x64"
	startproject "Game"

	configurations {
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
gamedir = "Game-Build"
appdir = gamedir .. "/App"

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
		"%{prj.name}/dependencies/glew/include",
		"%{prj.name}/dependencies/glew/include/GL",
		"%{prj.name}/dependencies/stb_image",
		"%{prj.name}/dependencies/imgui",
		"%{prj.name}/dependencies/imgui/backends",
		"%{prj.name}/dependencies/FastNoise2/include/",
		"%{prj.name}/dependencies/FastNoise2/include/**",
		"%{prj.name}/dependencies",
		"%{prj.name}/dependencies/dotnet-hosting"
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
			("if not exist \"$(SolutionDir)" .. gamedir .. "\" mkdir \"$(SolutionDir)" .. gamedir .. "\"")
		}
		postbuildcommands {
			("if not exist \"$(SolutionDir)" .. appdir .. "\" mkdir \"$(SolutionDir)" .. appdir .. "\"")
		}
		postbuildcommands {
			("copy /Y \"$(TargetPath)\" \"$(SolutionDir)" .. appdir .. "\\\"")
		}
		postbuildcommands {
			("if exist \"$(ProjectDir)dependencies\\glew\\bin\\Release\\x64\\glew32.dll\" copy /Y \"$(ProjectDir)dependencies\\glew\\bin\\Release\\x64\\glew32.dll\" \"$(SolutionDir)" .. appdir .. "\\\"")
		}
		postbuildcommands {
			("if exist \"$(ProjectDir)dependencies\\glfw\\lib-vc2019\\glfw3.dll\" copy /Y \"$(ProjectDir)dependencies\\glfw\\lib-vc2019\\glfw3.dll\" \"$(SolutionDir)" .. appdir .. "\\\"")
		}

	filter "configurations:Debug"
		defines "IB_DEBUG"
		symbols "On"
		links {
			"FastNoiseD"
		}
		postbuildcommands {
			("if exist \"$(ProjectDir)dependencies\\FastNoise2\\bin\\FastNoiseD.dll\" copy /Y \"$(ProjectDir)dependencies\\FastNoise2\\bin\\FastNoiseD.dll\" \"$(SolutionDir)" .. appdir .. "\\\"")
		}

	filter "configurations:Release"
		defines "IB_RELEASE"
		optimize "On"
		links {
			"FastNoise"
		}
		postbuildcommands {
			("if exist \"$(ProjectDir)dependencies\\FastNoise2\\bin\\FastNoise.dll\" copy /Y \"$(ProjectDir)dependencies\\FastNoise2\\bin\\FastNoise.dll\" \"$(SolutionDir)" .. appdir .. "\\\"")
		}
		postbuildcommands {
			("if exist \"$(ProjectDir)dependencies\\glfw\\lib-vc2019\\glfw3.dll\" copy /Y \"$(ProjectDir)dependencies\\glfw\\lib-vc2019\\glfw3.dll\" \"$(SolutionDir)" .. appdir .. "\\\"")
		}

	filter "configurations:Dist"
		defines "IB_DIST"
		optimize "On"
		links {
			"FastNoise"
		}
		postbuildcommands {
			("if exist \"$(ProjectDir)dependencies\\FastNoise2\\bin\\FastNoise.dll\" copy /Y \"$(ProjectDir)dependencies\\FastNoise2\\bin\\FastNoise.dll\" \"$(SolutionDir)" .. appdir .. "\\\"")
		}
		postbuildcommands {
			("if exist \"$(ProjectDir)dependencies\\glfw\\lib-vc2019\\glfw3.dll\" copy /Y \"$(ProjectDir)dependencies\\glfw\\lib-vc2019\\glfw3.dll\" \"$(SolutionDir)" .. appdir .. "\\\"")
		}

project "Game"
	location "Game"
	kind "ConsoleApp"
	language "C++"

	targetdir ("$(SolutionDir)" .. gamedir)
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
		"bin/" .. outputdir .. "/Iberus",
		"Iberus/dependencies/glfw/lib-vc2019",
		"Iberus/dependencies/glew/lib/Release/x64",
		"Iberus/dependencies/FastNoise2/lib"
	}

	links {
		"glew32",
		"opengl32",
		"glfw3_mt",
		"delayimp",
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

		linkoptions { "/DELAYLOAD:Iberus.dll" }

		postbuildcommands {
			("if not exist \"$(TargetDir)App\" mkdir \"$(TargetDir)App\"")
		}
		postbuildcommands {
			("if exist \"$(TargetDir)Iberus.dll\" move /Y \"$(TargetDir)Iberus.dll\" \"$(TargetDir)App\\\"")
		}
		postbuildcommands {
			("dotnet publish \"$(SolutionDir)Iberus\\Scripts\\ScriptHost\\ScriptHost.csproj\" -c Release -r win-x64 --self-contained false -o \"$(TargetDir)App\" -p:PublishSingleFile=false || echo ScriptHost publish failed")
		}
		postbuildcommands {
			("if exist \"$(SolutionDir)Iberus\\Scripts\\Iberus.Scripts.runtimeconfig.json\" copy /Y \"$(SolutionDir)Iberus\\Scripts\\Iberus.Scripts.runtimeconfig.json\" \"$(TargetDir)App\\\"")
		}
		postbuildcommands {
			("powershell -NoProfile -Command \"if(-not (Test-Path '$(TargetDir)App\\nethost.dll')){ $paths=@('C:\\Program Files\\dotnet\\packs\\Microsoft.NETCore.App.Host.win-x64','$env:USERPROFILE\\.dotnet\\packs\\Microsoft.NETCore.App.Host.win-x64'); foreach($p in $paths){ if(Test-Path $p){ $d=Get-ChildItem $p -Directory|Sort-Object Name -Descending|Select-Object -First 1; if($d){ $src=Join-Path $d.FullName 'runtimes\\win-x64\\native\\nethost.dll'; if(Test-Path $src){ Copy-Item $src '$(TargetDir)App\\' -Force; break } } } } }\"")
		}
		postbuildcommands {
			("if exist \"$(SolutionDir)Iberus\\dependencies\\glew\\bin\\Release\\x64\\glew32.dll\" copy /Y \"$(SolutionDir)Iberus\\dependencies\\glew\\bin\\Release\\x64\\glew32.dll\" \"$(TargetDir)App\\\"")
		}
		postbuildcommands {
			("if exist \"$(SolutionDir)Iberus\\dependencies\\glfw\\lib-vc2019\\glfw3.dll\" copy /Y \"$(SolutionDir)Iberus\\dependencies\\glfw\\lib-vc2019\\glfw3.dll\" \"$(TargetDir)App\\\"")
		}

	filter "configurations:Debug"
		defines "IB_DEBUG"
		symbols "On"
		links {
			"FastNoiseD"
		}
		postbuildcommands {
			("if exist \"$(SolutionDir)Iberus\\dependencies\\FastNoise2\\bin\\FastNoiseD.dll\" copy /Y \"$(SolutionDir)Iberus\\dependencies\\FastNoise2\\bin\\FastNoiseD.dll\" \"$(TargetDir)App\\\"")
		}

	filter "configurations:Release"
		defines "IB_RELEASE"
		optimize "On"
		links {
			"FastNoise"
		}
		postbuildcommands {
			("if exist \"$(SolutionDir)Iberus\\dependencies\\FastNoise2\\bin\\FastNoise.dll\" copy /Y \"$(SolutionDir)Iberus\\dependencies\\FastNoise2\\bin\\FastNoise.dll\" \"$(TargetDir)App\\\"")
		}
		postbuildcommands {
			("if exist \"$(SolutionDir)Iberus\\dependencies\\glfw\\lib-vc2019\\glfw3.dll\" copy /Y \"$(SolutionDir)Iberus\\dependencies\\glfw\\lib-vc2019\\glfw3.dll\" \"$(TargetDir)App\\\"")
		}

	filter "configurations:Dist"
		defines "IB_DIST"
		optimize "On"
		links {
			"FastNoise"
		}
		postbuildcommands {
			("if exist \"$(SolutionDir)Iberus\\dependencies\\FastNoise2\\bin\\FastNoise.dll\" copy /Y \"$(SolutionDir)Iberus\\dependencies\\FastNoise2\\bin\\FastNoise.dll\" \"$(TargetDir)App\\\"")
		}
		postbuildcommands {
			("if exist \"$(SolutionDir)Iberus\\dependencies\\glfw\\lib-vc2019\\glfw3.dll\" copy /Y \"$(SolutionDir)Iberus\\dependencies\\glfw\\lib-vc2019\\glfw3.dll\" \"$(TargetDir)App\\\"")
		}
