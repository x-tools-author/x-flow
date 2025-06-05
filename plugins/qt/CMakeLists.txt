cmake_minimum_required(VERSION 3.16)
# cmake-format: off
project(xFlowQtPlugins VERSION 1.0 LANGUAGES C CXX)
# cmake-format: on

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(qt_components Core Gui Widgets)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS ${qt_components})
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${qt_components})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/bypass)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/datachecker)
