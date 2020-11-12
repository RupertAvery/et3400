cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE} CACHE STRING "" FORCE)
project(et3400 VERSION 0.1.0)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Widgets Gui REQUIRED)
find_package(Threads REQUIRED)

set(WIDGETS_SRC src/widgets/display.cpp src/widgets/keypad.cpp src/widgets/memory_view.cpp src/widgets/disassembly_view.cpp src/widgets/status_view.cpp)
set(WINDOWS_SRC ${WIDGETS_SRC} src/windows/mainwindow.cpp src/windows/about.cpp src/windows/settings.cpp  src/windows/debugger.cpp src/windows/file.cpp src/windows/add_label.cpp  src/windows/remove_label.cpp)
set(DEVSRC src/dev/memory_map.cpp src/dev/memory_dev.cpp src/dev/keypad_dev.cpp src/dev/display_dev.cpp)
set(EMUSRC src/cpu/m6800.cpp src/emu/et3400.cpp)
set(TOOLSRC src/util/srec.cpp src/util/map.cpp src/util/disassembly_builder.cpp src/dasm/disassembler.cpp src/cpu/breakpoint_manager.cpp)
set(SOURCES src/main.cpp ${TOOLSRC} ${DEVSRC} ${EMUSRC} ${WINDOWS_SRC} src/resources/resources.qrc src/resources/resources.rc)

add_executable(et3400 ${SOURCES} ${PROJECT_RESOURCES})

target_link_libraries(et3400 PRIVATE Qt5::Core Qt5::Widgets Qt5::Gui Threads::Threads)
