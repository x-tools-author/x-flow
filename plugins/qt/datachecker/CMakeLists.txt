set(qt_components Core Gui Widgets)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS ${qt_components})
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${qt_components})

qt_add_plugin(
  datachecker
  CLASS_NAME
  DataChecker
  datachecker.cpp
  datachecker.h
  datacheckerui.cpp
  datacheckerui.h
  datacheckerui.ui)

set(data_checker_libs Qt${QT_VERSION_MAJOR}::Core)
list(APPEND data_checker_libs Qt${QT_VERSION_MAJOR}::Gui)
list(APPEND data_checker_libs Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(datachecker PRIVATE ${data_checker_libs})

# --------------------------------------------------------------------------------------------------
qt_standard_project_setup()
