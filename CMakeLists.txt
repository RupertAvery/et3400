cmake_minimum_required(VERSION 3.0.0)
project(et3400 VERSION 0.1.0)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED Core Gui Widgets)
find_package(Threads REQUIRED)

set(WIDGETS_SRC src/windows/display.cpp)
set(EMUSRC src/cpu/m6800.cpp src/emu/et3400.cpp)
set(SOURCES src/main.cpp src/util/srec.cpp src/windows/mainwindow.cpp ${EMUSRC} ${WIDGETS_SRC} src/resources/resources.qrc)

add_executable(et3400 ${SOURCES} ${PROJECT_RESOURCES})

target_link_libraries(et3400 PRIVATE Qt5::Core Qt5::Widgets Qt5::Gui Threads::Threads)
