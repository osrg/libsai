#include <stdio.h>

#include "sai_rocker.h"
#include "sai_rocker_switch.h"

sai_switch_api_t rocker_sai_switch_api;

extern struct rtnl_handle rth;
extern struct __port_list* global_plist;

sai_status_t rocker_get_switch_attribute(_In_ uint32_t attr_count, _Inout_ sai_attribute_t *attr_list){
  int i;

  static uint32_t port_number;
  static int first_call = 1;

  // initialization, called once on the first call to this functions
  if(first_call){
    port_number = (uint32_t)get_port_number(global_plist);
    first_call = 0;
  }
  
  for(i=0;i<attr_count;i++){
    switch (attr_list[i].id){
    case SAI_SWITCH_ATTR_PORT_NUMBER:
      attr_list[i].value.u32 = port_number;
      goto next_attr;
    case SAI_SWITCH_ATTR_PORT_LIST:
      if(attr_list[i].value.objlist.count < port_number){
	attr_list[i].value.objlist.count = port_number;
	return SAI_STATUS_BUFFER_OVERFLOW;
      }
      else{
	int j;
	attr_list[i].value.objlist.count = port_number;
	for(j=0;j<port_number;j++){
	  attr_list[i].value.objlist.list[j] = (uint64_t)j;
	}
	goto next_attr;
      }
    default:
      fprintf(stderr, "Error: value %d of sai_port_attr_t is not supported\n", attr_list[i].id);
      return SAI_STATUS_NOT_SUPPORTED;
    }

  next_attr:;
  }

  // this function succeedes if (and only if) no error happens on the way.
  return SAI_STATUS_SUCCESS;
}

sai_status_t rocker_initialize_switch(_In_ sai_switch_profile_id_t profile_id,
				      _In_reads_z_(SAI_MAX_HARDWARE_ID_LEN) char* switch_hardware_id,
				      _In_reads_opt_z_(SAI_MAX_FIRMWARE_PATH_NAME_LEN) char* firmware_path_name,
				      _In_ sai_switch_notification_t* switch_notifications
				      ){
  int i, n_ports;

  global_plist = get_dev_list();
  n_ports = get_port_number(global_plist);

  for(i=0;i<n_ports;i++){
    struct __port* p = get_port(i);
    dev_up(p->name);
  }

  return SAI_STATUS_SUCCESS;
}
