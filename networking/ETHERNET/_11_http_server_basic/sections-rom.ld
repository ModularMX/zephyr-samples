#include <zephyr/linker/iterable_sections.h>

// HTTP resource descriptors section
// This section holds all HTTP resource definitions (routes/endpoints)
ITERABLE_SECTION_ROM(http_resource_desc_http_service, Z_LINK_ITERABLE_SUBALIGN)
