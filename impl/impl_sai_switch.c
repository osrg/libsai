// Copyright (C) 2015 Nippon Telegraph and Telephone Corporation.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
// implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdio.h>
#include <stdlib.h>
#include "impl_sai_switch.h"

#include "../rocker/sai_rocker.h"
#include "../rocker/sai_rocker_fdb.h"

extern struct __port_list* global_plist;
sai_switch_api_t rocker_sai_switch_api;

sai_status_t impl_get_switch_attribute(_In_ uint32_t attr_count, _Inout_ sai_attribute_t *attr_list){
  int i, n_ports;

  // data plane access
  n_ports = rocker_get_port_number();
  
  for(i=0;i<attr_count;i++){
    switch (attr_list[i].id){
    case SAI_SWITCH_ATTR_PORT_NUMBER:
      attr_list[i].value.u32 = n_ports;
      goto next_attr;
      
    case SAI_SWITCH_ATTR_PORT_LIST:
      if(attr_list[i].value.objlist.count < n_ports){
	attr_list[i].value.objlist.count = n_ports;
	return SAI_STATUS_BUFFER_OVERFLOW;
      }
      else{
	int j;
	attr_list[i].value.objlist.count = n_ports;
	for(j=0;j<n_ports;j++){
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

sai_status_t impl_initialize_switch(_In_ sai_switch_profile_id_t profile_id,
				      _In_reads_z_(SAI_MAX_HARDWARE_ID_LEN) char* switch_hardware_id,
				      _In_reads_opt_z_(SAI_MAX_FIRMWARE_PATH_NAME_LEN) char* firmware_path_name,
				      _In_ sai_switch_notification_t* switch_notifications
				      ){
  if(switch_notifications != NULL){
    if(switch_notifications->on_fdb_event != NULL){
      pthread_t pt;
      struct fdb_update_nitifier_arg* arg = malloc(sizeof(struct fdb_update_nitifier_arg));
      arg->f = switch_notifications->on_fdb_event;
      arg->interval = 1;
      pthread_create(&pt, NULL, fdb_update_notifier, (void*)arg);
    }

    // to be added for other event types
  }

  // data plane
  global_plist = get_dev_list();
  set_ports_up(global_plist);
  
  return SAI_STATUS_SUCCESS;
}
