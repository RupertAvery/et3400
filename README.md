# ET-3400 Emulator

This is an ET-3400 Trainer Emulator built in C++.

This is a port of the emulator build in C# (https://github.com/RupertAvery/et3400-emu) with the goal of better performance and speed accuracy, as well as portability, with Windows and Linux targets.

## Windows

Requiremments:

* Visual Studio 2017 or later
* CMake (https://cmake.org/download/)
* vcpkg (see below)
* Qt libraries

### Installing vcpkg 

`vcpkg` is a tool from Microsoft to install C++ libraries from source.

Install vcpkg (https://github.com/microsoft/vcpkg)

```
git clone https://github.com/microsoft/vcpkg
cd vcpkg
bootstrap-vcpkg.bat
```

### Installing Qt packages

In the `vcpkg` directory run the following command

```
vcpkg install qt5-base qt5-multimedia
```

This will take a while. Go watch a movie.

### Building

Perform a standard out-of-source build from the `src` directory.

```
md build
cd build
cmake .. "-DCMAKE_TOOLCHAIN_FILE=<path-to-vckpkg>\scripts\buildsystems\vcpkg.cmake"
```

This will generate a `.sln` and all necessary files in the `build` folder.

You should be able to compile the solution into an executable with

```
cmake --build .
```

CMake should launch msbuild for you.

You can also open the `.sln` file in Visual Studio if you have C++ workload installed, and compile and debug from there.


# Linux
