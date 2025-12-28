# Minimum CMake required version
cmake_minimum_required( VERSION 3.20.0 )

# set the board in use if you don't want to pass it as a parameter
# west build -p always -b <board_name> app 
#set(BOARD nrf54l15dk/nrf54l15/cpuapp)
#set(BOARD nucleo_g0b1re)
#set(BOARD native_sim)

# WARNING: DT doctor generates very long command lines on Windows
# which can exceed the Windows command line limit (8191 chars).
# Only enable temporarily for device tree debugging.
# set(ZEPHYR_SCA_VARIANT dtdoctor)

# Configurable toggle delay in milliseconds (default: 300ms)
# Override with: west build -b <board> -- -DTOGGLE_DELAY_MS=500
if(NOT DEFINED TOGGLE_DELAY_MS)
    set(TOGGLE_DELAY_MS 300)
endif()

# Get Zephyr directory
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

# Project and source files
project(_01_toggle_pin)
target_sources(app PRIVATE src/main.c)

# Pass toggle delay to source code as compile definition
target_compile_definitions(app PRIVATE TOGGLE_DELAY_MS=${TOGGLE_DELAY_MS})

