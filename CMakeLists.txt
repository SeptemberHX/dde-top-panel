cmake_minimum_required(VERSION 3.7)

if (NOT DEFINED VERSION)
    set(VERSION 4.0)
endif()

project(dde-top-panel)

#set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_FLAGS "-g -Wall")

# generate a compile commands file as complete database for vim-YouCompleteMe or some other similar tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(GNUInstallDirs)
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX /usr)
endif ()

if (NOT (${CMAKE_BUILD_TYPE} MATCHES "Debug"))
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")

    # generate qm
    execute_process(COMMAND bash "translate_generation.sh"
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif ()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DQT_DEBUG")

# Test architecture
if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "sw_64")
    # add compiler flags -mieee for mathmatic
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mieee")
    add_definitions(-DDISABLE_SHOW_ANIMATION)
endif ()

file(GLOB INTERFACES "interfaces/*.h")

add_subdirectory("frame")
#add_subdirectory("plugins")
add_subdirectory("appmenu")

if (CMAKE_BUILD_TYPE STREQUAL "RELEASE")
    add_definitions(-DQT_NO_DEBUG_OUTPUT)
endif (CMAKE_BUILD_TYPE STREQUAL "RELEASE")

execute_process(COMMAND apt --version OUTPUT_VARIABLE APTVERSION RESULT_VARIABLE EXIT_CODE)
if (${EXIT_CODE} EQUAL 0)
    message(STATUS "apt ${APTVERSION} - has found： CPank module enable")
    include(deb.cmake)
    include(CPack)
else()
    message(STATUS "apt not found! - CPank module disable")
    install(FILES ${CMAKE_BINARY_DIR}/frame/dde-top-panel DESTINATION ${CMAKE_INSTALL_BINDIR} 
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
    install(FILES ${CMAKE_SOURCE_DIR}/xml/com.deepin.dde.toppanel.gschema.xml DESTINATION ${CMAKE_INSTALL_DATADIR}/glib-2.0/schemas)
    install(FILES ${CMAKE_SOURCE_DIR}/dde-top-panel.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
    install(DIRECTORY ${CMAKE_BINARY_DIR}/translations DESTINATION ${CMAKE_INSTALL_DATADIR}/dde-top-panel)
endif()
