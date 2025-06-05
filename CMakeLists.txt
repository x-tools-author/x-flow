cmake_minimum_required(VERSION 3.25)
project(xFlow VERSION 3.0)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(WIN32 OR LINUX)
  # cmake-format: off
  set(BUILD_SHARED_LIBS OFF CACHE BOOL "Disable build shared libraries" FORCE)
  # cmake-format: on
endif()

set(QT_PACKAGES
    Widgets
    SerialPort
    Network
    WebSockets
    Svg
    Qml
    OpenGL
    LinguistTools)
find_package(Qt NAMES Qt6 REQUIRED COMPONENTS ${QT_PACKAGES})
find_package(Qt6 REQUIRED COMPONENTS ${QT_PACKAGES})

if(MINGW)
  # * MinGW is not supported.
  # * message(FATAL_ERROR "MinGW is not supported. Please use MSVC.")
endif()

# --------------------------------------------------------------------------------------------------
if(APPLE)
  # Disable -Werror,-Wunused-variable
  set(CMAKE_COMPILE_WARNING_AS_ERROR
      OFF
      CACHE BOOL "Disable compile warning as error")
  add_compile_options(-Wno-unused-parameter)
  add_compile_options(-Wno-unused-variable)
  add_compile_options(-Wno-unused-function)
endif()

# --------------------------------------------------------------------------------------------------
# Magic option
option(X_MAGIC "Do not change the option unless you know what are you doing..." OFF)
if(X_MAGIC)
  add_compile_definitions(X_MAGIC)
endif()

# --------------------------------------------------------------------------------------------------
# Git modules
include(cmake/git.cmake)
x_git_get_latest_tag(${CMAKE_SOURCE_DIR} X_FLOW)
x_git_get_latest_commit(${CMAKE_SOURCE_DIR} X_FLOW)
x_git_get_latest_commit_time(${CMAKE_SOURCE_DIR} X_FLOW)

