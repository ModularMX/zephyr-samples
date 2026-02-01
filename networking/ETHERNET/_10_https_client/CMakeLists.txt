cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(https_client)

# Add application source files
target_sources(app PRIVATE
    src/main.c
    src/net_sample_common.c
)

# Set output directory for generated files
set(gen_dir ${ZEPHYR_BINARY_DIR}/include/generated/)

# Convert binary certificate (.der) to C header file (.inc)
# This generates an includable C array at build time
generate_inc_file_for_target(
    app
    src/https-cert.der
    ${gen_dir}/https-cert.der.inc
)
