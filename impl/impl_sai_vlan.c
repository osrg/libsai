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

#include "impl_sai_vlan.h"
#include "../dataplane/rocker/rocker_vlan.h"

sai_vlan_api_t rocker_vlan_api;

static int number_of_vlans;
struct __vlan* vlans = NULL;

// instantiated in impl_sai_switch.c
extern sai_object_id_t* ports;
extern sai_vlan_id_t* port_vlans;

struct __vlan* get_vlan(sai_object_id_t vlan_id){
  int i;

  for(i=0;i<number_of_vlans;i++){
    if(vlans[i].id == vlan_id)
      return &vlans[i];
  }

  return NULL;
}

sai_status_t impl_create_vlan(sai_vlan_id_t vlan_id){
  int i;
  sai_status_t ret;

  // make sure the given vlan_id is available
  for(i=0;i<number_of_vlans;i++){
    if(vlans[i].id == vlan_id){
      fprintf(stderr, "Error: given vlan_id (%d) already exsits.\n", vlan_id);
      return SAI_STATUS_INVALID_VLAN_ID;
    }
  }

  // add new vlan
  number_of_vlans++;
  vlans = realloc(vlans, number_of_vlans * sizeof(struct __vlan));

  if(vlans == NULL){
    fprintf(stderr, "Error: memory allocation for creating a new vlan failed.\n");
    return SAI_STATUS_NO_MEMORY;
  }

  struct __vlan* v = &(vlans[number_of_vlans - 1]);
  v->id = vlan_id;
  v->number_of_ports = 0;
  v->port_list = NULL;

  // data plane
  ret = rocker_create_vlan(v);
}

sai_status_t impl_remove_vlan(sai_vlan_id_t vlan_id){
  int i, index_removed_vlan = -1;

  // make sure the given vlan_id exists
  for(i=0;i<number_of_vlans;i++){
    if(vlans[i].id == vlan_id){
      index_removed_vlan = i;
      break;
    }
  }
  if(index_removed_vlan == -1){
    fprintf(stderr, "Error: the given vlan id (%d) does not exist.\n", vlan_id);
    return SAI_STATUS_INVALID_VLAN_ID;
  }

  // delete the vlans[index_removed_vlan]
  for(i=0;i<number_of_vlans;i++){
    if(i > index_removed_vlan){
      vlans[i-1] = vlans[i];
    }
  }
  number_of_vlans--;
  vlans = realloc(vlans, sizeof(struct __vlan) * number_of_vlans);

  // data plane
  // not yet implemented

  return SAI_STATUS_SUCCESS;
}  

sai_status_t impl_add_ports_to_vlan(_In_ sai_vlan_id_t vlan_id, 
				    _In_ uint32_t port_count,
				    _In_ const sai_vlan_port_t* port_list){
  sai_status_t ret;
  int i, index_target_vlan = -1;
  int number_of_ports = get_port_number();
  char msg[256];
  
  for(i=0;i<number_of_vlans;i++){
    if(vlans[i].id == vlan_id){
      index_target_vlan = i;
      break;
    }
  }

  if(index_target_vlan == -1){
    fprintf(stderr, "Error: a VLAN with the given vlan id (%d) does not exit.\n", vlan_id);
    return SAI_STATUS_INVALID_VLAN_ID;
  }

  // remove the given ports from the old vlan to which they have belonged
  for (i = 0; i < (int)port_count; i++) {
    if ((int)port_list[i].port_id >= number_of_ports){
      fprintf(stderr, "the given port id (%d) is too large\n", port_list[i].port_id);
      return SAI_STATUS_INVALID_PORT_NUMBER;
    }
    
    sai_vlan_id_t old_vlan = port_vlans[port_list[i].port_id];
    if (old_vlan != VLAN_ID_NOT_ASSIGNED) {
      impl_remove_ports_from_vlan(old_vlan, 1, &port_list[i]);
    }
    port_vlans[port_list[i].port_id] = vlan_id;
  }
  
  struct __vlan* v = &vlans[index_target_vlan];
  int old_size = v->number_of_ports, new_size = old_size + port_count;
  v->port_list = realloc(v->port_list, new_size * sizeof(sai_vlan_port_t));

  if(v->port_list == NULL){
    fprintf(stderr, "Error: memory allocation for adding vlan ports failed.\n");
    return SAI_STATUS_NO_MEMORY;
  }

  v->number_of_ports = new_size;
  memcpy(v->port_list + old_size, port_list, port_count * sizeof(sai_vlan_port_t));
  
  sprintf(msg, "Ports added to VLAN(%u):", vlan_id);
  for (i = 0; i < v->number_of_ports; i++) {
    sprintf(msg, "%s %d", msg, (int)v->port_list[i].port_id);
  }
  fprintf(stderr, "%s\n", msg);
  
  sprintf(msg, "Vlan assigned to each port:");
  for (i = 0; i < /*global*/number_of_ports; i++) {
    sprintf(msg, "%s %d", msg, port_vlans[i]);
  }
  fprintf(stderr, "%s\n", msg);
  
  // data plane
  ret = rocker_add_ports_to_vlan(v, port_count, port_list);

  return ret;
}

static int remove_a_port_from_vlan(struct __vlan* v, sai_vlan_port_t port){
  int i, j;
  int deleted = 0;
  
  for(i=0;i<v->number_of_ports;i++){
    if(port.port_id == v->port_list[i].port_id){
      // delete this port
      deleted = 1;

      for(j=i;j<v->number_of_ports;j++){
	v->port_list[j-1] = v->port_list[j-1];
      }

      (v->number_of_ports)--;
      v->port_list = realloc(v->port_list, sizeof(sai_vlan_port_t) * v->number_of_ports);

      // data plane
      rocker_remove_a_port_from_vlan(v, port);

      break;
    }
  }

  return deleted;
}

sai_status_t impl_remove_ports_from_vlan(_In_ sai_vlan_id_t          vlan_id,
                                         _In_ uint32_t               port_count,
                                         _In_ const sai_vlan_port_t* port_list){
  int i, j;
  struct __vlan* v = get_vlan(vlan_id);

  if(v == NULL){
    fprintf(stderr, "Error: the given vlan id (%d) does not exist.\n", vlan_id);
    return SAI_STATUS_INVALID_VLAN_ID;
  }

  for(i=0;i<port_count;i++){
    remove_a_port_from_vlan(v, port_list[i]);
  }

  // data plane
  // not yet implemented

  return SAI_STATUS_SUCCESS;
}
