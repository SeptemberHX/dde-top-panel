cmake_minimum_required(VERSION 3.7)

set(VERSION 4.0)

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

include(deb.cmake)
include(CPack)
