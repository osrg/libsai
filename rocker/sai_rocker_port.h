#if !defined(_SAI_ROCKER_PORT_H)

#define _SAI_ROCKER_PORT_H

#include "sai.h"
#include "saiport.h"

sai_status_t rocker_get_port_attribute(_In_ sai_object_id_t, _In_ uint32_t, _Inout_ sai_attribute_t*);

#endif
