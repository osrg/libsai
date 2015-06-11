#if !defined(_SAI_ROCKER_ROUTER_H)

#define _SAI_ROCKER_ROUTER_H

#include "sai.h"
#include "saistatus.h"
#include "saitypes.h"

sai_status_t rocker_create_virtual_router(_Out_ sai_object_id_t*, _In_ uint32_t, _In_ const sai_attribute_t *);

#endif
