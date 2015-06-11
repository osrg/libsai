#include <stdio.h>

#include "sai_rocker.h"
#include "sai_rocker_port.h"

sai_port_api_t rocker_sai_port_api;

sai_status_t rocker_get_port_attribute(_In_ sai_object_id_t port_id, _In_ uint32_t attr_count, _Inout_ sai_attribute_t *attr_list){
  uint32_t i;

  for(i=0;i<attr_count;i++){
    switch (attr_list[i].id){
    case SAI_PORT_ATTR_SPEED:
      attr_list[i].value.u32 = (uint32_t)get_port_speed(get_port(port_id)->name);
      return SAI_STATUS_SUCCESS;
    default:
      fprintf(stderr, "Error: value %d of sai_port_attr_t is not supported\n", attr_list[i].id);
      return SAI_STATUS_NOT_SUPPORTED;
    }
  }
}
