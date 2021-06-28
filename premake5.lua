-- premake5.lua
workspace "AHTTPServer"
   configurations { "Debug", "Release" }
   platforms { "Win32", "Win64", "Linux32", "Linux64", "MacOS" }
   location "build/%{_ACTION}"

project "AHTTPServer"
   kind "ConsoleApp"
   language "C++"
   targetdir "bin/%{cfg.buildcfg}/%{cfg.platform}"

   files {
        "./src/**.c*",
		"./src/**.h*"
	}

filter "configurations:Debug"
    defines { "DEBUG" }
    symbols "On"

filter "configurations:Release"
    defines { "NDEBUG" }
    optimize "On"

filter { "platforms:*32" }
    architecture "x86"
	
filter { "platforms:*64" }
    architecture "x86_64"

filter { "platforms:Win*" }
    system "windows"
	links{"wsock32"}
	
filter { "platforms:Linux*" }
    system "linux"

filter { "platforms:MacOS" }
    system "macosx"