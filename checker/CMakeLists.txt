cmake_minimum_required(VERSION 3.26)
project(
  xFlowPluginChecker
  VERSION 0.1
  LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets LinguistTools)

file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.*)
list(APPEND SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/checker.rc)
list(APPEND SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/checker.qrc)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/xFlowPluginChecker)
qt_add_executable(xFlowPluginChecker WIN32 MANUAL_FINALIZATION ${SRC_FILES})
target_link_libraries(xFlowPluginChecker PRIVATE Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(xFlowPluginChecker PRIVATE Qt${QT_VERSION_MAJOR}::Gui)
target_link_libraries(xFlowPluginChecker PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# --------------------------------------------------------------------------------------------------
# Deploying the application
if(WINDEPLOYQT_EXECUTABLE)
  add_custom_command(
    TARGET xFlowPluginChecker
    POST_BUILD
    COMMAND ${WINDEPLOYQT_EXECUTABLE} $<TARGET_FILE:xFlowPluginChecker>)
endif()