# --------------------------------------------------------------------------------------------------
# Target platform
message(STATUS "------------------------------------------------------------")
message(STATUS "[xFlow]CMAKE_VERSION: ${CMAKE_VERSION}")
message(STATUS "[xFlow]CMAKE_GENERATOR: ${CMAKE_GENERATOR}")
message(STATUS "[xFlow]CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "[xFlow]CMAKE_SYSTEM: ${CMAKE_SYSTEM}")
message(STATUS "[xFlow]CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")
message(STATUS "[xFlow]CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
message(STATUS "[xFlow]CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "[xFlow]CMAKE_HOST_SYSTEM: ${CMAKE_HOST_SYSTEM}")
message(STATUS "[xFlow]CMAKE_HOST_SYSTEM_NAME: ${CMAKE_HOST_SYSTEM_NAME}")
message(STATUS "[xFlow]CMAKE_HOST_SYSTEM_PROCESSOR: ${CMAKE_HOST_SYSTEM_PROCESSOR}")
message(STATUS "[xFlow]CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "[xFlow]CMAKE_CXX_COMPILER_VERSION: ${CMAKE_CXX_COMPILER_VERSION}")

set(X_ASSET_NAME "xFlow-${CMAKE_SYSTEM_NAME}-${X_FLOW_LATEST_GIT_TAG}-${CMAKE_SYSTEM_PROCESSOR}")
if(MSVC)
  set(X_ASSET_NAME "${X_ASSET_NAME}-msvc")
elseif(MINGW)
  set(X_ASSET_NAME "${X_ASSET_NAME}-mingw")
else()
  # Linux, MacOS, etc.
endif()
string(TOLOWER ${X_ASSET_NAME} X_ASSET_NAME)
message(STATUS "[xFlow]Asset name: ${X_ASSET_NAME}")

set(libs_dir1 ${CMAKE_CURRENT_SOURCE_DIR}/libs)
set(libs_dir2 ${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}/${QT_VERSION}/${CMAKE_CXX_COMPILER_ID})
set(X_DEPLOY_LIBS_DIR ${libs_dir1}/${libs_dir2})
message(STATUS "[xFlow]Deploy libraries directory: ${X_DEPLOY_LIBS_DIR}")
message(STATUS "------------------------------------------------------------")

# --------------------------------------------------------------------------------------------------
# CMake modules
include(cmake/3rd.cmake)
include(cmake/common.cmake)
include(cmake/dmg/dmg.cmake)
include(cmake/linux/linux.cmake)
include(cmake/msix/msix.cmake)

# --------------------------------------------------------------------------------------------------
# All sources files
file(GLOB_RECURSE X_SOURCES "src/*.*")
list(APPEND X_SOURCES xFlow.qrc)
list(APPEND X_SOURCES xFlow.rc)

# --------------------------------------------------------------------------------------------------
# Microsoft Application Store
option(X_FLOW_STORE "Build for Microsoft Store" OFF)
if(X_FLOW_STORE)
  if(WIN32)
    add_compile_definitions(X_MS_STORE)
    include_directories(${CMAKE_SOURCE_DIR}/libs/microsoftstore/src)
    add_subdirectory(${CMAKE_SOURCE_DIR}/libs/microsoftstore)
  endif()
  if(APPLE)
    add_compile_definitions(X_APPLE_STORE)
  endif()
endif()

# --------------------------------------------------------------------------------------------------
# xFlow application
include_directories(src)
set(bin ${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}/xFlow)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${bin})
qt_add_executable(xFlow ${X_SOURCES})
x_deploy_qt(xFlow)
x_generate_translations(xFlow)
x_setup_python(xFlow)
x_generate_zip(xFlow ${X_FLOW_LATEST_GIT_TAG})
x_generate_dmg(xFlow ${X_FLOW_LATEST_GIT_TAG})
x_build_deb(xFlow "x-flow" "xFlow" ${X_FLOW_LATEST_GIT_TAG} ${CMAKE_SOURCE_DIR}/xFlow.png)

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
  set(libqlementine "qlementine_debug")
else()
  set(libqlementine "qlementine")
endif()

target_link_libraries(
  xFlow
  PRIVATE Qt6::Widgets
          Qt6::SerialPort
          Qt6::Svg
          Qt6::Network
          Qt6::WebSockets
          Qt6::Qml
          Qt6::OpenGL
          ${libqlementine}
          lua
          QtNodes::QtNodes
          QtAES::QtAES
          glog::glog)

if(WIN32)
  target_link_libraries(xFlow PRIVATE Dwmapi)
elseif(LINUX)
  target_link_libraries(xFlow PRIVATE dl)
endif()

# --------------------------------------------------------------------------------------------------
set(X_FLOW_ENABLE_PYTHON OFF)
if(NOT X_FLOW_ENABLE_PYTHON)
  # Remove all file starting with 'pyplugin' from X_SOURCES
  foreach(file ${X_SOURCES})
    if(${file} MATCHES "pyplugin.*")
      list(REMOVE_ITEM X_SOURCES ${file})
    endif()
  endforeach()
endif()

# --------------------------------------------------------------------------------------------------
# Copy all files in res/scripts to the output directory
file(GLOB_RECURSE X_FLOW_SCRIPTS "res/scripts/*.*")
foreach(file ${X_FLOW_SCRIPTS})
  get_filename_component(filename ${file} NAME)
  add_custom_command(
    TARGET xFlow
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${file} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endforeach()

# --------------------------------------------------------------------------------------------------
# Debug message output
option(X_FLOW_WITH_VS_CODE "Output message to vs code terminal" OFF)
if(NOT X_FLOW_WITH_VS_CODE)
  set_target_properties(
    xFlow
    PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER com.xtools.xflow
               MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
               MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
               MACOSX_BUNDLE TRUE
               WIN32_EXECUTABLE TRUE)
endif()

# --------------------------------------------------------------------------------------------------
# xFlow package
if(WIN32 AND X_FLOW_STORE)
  include(cmake/msix/msix.cmake)
  target_link_libraries(xFlow PRIVATE MicrosoftStore)
  add_compile_definitions(X_FLOW_STORE)

  # Remove 'v' from version string
  string(REGEX REPLACE "v" "" version ${X_FLOW_LATEST_GIT_TAG})
  x_ms_store_deploy_dlls(xFlow)
  x_generate_msix(xFlow "DataFlowStudio" "xFlow" ${version} FALSE)
endif()
