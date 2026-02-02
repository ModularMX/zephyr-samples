#include <zephyr/linker/iterable_sections.h>

// HTTP resource descriptors section for HTTPS service
// This section holds all HTTP resource definitions (routes/endpoints)
ITERABLE_SECTION_ROM(http_resource_desc_https_service, Z_LINK_ITERABLE_SUBALIGN)
