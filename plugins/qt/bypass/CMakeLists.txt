set(qt_components Core Gui Widgets)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS ${qt_components})
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${qt_components})

qt_add_plugin(bypass_qt CLASS_NAME Bypass bypass.cpp bypass.h)

set(bypass_libs Qt${QT_VERSION_MAJOR}::Core)
list(APPEND bypass_libs Qt${QT_VERSION_MAJOR}::Gui)
list(APPEND bypass_libs Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(bypass_qt PRIVATE ${bypass_libs})

# --------------------------------------------------------------------------------------------------
qt_standard_project_setup()
