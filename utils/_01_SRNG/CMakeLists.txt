# Minimum CMake required version
cmake_minimum_required( VERSION 3.20.0 )

# Board-specific configuration files
set(CONF_FILE "prj.conf")
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/boards/${BOARD}.conf)
    set(CONF_FILE "prj.conf boards/${BOARD}.conf")
endif()

# Get Zephyr directory
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

# Project and source files
project(srng_demo)
target_sources(app PRIVATE src/main.c)