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
#include <string.h>

#include "impl_sai_switch.h"
#include "impl_sai_vlan.h"
#include "impl_sai_util.h"
#include "../rocker/sai_rocker.h"
#include "../rocker/sai_rocker_fdb.h"

#define NO_DATA_PLANE

struct __switch {
  sai_vlan_id_t default_port_vlan_id;
  sai_mac_t src_mac_address;
};

static struct __switch* sw;
extern struct __port_list* global_plist;
sai_switch_api_t rocker_sai_switch_api;

sai_object_id_t* ports;
sai_vlan_id_t* port_vlans;

int get_port_number(){
  return rocker_get_port_number();
}

sai_status_t impl_get_switch_attribute(_In_ uint32_t attr_count, _Inout_ sai_attribute_t *attr_list){
  int i, port_number;

  // data plane access
  port_number = get_port_number();
  
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

    case SAI_SWITCH_ATTR_DEFAULT_PORT_VLAN_ID:
      attr_list[i].value.u16 = sw->default_port_vlan_id;
      goto next_attr;

    case SAI_SWITCH_ATTR_SRC_MAC_ADDRESS:
      memcpy(attr_list[i].value.mac, sw->src_mac_address, 6);
      goto next_attr;

    default:
      fprintf(stderr, "Error: value %d of sai_port_attr_t is not supported\n", attr_list[i].id);
      return SAI_STATUS_NOT_SUPPORTED;
    }

  next_attr:;
  }

  // this function succeedes if (and only if) no error happens on the way.
  return SAI_STATUS_SUCCESS;
}

static sai_status_t default_port_vlan_set(sai_vlan_id_t vlan_id_new){
    sai_status_t ret;
    struct __vlan* vlan_current;

    // create a new vlan
    ret = impl_create_vlan(vlan_id_new);
    if (ret != SAI_STATUS_SUCCESS) {
      return ret;
    }

    // retrieving the current default vlan.
    // This must be executed after impl_create_vlan(vlan_id_new), which realloc()s vlans
    vlan_current = get_vlan(sw->default_port_vlan_id);

    fprintf(stderr, "Default vlan changed from %u to %u\n", vlan_current->id, vlan_id_new);

    // Add ports of old default vlan to the new default vlan.
    sai_vlan_port_t* ports_to_add = (sai_vlan_port_t*)malloc(sizeof(sai_vlan_port_t) * vlan_current->number_of_ports);
    memcpy(ports_to_add, vlan_current->port_list, sizeof(sai_vlan_port_t) * vlan_current->number_of_ports);
    impl_add_ports_to_vlan(vlan_id_new, vlan_current->number_of_ports, ports_to_add);
    free(ports_to_add);

    // set the default vlan to the new id and remove the old one
    sw->default_port_vlan_id = vlan_id_new;
    impl_remove_vlan(vlan_current->id);

    return SAI_STATUS_SUCCESS;
}

sai_status_t sai_set_switch_attribute(_In_ const sai_attribute_t *attr){
  sai_status_t ret;
  sai_vlan_id_t vlan_id;
  
  switch(attr->id){
  case SAI_SWITCH_ATTR_SWITCHING_MODE:
    
  case SAI_SWITCH_ATTR_DEFAULT_PORT_VLAN_ID:
    vlan_id = attr->value.u16;

    if(vlan_id_range_ok(vlan_id)){
      ret = default_port_vlan_set(vlan_id);

      if(ret != SAI_STATUS_SUCCESS){
	return ret;
      }
    }
    else{
      fprintf(stderr, "vlan_id (%d) must satisfy 1 <= vlan_id <= 4095\n", vlan_id);
      return SAI_STATUS_INVALID_VLAN_ID;
    }
    break;

  case SAI_SWITCH_ATTR_SRC_MAC_ADDRESS:
    memcpy(sw->src_mac_address, attr->value.mac, 6);
    break;

  default:
    fprintf(stderr, "switch attribute %d is not yet supported\n", attr->id);
    return SAI_STATUS_NOT_SUPPORTED;
  }

  return SAI_STATUS_SUCCESS;
}

sai_status_t impl_initialize_switch(_In_ sai_switch_profile_id_t profile_id,
				      _In_reads_z_(SAI_MAX_HARDWARE_ID_LEN) char* switch_hardware_id,
				      _In_reads_opt_z_(SAI_MAX_FIRMWARE_PATH_NAME_LEN) char* firmware_path_name,
				      _In_ sai_switch_notification_t* switch_notifications
				      ){
  int number_of_ports, i;

  // data plane
  global_plist = get_dev_list();
  number_of_ports = get_port_number();

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

  sw = (struct __switch*)malloc(sizeof(struct __switch));
  sw->default_port_vlan_id = 1; // default value of for default vlan id

  ports = (sai_object_id_t*)malloc(sizeof(sai_object_id_t) * number_of_ports);
  port_vlans = (sai_vlan_id_t*)malloc(sizeof(sai_vlan_id_t) * number_of_ports);
  if (ports == NULL || port_vlans == NULL) {
    fprintf(stderr, "Cannot allocate sufficient amount of memory for the ports.\n");
    return SAI_STATUS_NO_MEMORY;
  }
  for (i = 0; i < number_of_ports; i++) {
    ports[i] = i;
    port_vlans[i] = VLAN_ID_NOT_ASSIGNED;
  }
  
  // data plane, must be executed before adding ports to the default vlan?
  set_ports_up(global_plist);

  // assign all ports to the default vlan
  sai_vlan_port_t* vlan_ports = (sai_vlan_port_t*)malloc(sizeof(sai_vlan_port_t) * number_of_ports);
  for (i = 0; i < number_of_ports; i++) {
    vlan_ports[i].port_id = ports[i];
    vlan_ports[i].tagging_mode = SAI_VLAN_PORT_TAGGED;
  }
  sw->default_port_vlan_id = 1;
  impl_create_vlan(1);
  impl_add_ports_to_vlan(1, number_of_ports, vlan_ports);
  free(vlan_ports);

  return SAI_STATUS_SUCCESS;
}
