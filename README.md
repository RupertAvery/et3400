# ET-3400 Emulator

This is an [ET-3400 Trainer](http://www.oldcomputermuseum.com/heathkit_et3400.html) Emulator built in C++.

This is a port of the emulator built in C# (https://github.com/RupertAvery/et3400-emu) with the goal of better performance and speed accuracy, as well as portability, with Windows and Linux targets.

It emulates the Motorola 6800 CPU, the 6 7-segment LED displays and the hex keypad. The ROM contains the Monitor program for the Heathkit ET-3400 Trainer, which interacts with the keypad and display to run programs and view CPU registers.

![emulator](https://user-images.githubusercontent.com/1910659/99161743-41c0cd00-2730-11eb-83b4-651fbd18b973.png)

The emulator also features a debugger which allows you to stop emulation and step into each instruction and see memory and registers update in real time. You can also set breakpoints (separate from the software breakpoints allowed by the Monitor ROM) and add labels to the disassembly to clean up the output. 

![debugger](https://user-images.githubusercontent.com/1910659/99161744-44bbbd80-2730-11eb-8ce4-54aec7b27489.png)

The manual for this trainer is available here: https://archive.org/details/HeathkitManualForTheEt-3400MicroprocessorTrainer

## Windows

### Requiremments

* Visual Studio 2017 or later
* git
* CMake (https://cmake.org/download/)
* vcpkg (see below)
* Qt libraries

### Clone this repository

```
git clone https://github.com/RupertAvery/et3400.git
```

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

or if you want 64-bit

```
vcpkg install qt5-base qt5-multimedia --triplet x64-windows
```


This will take a while. Go watch a movie.

### Building

Perform a standard out-of-source build.

**NOTE:** This has only been tested on **Visual Studio 16 2019**. For other targets you may need to add `-G "Visual Studio 15 2017 [arch]"`

**x86**

```
md build
cd build
cmake .. "-DCMAKE_TOOLCHAIN_FILE=<path-to-vckpkg>\scripts\buildsystems\vcpkg.cmake" -A Win32 -DCMAKE_BUILD_TYPE=Debug
```

**x64**

```
md build
cd build
cmake .. "-DCMAKE_TOOLCHAIN_FILE=<path-to-vckpkg>\scripts\buildsystems\vcpkg.cmake" -A x64 -DCMAKE_BUILD_TYPE=Debug
```

For Release mode, change CMAKE_BUILD_TYPE accordingly.

Remember to clear out the build folder, or use a different folder if changing architectures or build types.
 
This will generate a `.sln` and all necessary files in the `build` folder.

You should be able to compile the solution into an executable with

```
cmake --build . --config Debug
```

CMake should launch msbuild for you.

You can also open the `.sln` file in Visual Studio if you have C++ workload installed, and compile and debug from there.


## Linux

### Requiremments

* build-essentials
* git
* cmake
* Qt libraries


## Installation

```
sudo apt-get update
sudo apt install git build-essential cmake qt5-base qt5-multimedia
```

### Clone this repository

```
git clone https://github.com/RupertAvery/et3400.git
```

### Build and compile

Perform a standard out-of-source build.

```
cd et3400/build
cmake ..
make
```

The executable `et3400` will be created in the `build` directory.
