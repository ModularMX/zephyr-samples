# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(http_server)

# Add application source files
target_sources(app PRIVATE src/main.c)

# Set output directory for generated files
set(gen_dir ${ZEPHYR_BINARY_DIR}/include/generated/)

# Link with Zephyr
target_link_libraries(app PRIVATE zephyr_interface zephyr)

# Configure linker for HTTP resource sections
zephyr_linker_sources(SECTIONS sections-rom.ld)
zephyr_linker_section_ifdef(CONFIG_HTTP_SERVER
  NAME http_resource_desc_http_service
  KVMA RAM_REGION GROUP RODATA_REGION
)

# Compress and include HTML/JS resources
foreach(web_resource
  index.html
  main.js
)
  generate_inc_file_for_target(app
    src/static_web_resources/${web_resource}
    ${gen_dir}/${web_resource}.gz.inc
    --gzip
  )
endforeach()