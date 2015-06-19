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

#include "saistatus.h"
#include "saitypes.h"
#include "saivlan.h"

#include "../impl/impl_sai_vlan.h"
#include "sai_rocker.h"
#include "rocker_vlan.h"

sai_status_t rocker_create_vlan(struct __vlan* v){
  int i, ret;
  char br_name[32];

  sprintf(br_name, "rocker_vlan%u", v->id);

  struct __rocker_vlan* rocker_vlan = malloc(sizeof(struct __rocker_vlan));
  
  // A bridge is created for every single VLAN.
  // Rocker automatically assigns the same VLAN id for
  // rocker ports connecetd to the same bridge.
  rocker_vlan->br_name = strdup(br_name);
  ret = create_bridge(br_name);
  if(ret != 0)
    return SAI_STATUS_FAILURE;    
  
  // 0xff is tentative and to be replaced by random values
  sai_mac_t br_mac = {0x52, 0x54, 0, 0xff, 0xff, 0xff};
  for(i=3;i<=5;i++){
    br_mac[i] = get_random_byte();
  }
  memcpy(&rocker_vlan->br_mac, br_mac, 6);
  ret = set_mac_address(rocker_vlan->br_mac, rocker_vlan->br_name);
  if(ret != 0)
    return SAI_STATUS_FAILURE;    

  ret = dev_up(rocker_vlan->br_name);
  if(ret != 0)
    return SAI_STATUS_FAILURE;

  v->data_plane_attributes = (void*)rocker_vlan;
  
  SAI_STATUS_SUCCESS;
}

sai_status_t rocker_add_ports_to_vlan(const struct __vlan* v, int port_count, const sai_vlan_port_t* port_list){
  int i, ret;
  
  for(i=0;i<port_count;i++){
    struct __port* p = get_port(port_list[i].port_id);
    struct __rocker_vlan* rocker_vlan = (struct __rocker_vlan*)v->data_plane_attributes;
    ret = join_bridge(rocker_vlan->br_name, p->ifi_index);
    
    if(ret < 0)
      return SAI_STATUS_FAILURE;
  }
  
  return SAI_STATUS_SUCCESS;
}
