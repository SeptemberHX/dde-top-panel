cmake_minimum_required(VERSION 3.7)

set(BIN_NAME dde-top-panel)

# Sources files
file(GLOB_RECURSE SRCS "*.h" "*.cpp" "../widgets/*.h" "../widgets/*.cpp" "util/XUtils.h"
        "util/XUtils.cpp" "../appmenu/*.h" "../appmenu/*.cpp" "../widgets/*.ui ../widgets/QOperationWidget.cpp" "../widgets/QOperationWidget.h")

# Find the library
find_package(PkgConfig REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Concurrent REQUIRED)
find_package(Qt5X11Extras REQUIRED)
find_package(Qt5DBus REQUIRED)
find_package(Qt5Svg REQUIRED)
find_package(DtkWidget REQUIRED)
find_package(DtkCore REQUIRED)
find_package(DtkCMake REQUIRED)
find_package(KF5WindowSystem REQUIRED)

pkg_check_modules(XCB_EWMH REQUIRED xcb-ewmh x11)
pkg_check_modules(DFrameworkDBus REQUIRED dframeworkdbus)
pkg_check_modules(QGSettings REQUIRED gsettings-qt)
pkg_check_modules(DtkGUI REQUIRED dtkgui)

find_package(Qt5LinguistTools REQUIRED)

set(TS_FILES
        ${CMAKE_SOURCE_DIR}/translations/dde-top-panel_zh_CN.ts
        )

set_source_files_properties(${TS_FILES} PROPERTIES OUTPUT_LOCATION ${CMAKE_BINARY_DIR}/translations)
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../widgets ${TS_FILES})
qt5_add_translation(QM_FILES ${TS_FILES})

# driver-manager
add_executable(${BIN_NAME} ${SRCS} ${INTERFACES} ${QM_FILES} item/item.qrc frame.qrc)
target_include_directories(${BIN_NAME} PUBLIC ${DtkWidget_INCLUDE_DIRS}
        ${XCB_EWMH_INCLUDE_DIRS}
        ${DFrameworkDBus_INCLUDE_DIRS}
        ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
        ${PROJECT_BINARY_DIR}
        ${QGSettings_INCLUDE_DIRS}
        ${DtkGUI_INCLUDE_DIRS}
        ${DtkCore_INCLUDE_DIRS}
        ${KF5WindowSystem_INCLUDE_DIRS}
        ../interfaces
        controller
        dbus
        item
        item/components
        util
        window
        xcb
        )

target_link_libraries(${BIN_NAME} PRIVATE
        ${XCB_EWMH_LIBRARIES}
        ${DFrameworkDBus_LIBRARIES}
        ${DtkWidget_LIBRARIES}
        ${Qt5Widgets_LIBRARIES}
        ${Qt5Concurrent_LIBRARIES}
        ${Qt5X11Extras_LIBRARIES}
        ${Qt5DBus_LIBRARIES}
        ${QGSettings_LIBRARIES}
        ${DtkGUI_LIBRARIES}
        ${Qt5Svg_LIBRARIES}
        ${DtkCore_LIBRARIES}
        KF5::WindowSystem
        xdo
        X11
        )

if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "sw_64")
    target_compile_definitions(${BIN_NAME} PUBLIC DISABLE_SHOW_ANIMATION)
endif()

if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "mips64")
    target_compile_definitions(${BIN_NAME} PUBLIC DISABLE_SHOW_ANIMATION)
endif()

if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "aarch64")
    target_compile_definitions(${BIN_NAME} PUBLIC DISABLE_SHOW_ANIMATION)
endif()
