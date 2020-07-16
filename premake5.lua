-- premake5.lua
workspace "AHTTPServer"
   configurations { "Debug", "Release" }
   platforms { "Win32", "Win64", "linux", "macos" }
project "AHTTPServer"
   kind "ConsoleApp"
   language "C"
   targetdir "bin/%{cfg.buildcfg}"

   files { 
       --"**.h", "**.c", "**.cpp",
        "./src/**.cpp", "./src/**.h" }

filter "configurations:Debug"
    defines { "DEBUG" }
    symbols "On"

filter "configurations:Release"
    defines { "NDEBUG" }
    optimize "On"
filter { "platforms:Win32" }
    system "Windows"
    architecture "x86"
    links{"wsock32"}

filter { "platforms:Win64" }
    system "Windows"
    architecture "x64"
    links{"wsock32"}