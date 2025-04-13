set(qt_components Core Gui Widgets)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS ${qt_components})
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${qt_components})

qt_add_plugin(datachecker CLASS_NAME DataChecker datachecker.cpp datachecker.h datachecker.ui)

set_target_properties(datachecker PROPERTIES LIBRARY_OUTPUT_DIRECTORY
                                             ${X_FLOW_PLUGIN_OUTPUT_DIRECTORY}/datachecker/lib)
set(data_checker_libs Qt${QT_VERSION_MAJOR}::Core)
list(APPEND data_checker_libs Qt${QT_VERSION_MAJOR}::Gui)
list(APPEND data_checker_libs Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(datachecker PRIVATE ${data_checker_libs})
add_custom_command(
  TARGET datachecker
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory ${X_FLOW_PLUGIN_OUTPUT_DIRECTORY}/datachecker/conf
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/conf.json
          ${X_FLOW_PLUGIN_OUTPUT_DIRECTORY}/datachecker/conf/conf.json)

# cmake-format: off
qt_standard_project_setup()
set(install_dir "${CMAKE_BINARY_DIR}/plugins/datachecker")
install(TARGETS datachecker RUNTIME DESTINATION "${install_dir}" LIBRARY DESTINATION "${install_dir}")
# cmake-format: on
