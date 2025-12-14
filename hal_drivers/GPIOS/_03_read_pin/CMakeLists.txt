# Minimum CMake required version
cmake_minimum_required( VERSION 3.20.0 )

# set the board in use if you don't want to pass it as a parameter
# west build -p always -b <board_name> app 
#set(BOARD nrf54l15dk/nrf54l15/cpuapp)
#set(BOARD nucleo_g0b1re)
#set(BOARD native_sim)

# build with DT doctor to debug device tree issues
set(ZEPHYR_SCA_VARIANT dtdoctor)

# Get Zephyr directory
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

# Project and source files
project(demo)
target_sources(app PRIVATE src/main.c)