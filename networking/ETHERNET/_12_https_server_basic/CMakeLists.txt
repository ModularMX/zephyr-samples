# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
find_package(Python REQUIRED COMPONENTS Interpreter)

project(http_server)

if(CONFIG_NET_SOCKETS_SOCKOPT_TLS AND
  CONFIG_MBEDTLS_KEY_EXCHANGE_PSK_ENABLED AND
  (CONFIG_NET_SAMPLE_PSK_HEADER_FILE STREQUAL "dummy_psk.h")
)
  add_custom_target(development_psk
    COMMAND ${CMAKE_COMMAND} -E echo "----------------------------------------------------------"
    COMMAND ${CMAKE_COMMAND} -E echo "--- WARNING: Using dummy PSK! Only suitable for        ---"
    COMMAND ${CMAKE_COMMAND} -E echo "--- development. Set NET_SAMPLE_PSK_HEADER_FILE to use ---"
    COMMAND ${CMAKE_COMMAND} -E echo "--- own pre-shared key.                                ---"
    COMMAND ${CMAKE_COMMAND} -E echo "----------------------------------------------------------"
  )
  add_dependencies(app development_psk)
endif()

set(CERTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/certs)

# Create custom command to generate certificates
add_custom_command(
  OUTPUT
    ${CERTS_DIR}/server_cert.der
    ${CERTS_DIR}/server_privkey.der
    ${CERTS_DIR}/ca_cert.der
    ${CERTS_DIR}/server_cert.pem
    ${CERTS_DIR}/server_privkey.pem
    ${CERTS_DIR}/ca_cert.pem
    ${CERTS_DIR}/ca_privkey.pem
  WORKING_DIRECTORY ${CERTS_DIR}
  COMMAND ${Python_EXECUTABLE} generate_certificates.py
  COMMENT "Generating TLS certificates"
  VERBATIM
)

# Add a target that depends on the certificate outputs
add_custom_target(sample_ca_cert DEPENDS
  ${CERTS_DIR}/server_cert.der
  ${CERTS_DIR}/server_privkey.der
  ${CERTS_DIR}/ca_cert.der
)

add_custom_target(sample_server_cert DEPENDS
  ${CERTS_DIR}/server_cert.der
  ${CERTS_DIR}/server_privkey.der
)

option(INCLUDE_HTML_CONTENT "Include the HTML content" ON)

target_sources(app PRIVATE src/main.c)

# Add project root to include paths so certificate.h can be found
target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

set(gen_dir ${ZEPHYR_BINARY_DIR}/include/generated/)

# Generate gzipped include files for web resources
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

# if(CONFIG_USB_DEVICE_STACK_NEXT)
#   include(${ZEPHYR_BASE}/samples/subsys/usb/common/common.cmake)
# endif()

target_link_libraries(app PRIVATE zephyr_interface zephyr)

zephyr_linker_sources(SECTIONS sections-rom.ld)
zephyr_linker_section_ifdef(CONFIG_NET_SAMPLE_HTTPS_SERVICE
  NAME http_resource_desc_test_https_service
  KVMA RAM_REGION GROUP RODATA_REGION
)
zephyr_linker_section_ifdef(CONFIG_NET_SAMPLE_HTTP_SERVICE
  NAME http_resource_desc_test_http_service
  KVMA RAM_REGION GROUP RODATA_REGION
)

foreach(inc_file
  server_cert.der
  server_privkey.der
)
  generate_inc_file_for_target(app
    src/certs/${inc_file}
    ${gen_dir}/${inc_file}.inc
  )
  # Ensure certificates are generated before inc files are generated
  add_dependencies(app sample_ca_cert)
endforeach()
