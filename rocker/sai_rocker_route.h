#if !defined (_SAI_ROCKER_ROUTE_H)

#define _SAI_ROCKER_ROUTE_H

#include "saistatus.h"

sai_status_t rocker_create_route(_In_ const sai_unicast_route_entry_t*, _In_ uint32_t, _In_ const sai_attribute_t*);

#endif
