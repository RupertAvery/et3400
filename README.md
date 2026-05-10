# ET-3400 Emulator

This is an emulator for the [Heathkit ET-3400 Trainer](http://www.oldcomputermuseum.com/heathkit_et3400.html) built in C++.

This is a port of my emulator built in C# (https://github.com/RupertAvery/et3400-emu) with the goal of better performance and speed accuracy, as well as portability, with Windows and Linux targets.

## What's emulated

* Motorola 8-bit 6800 CPU running at 471kHz 
* 6 7-segment LED displays
* the hex keypad

A built-in ROM is provided. The ROM contains the Monitor program for the Heathkit ET-3400 Trainer, which interacts with the keypad and display to run programs and view CPU registers.

<img width="352" height="532" alt="emulator" src="https://github.com/user-attachments/assets/acf8accc-402a-432c-8053-54c7b466c3fe" />

# Features

* [Debugger](#debugger)
* [Breakpoints](#breakpoints)
* [Labels](#labels)

## Debugger

The emulator features a debugger which allows you to stop emulation and step into each instruction and see memory and registers update in real time.

### Controls

Aside from the controls in the toolbar, emulation can be controlled with the following keys

* F5 - Start emulation
* F4 - Stop emulation
* F9 - Toggle breakpoint
* 10 - Step into next instruction
* ESC - Reset emulator

<img width="987" height="753" alt="debugger" src="https://github.com/user-attachments/assets/9c0e19d1-5436-4fec-85eb-26ff87c84b39" />

## Breakpoints

### Setting a breakpoint

You can toggle a breakpoint at the currently selected line in the disassembly view by pressing F9.

You can also toggle a breakpoint at any line by hovering over the leftmost side of the disassembly view. An empty red circle will appear indicating a breakpoint can be set.

<img width="400" height="148" alt="image" src="https://github.com/user-attachments/assets/fdbd20bd-87d1-40d7-b7df-72c119d5d4a2" />

Setting a breakpoint will add a filled red circle to the left of the instructions, indicating that a breakpoint has been set.

<img width="386" height="127" alt="image" src="https://github.com/user-attachments/assets/9f69236c-5d10-4c56-99d1-7534d80a2c8c" />

### Hitting a breakpoint

When emulator execution reaches the instruction, the emulator will stop and the background color of the line will change to yellow, indicating that the instruction will be executed next when the emulator advances.

<img width="369" height="112" alt="image" src="https://github.com/user-attachments/assets/b5fa37c9-e53c-41ac-a976-545754f6ceee" />

### Loading and Saving Breakpoints

Breakpoints can be loaded and saved from the toolbar menu (File > Load Breakpoints and File > Save Breakpoints)

## Labels

Labels allow you to label and categorize areas of memory and change the way the memory is disassembled. For example, you can set an area of memory as DATA, and the disassembler will skip over the area, preventing it from being disassembled as instructions.

Below is a view of a section of RAM before and after adding labels.

<img alt="image" src="documentation/et3400-labels.png" />

### Adding Labels

Labels can be added from the toolbar menu (Labels > Add Label) or by right-clicking on an unlabeled line in the disassembly view. 

<img width="331" height="123" alt="image" src="https://github.com/user-attachments/assets/de78fb0d-3617-4b67-b7f3-144beaa01530" />

You will be prompted to select a type of label, Comment or Data. 

**Comment** will treat memory at the start address as an instruction, not affecting disassembly.

**Data** requires a start address and end address, will treat memory between the the start and end address as data, skipping disassembly over the range and displaying the memory as raw data.

<img width="352" height="282" alt="image" src="https://github.com/user-attachments/assets/71a626c5-8c9c-4918-bd41-fe08327cfad3" />

### Editing and Removing Labels

You can edit or remove a label by right-clicking on a labeled range in the disassembly view.

<img width="402" height="214" alt="image" src="https://github.com/user-attachments/assets/a22ba124-099a-4eb6-b172-2490e59c5c9d" />

### Loading and Saving Labels

Labels can be loaded and saved from the toolbar menu (File > Load Labels (RAM) and File > Save Labels (RAM))

# Manual

The original manual for this trainer is available in pdf format here: https://archive.org/details/HeathkitManualForTheEt-3400MicroprocessorTrainer

# Development

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

Install the necessary packages (Note: This was from 2022)

```
sudo apt-get update
sudo apt install git build-essential cmake qt5-base qt5-multimedia
```

If this does not work, you might want to try the following (from https://github.com/RupertAvery/et3400/issues/13)

```
apt-get install cmake-dbgsym cmake-qt-gui-dbgsym cmake
apt-get install qt5ct
apt-get install qtbase5-dev
apt-get install qtdeclarative5-dev
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
