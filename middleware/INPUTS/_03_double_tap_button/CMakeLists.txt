# Minimum CMake required version
cmake_minimum_required( VERSION 3.20.0 )

# Get Zephyr directory
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

# Project and source files
project(double_tap_button)
target_sources(app PRIVATE src/main.c)
