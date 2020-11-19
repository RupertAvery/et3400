cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE} CACHE STRING "" FORCE)
project(et3400 VERSION 0.1.0)


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Widgets Gui REQUIRED)
find_package(Threads REQUIRED)

set(WIDGETS_SRC 
    src/widgets/display.cpp 
    src/widgets/keypad.cpp 
    src/widgets/memory_view.cpp 
    src/widgets/disassembly_view.cpp 
    src/widgets/status_view.cpp
    )

set(WINDOWS_SRC 
    ${WIDGETS_SRC} 
    src/windows/mainwindow.cpp 
    src/windows/about.cpp 
    src/windows/settings.cpp  
    src/windows/debugger.cpp 
    src/windows/file.cpp 
    src/windows/label.cpp 
    src/windows/remove_label.cpp 
    src/windows/tips.cpp 
    src/windows/goto.cpp 
    )

set(DEVSRC 
    src/dev/memory_map.cpp 
    src/dev/memory_dev.cpp 
    src/dev/keypad_dev.cpp 
    src/dev/display_dev.cpp
    src/dev/mc6820.cpp
    src/dev/rs232.cpp
    )

set(EMUSRC 
    src/cpu/m6800.cpp 
    src/emu/et3400.cpp
    )

set(TOOLSRC 
    src/util/csv.cpp 
    src/util/srec.cpp 
    src/util/label.cpp 
    src/util/breakpoint.cpp 
    src/util/label_manager.cpp  
    src/util/disassembly_builder.cpp 
    src/util/breakpoint_manager.cpp 
    src/util/settings.cpp 
    src/dasm/disassembler.cpp 
    )

set(SOURCES 
    src/main.cpp 
    src/common/util.cpp
    ${TOOLSRC} 
    ${DEVSRC} 
    ${EMUSRC} 
    ${WINDOWS_SRC} 
    src/resources/resources.qrc 
    src/resources/resources.rc
    )

add_executable(et3400 
    ${SOURCES} 
    ${PROJECT_RESOURCES}
    )

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  set_property(TARGET et3400 PROPERTY WIN32_EXECUTABLE true)
endif()


target_link_libraries(et3400 
    PRIVATE 
    Qt5::Core 
    Qt5::Widgets 
    Qt5::Gui 
    Threads::Threads
    )
