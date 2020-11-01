cmake_minimum_required(VERSION 3.0.0)
project(et3400 VERSION 0.1.0)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED Core Gui Widgets)

set(WIDGETS_SRC src/windows/display.cpp)
set(EMUSRC src/cpu/m6800.cpp src/emu/et3400.cpp)
set(SOURCES src/main.cpp src/console.cpp src/util/srec.cpp src/windows/mainwindow.cpp ${EMUSRC} ${WIDGETS_SRC} src/resources/resources.qrc)

add_executable(et3400 ${SOURCES} ${PROJECT_RESOURCES})

target_link_libraries(et3400 PRIVATE Qt::Core Qt5::Core Qt::WinMain Qt5::WinMain)
target_link_libraries(et3400 PRIVATE Qt::Widgets Qt5::Widgets Qt::WidgetsPrivate Qt5::WidgetsPrivate)
target_link_libraries(et3400 PRIVATE Qt::Gui Qt5::Gui)
