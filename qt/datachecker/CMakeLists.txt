set(qt_components Core Gui Widgets)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS ${qt_components})
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${qt_components})

qt_add_plugin(datachecker CLASS_NAME DataChecker datachecker.cpp datachecker.h
              datachecker.ui)

set(tmp "${CMAKE_BINARY_DIR}/plugins/datachecker")
set_target_properties(datachecker PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${tmp})
target_include_directories(datachecker PRIVATE ../xflowplugininterface)
set(data_checker_libs Qt${QT_VERSION_MAJOR}::Core)
list(APPEND data_checker_libs Qt${QT_VERSION_MAJOR}::Gui)
list(APPEND data_checker_libs Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(datachecker PRIVATE ${data_checker_libs})

# cmake-format: off
set(install_dir "${CMAKE_BINARY_DIR}/plugins/datachecker")
qt_standard_project_setup()
install(TARGETS datachecker RUNTIME DESTINATION "${install_dir}" LIBRARY DESTINATION "${install_dir}")
# cmake-format: on
