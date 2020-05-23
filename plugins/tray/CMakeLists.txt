
set(PLUGIN_NAME "tray")

project(${PLUGIN_NAME})

# Sources files
file(GLOB_RECURSE SRCS "*.h" "*.cpp" "../../widgets/*.h" "../../widgets/*.cpp"
    "../../frame/util/themeappicon.h" "../../frame/util/themeappicon.cpp"
    "../../frame/util/dockpopupwindow.h" "../../frame/util/dockpopupwindow.cpp"
    "../../frame/util/abstractpluginscontroller.h" "../../frame/util/abstractpluginscontroller.cpp"
    "../../frame/util/pluginloader.h" "../../frame/util/pluginloader.cpp"
    "../../frame/dbus/sni/*.h" "../../frame/dbus/sni/*.cpp"
    "../../frame/dbus/dbusmenu.h" "../../frame/dbus/dbusmenu.cpp"
    "../../frame/dbus/dbusmenumanager.h" "../../frame/dbus/dbusmenumanager.cpp"
     "../../widgets/*.h" "../../widgets/*.cpp"
     "../../frame/util/imageutil.h" "../../frame/util/imageutil.cpp")

find_package(PkgConfig REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Svg REQUIRED)
find_package(Qt5DBus REQUIRED)
find_package(Qt5X11Extras REQUIRED)
find_package(DtkWidget REQUIRED)

pkg_check_modules(XCB_LIBS REQUIRED xcb-ewmh xcb xcb-image xcb-composite xtst x11 xext xcb-icccm dbusmenu-qt5)
pkg_check_modules(DDE-Network-Utils REQUIRED dde-network-utils)
pkg_check_modules(DFrameworkDBus REQUIRED dframeworkdbus)
pkg_check_modules(QGSettings REQUIRED gsettings-qt)

add_definitions("${QT_DEFINITIONS} -DQT_PLUGIN")
add_library(${PLUGIN_NAME} SHARED ${SRCS} tray.qrc)
set_target_properties(${PLUGIN_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ../)
target_include_directories(${PLUGIN_NAME} PUBLIC ${DtkWidget_INCLUDE_DIRS}
                                                 ${Qt5DBus_INCLUDE_DIRS}
                                                 ${XCB_LIBS_INCLUDE_DIRS}
                                                 ${DDE-Network-Utils_INCLUDE_DIRS}
                                                 ${DFrameworkDBus_INCLUDE_DIRS}
                                                 ${QGSettings_INCLUDE_DIRS}
                                                 ../../interfaces
                                                 ../../frame )
target_link_libraries(${PLUGIN_NAME} PRIVATE
    ${DtkWidget_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    ${Qt5X11Extras_LIBRARIES}
    ${Qt5DBus_LIBRARIES}
    ${Qt5Svg_LIBRARIES}
    ${XCB_LIBS_LIBRARIES}
    ${DDE-Network-Utils_LIBRARIES}
    ${DFrameworkDBus_LIBRARIES}
    ${QGSettings_LIBRARIES}
    pthread
)

install(TARGETS ${PLUGIN_NAME} LIBRARY DESTINATION lib/dde-dock/plugins)
