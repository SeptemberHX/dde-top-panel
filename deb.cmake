set(_VERSION_MAJOR 0)
set(_VERSION_MINOR 6)
set(_VERSION_PATCH 8)

set(CPACK_GENERATOR "DEB")

############下面是设置debian/control文件中的内容

# 设置版本信息
set(CPACK_PACKAGE_VERSION_MAJOR "${_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${_VERSION_PATCH}")

# 设置安装包的包名，打好的包将会是packagename-version-linux.debian，如果不设置，默认是工程名
set(CPACK_PACKAGE_NAME "dde-top-panel")

# 设置程序名，就是程序安装后的名字
 set(CPACK_DEBIAN_PACKAGE_NAME "dde-top-panel")

# 设置架构
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")

# 设置依赖
set(CPACK_DEBIAN_PACKAGE_DEPENDS
        "libqt5dbus5, libqt5core5a, libqt5gui5, libxcb-ewmh2, libxcb1, libdframeworkdbus2,
         libdtkcore5-bin, libkf5windowsystem5, libqt5svg5, libqt5concurrent5, libgsettings-qt1, libxcb-util1")

# 设置section
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")

# 设置priority
set(CPACK_DEBIAN_PACKAGE_PRIORITY "Optional")

# 设置description
set(CPACK_PACKAGE_DESCRIPTION "DDE Top Panel is a top bar supporting the global menu for Deepin V20")

# 设置联系方式
set(CPACK_PACKAGE_CONTACT "september_hx@outlook.com")

# 设置维护人
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "SeptemberHX")

##############debian/control设置完成了，下面设置控制脚本，动态库，changelog等

# lib目录下保存这一些动态库和安装控制脚本（postinst、postinst、preinst、prerm）  doc目录下是changelog和copyright等文件 这些东西是打包时要依赖的一些额外的东西
set(LIBS_DIR ${CMAKE_SOURCE_DIR}/debian/scripts)
set(DOCS_DIR ${CMAKE_SOURCE_DIR}/debian/doc)

# 设置控制脚本，这些都是打包者本人自己准备的，没有就算了
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${LIBS_DIR}/postinst;${LIBS_DIR}/prerm")

# 压缩 changelog
include(GNUInstallDirs)

add_custom_command(
    OUTPUT "${CMAKE_BINARY_DIR}/changelog.gz"
    COMMAND gzip -cn9 "${DOCS_DIR}/changelog" > "${CMAKE_BINARY_DIR}/changelog.gz"
    WORKING_DIRECTORY "${DOCS_DIR}"
    DEPENDS "${DOCS_DIR}/changelog"
    COMMENT "Compressing changelog"
)

add_custom_target(changelog ALL DEPENDS "${CMAKE_BINARY_DIR}/changelog.gz")

install(FILES ${CMAKE_BINARY_DIR}/frame/dde-top-panel DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${CMAKE_SOURCE_DIR}/xml/com.deepin.dde.toppanel.gschema.xml DESTINATION ${CMAKE_INSTALL_DATADIR}/glib-2.0/schemas)
install(FILES ${CMAKE_SOURCE_DIR}/dde-top-panel.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
install(DIRECTORY ${CMAKE_BINARY_DIR}/translations DESTINATION ${CMAKE_INSTALL_DATADIR}/dde-top-panel)


# 最后把我们的changelog等东西也加进来，project是你的工程名
install(FILES ${CMAKE_BINARY_DIR}/changelog.gz ${DOCS_DIR}/copyright DESTINATION ${CMAKE_INSTALL_DOCDIR})
