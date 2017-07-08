# Minimal-GLFW-and-GLEW-premake
Minimal build of glfw and glew using premake. Only tested for Visual Studio 2015 and windows 10.

Both x86 and x64 architecture supported.

Generate the solution by opening the command prompt with the path to the folder that contains premake5.exe.
Type "premake5.exe vs2015" to generate the Visual Studio 2015 solution.

If you want to change the name of the solution and/or project look in the [premake5.lua](premake5.lua) file. It should be commented enough for you to easily change the names etc.

The solution will be located in the project folder. The location can be changed in the [premake5.lua](premake5.lua) file as well.

## About Premake
Premake is a easy to use commandline tool to generate projects. Take a look at their [website](https://premake.github.io/) for more information.

## About dependencies
Both [GLFW](http://www.glfw.org/) (version 3.2.1) and [glew](http://glew.sourceforge.net/) (version 2.0.0) are built using Visual Studio 2015 and dynamicaly linked. The dlls are already located in the binary folder so it should be to just compile and run.

If you need earlier versions or documentation take a look at their respective homepages with the links above.
