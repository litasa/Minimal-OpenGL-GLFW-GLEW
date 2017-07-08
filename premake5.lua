-- premake5.lua


workspace "Minimal GLFW and GLEW" -- Name of sln file
    location "project" -- Folder where to put it
    configurations { "Debug", "Release" }
    platforms { "Win32", "x64" }

    --Set architecture
    filter { "platforms:Win32" }
        system "Windows"
        architecture "x86"
    
    filter { "platforms:x64" }
        system "Windows"
        architecture "x64"

    filter { }

project "Minimal Example" -- Name of project
    kind "ConsoleApp" -- Uses the console
    language "C++"
    location "project/Minimal Example" -- location of vcxproj file
    targetdir "bin/%{cfg.buildcfg}/%{cfg.architecture}" -- .exe files is in bin/(debug or release)/(x86 or x64)/
    --location of source files to include. Here we include All files ending with .h and .cpp
    --in the folder Minimal Example even files in subfolders.
    files { "./src/Minimal Example/**.h", "./src/Minimal Example/**.cpp" } 

    --Include directories
    includedirs {
        "./dependencies/glfw-3.2.1/include",
        "./dependencies/glew-2.0.0/include"
    }

    --libraries and links
        --links (the library files)
        links {
            "glew32",
            "opengl32",
            "glfw3"
        }
        --for 32 bit use these library paths
        filter "architecture:x86"
            libdirs { 
                "./dependencies/glfw-3.2.1/win32/lib",
                "./dependencies/glew-2.0.0/win32/lib"
             }
        filter { }
        --for x64 use these
        filter "architecture:x64"
            libdirs { 
                "./dependencies/glfw-3.2.1/x64/lib",
                "./dependencies/glew-2.0.0/x64/lib"
            }
    
    --Debug and Release configurations
    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"

    filter { }
