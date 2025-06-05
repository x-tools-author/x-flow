function(x_add_3rd file_name)
  execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ${file_name}.zip
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/3rd)
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rd/${file_name})
endfunction()

function(x_deploy_3rd_libs target file_name)
  add_custom_target(
    ${target}-deploy
    COMMAND ${CMAKE_COMMAND} --install . --prefix ${X_DEPLOY_LIBS_DIR}/${file_name}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/3rd/${file_name}
    COMMENT "Deploy 3rd libraries")
endfunction()

# --------------------------------------------------------------------------------------------------
# qlementine[https://github.com/oclero/qlementine]
set(file_name "qlementine-1.2.2")
if(EXISTS ${X_DEPLOY_LIBS_DIR}/${file_name})
  set(CMAKE_PREFIX_PATH ${X_DEPLOY_LIBS_DIR}/${file_name})
  find_package(qlementine REQUIRED)
  include_directories(${X_DEPLOY_LIBS_DIR}/${file_name}/include)
  link_directories(${X_DEPLOY_LIBS_DIR}/${file_name}/lib)
else()
  x_add_3rd(${file_name})
  x_deploy_3rd_libs(qlementine ${file_name})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rd/${file_name}/lib/include)
  set_target_properties(qlementine PROPERTIES FOLDER "3rd")
  link_directories(${CMAKE_BINARY_DIR}/3rd/${file_name}/lib)
endif()

# --------------------------------------------------------------------------------------------------
# nodeeditor[https://github.com/paceholder/nodeeditor]
set(nodeeditor_file_name "nodeeditor-3.0.11")
if(EXISTS ${X_DEPLOY_LIBS_DIR}/${nodeeditor_file_name})
  set(CMAKE_PREFIX_PATH ${X_DEPLOY_LIBS_DIR}/${nodeeditor_file_name})
  find_package(QtNodes REQUIRED)
else()
  x_add_3rd(${nodeeditor_file_name})
  x_deploy_3rd_libs(QtNodes ${nodeeditor_file_name})
endif()

# --------------------------------------------------------------------------------------------------
# Lua script[https://github.com/lua/lua]
set(file_name "lua-5.4.7")
get_filename_component(COMPILER_DIR ${CMAKE_CXX_COMPILER} DIRECTORY)
message(STATUS "Compiler dir: ${COMPILER_DIR}")
if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/3rd/${file_name})
  execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ${file_name}.zip
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/3rd)
endif()
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rd/${file_name}/src)
if(MSVC)
  if(EXISTS ${CMAKE_BINARY_DIR}/lua.lib)
    link_directories(${CMAKE_BINARY_DIR})
  else()
    file(GLOB LUA_SRC ${CMAKE_CURRENT_SOURCE_DIR}/3rd/${file_name}/src/*.*)
    list(REMOVE_ITEM LUA_SRC ${CMAKE_CURRENT_SOURCE_DIR}/3rd/${file_name}/src/lua.c)
    add_library(lua STATIC ${LUA_SRC})
    set_target_properties(lua PROPERTIES FOLDER "3rd")
  endif()
elseif(MINGW)
  add_custom_target(
    build-lua ALL
    COMMAND ${COMPILER_DIR}/mingw32-make.exe mingw
    COMMAND ${CMAKE_COMMAND} -E copy_if_different src/lua54.dll ${CMAKE_SOURCE_DIR}/bin/xFlow
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/3rd/${file_name}
    COMMENT "Build lua")
  link_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rd/${file_name}/src)
elseif(LINUX)
  add_custom_target(
    build-lua ALL
    COMMAND make linux
    # COMMAND ${CMAKE_COMMAND} -E copy_if_different src/lua54.so ${CMAKE_SOURCE_DIR}/bin/xFlow
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/3rd/${file_name}
    COMMENT "Build lua")
  link_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rd/${file_name}/src)
endif()

# --------------------------------------------------------------------------------------------------
# Qt AES[https://github.com/bricke/Qt-AES]
set(file_name "Qt-AES-master")
x_add_3rd(${file_name})

# --------------------------------------------------------------------------------------------------
# glog[https://github.com/google/glog]
# cmake-format: off
set(WITH_GTEST OFF CACHE BOOL "Disable build gtest" FORCE)
set(WITH_GFLAGS OFF CACHE BOOL "Disable build gflags" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "Disable build testing" FORCE)
# cmake-format: on
set(file_name "glog-0.7.1")
if(EXISTS ${X_DEPLOY_LIBS_DIR}/${file_name})
  set(CMAKE_PREFIX_PATH ${X_DEPLOY_LIBS_DIR}/${file_name})
  find_package(glog REQUIRED)
else()
  x_add_3rd(${file_name})
  x_deploy_3rd_libs(glog ${file_name})
endif()

# --------------------------------------------------------------------------------------------------
# Python
if(WIN32)
  set(X_FLOW_ENABLE_PYTHON ON)
  add_compile_definitions(X_FLOW_ENABLE_PYTHON)
  set(file_name "Python313")
  add_compile_definitions(PY_HOME=L"Python313")
  if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/3rd/${file_name})
    execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ${file_name}.zip
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/3rd)
  endif()
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rd/${file_name}/include)
  link_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rd/${file_name}/libs)

  set(py3_dll ${CMAKE_CURRENT_SOURCE_DIR}/3rd/${file_name}/python3.dll)
  set(py313_dll ${CMAKE_CURRENT_SOURCE_DIR}/3rd/${file_name}/python313.dll)
  function(x_setup_python_win target)
    add_custom_command(
      TARGET ${target}
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different ${py3_dll} $<TARGET_FILE_DIR:${target}>
      COMMAND ${CMAKE_COMMAND} -E copy_if_different ${py313_dll} $<TARGET_FILE_DIR:${target}>
      COMMAND
        ${CMAKE_COMMAND} -E copy_directory_if_different ${CMAKE_CURRENT_SOURCE_DIR}/3rd/${file_name}
        "$<TARGET_FILE_DIR:${target}>/${file_name}"
      COMMENT "Copy python dll and lib to target directory")
    target_link_libraries(${target} PRIVATE python313)
  endfunction()
endif()

function(x_setup_python_linux target)
  find_package(
    Python3
    COMPONENTS Interpreter Development
    REQUIRED)
  target_link_libraries(${target} PRIVATE Python3::Python)
  add_compile_definitions(PY_HOME=L"/usr/bin/python3")
endfunction()

function(x_setup_python target)
  if(WIN32)
    x_setup_python_win(${target})
  elseif(APPLE)

  elseif(LINUX)
    x_setup_python_linux(${target})
  endif()
endfunction()
