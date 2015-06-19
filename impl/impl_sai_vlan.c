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
#include "../rocker/rocker_vlan.h"

static int number_of_vlans;
struct __vlan* vlans = NULL;

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
  int i, n = -1;

  for(i=0;i<number_of_vlans;i++){
    if(vlans[i].id == vlan_id){
      n = i;
    }
  }

  if(n == -1){
    fprintf(stderr, "Error: a VLAN with the given vlan id (%d) does not exit.\n", vlan_id);
    return SAI_STATUS_INVALID_VLAN_ID;
  }

  struct __vlan* v = &vlans[n];
  int old_size = v->number_of_ports, new_size = old_size + port_count;
  v->port_list = realloc(v->port_list, new_size * sizeof(sai_vlan_port_t));

  if(v->port_list == NULL){
    fprintf(stderr, "Error: memory allocation for adding vlan ports failed.\n");
    return SAI_STATUS_NO_MEMORY;
  }

  v->number_of_ports = new_size;
  memcpy(v->port_list + old_size, port_list, port_count);

  // data plane
  ret = rocker_add_ports_to_vlan(v, port_count, port_list);

  return ret;
}
