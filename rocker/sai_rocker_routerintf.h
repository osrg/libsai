#if !defined(_SAI_ROCKER_ROUTERINF_H)

#define _SAI_ROCKER_ROUTERINF_H

#include "sai.h"
#include "saitypes.h"
#include "sairouterintf.h"

struct __router_interface {
  sai_object_id_t vlan_id;
};

sai_status_t rocker_create_router_interface(_Out_ sai_object_id_t*, _In_ uint32_t, _In_ sai_attribute_t*);

#endif
