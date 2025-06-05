# --------------------------------------------------------------------------------------------------
# Qt module
list(APPEND X_MS_STORE_COMPONENTS Gui)
list(APPEND X_MS_STORE_COMPONENTS Svg)
list(APPEND X_MS_STORE_COMPONENTS Core)
list(APPEND X_MS_STORE_COMPONENTS Widgets)

find_package(Qt6 REQUIRED COMPONENTS ${X_MS_STORE_COMPONENTS})
find_package(Qt6 REQUIRED COMPONENTS ${X_MS_STORE_COMPONENTS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
file(GLOB MICROSOFT_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/*.*)

# using this includes

add_library(MicrosoftStore STATIC ${MICROSOFT_SOURCE})
target_link_libraries(MicrosoftStore PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)

function(x_ms_store_deploy_dlls target)
  set(microsoft_dll_dir "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/bin")
  add_custom_command(
    TARGET ${target}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${microsoft_dll_dir}/WindowsStoreDLL.dll"
            "$<TARGET_FILE_DIR:${target}>/WindowsStoreDLL.dll"
    COMMENT "Copying Microsoft Store DLLs for ${target}")
endfunction()
