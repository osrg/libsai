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

#include "sai.h"
#include "saistatus.h"
#include "impl_sai_nexthop.h"
#include "impl_sai_vlan.h"

#include <stdio.h>
#include <stdlib.h>

extern struct __vlan* vlans; // defined and initialized in sai_rocker_vlan.c
struct __next_hop* next_hops;
static int _next_hop_id;

sai_status_t impl_create_next_hop(_Out_ sai_object_id_t* next_hop_id,
				 _In_ uint32_t attr_count,
				 _In_ const sai_attribute_t *attr_list){
  int i;
  *next_hop_id = _next_hop_id;
  sai_ip4_t interface_ip;

  // create a new nexthop
  next_hops = realloc(next_hops, _next_hop_id * sizeof(struct __next_hop));
  if(next_hops == NULL){
    fprintf(stderr, "Error: allocation memory for a new next_hop failed\n");
    return SAI_STATUS_NO_MEMORY;
  }

  struct __next_hop* n = &next_hops[_next_hop_id];
  n->id = _next_hop_id;
  _next_hop_id++;

  // handle the given attributes
  for(i=0;i<attr_count;i++){
    sai_attribute_t attr = attr_list[i];
    
    switch(attr.id){
    case SAI_NEXT_HOP_ATTR_TYPE:
      if(attr.value.u32 != SAI_NEXT_HOP_IP)
	return SAI_STATUS_NOT_SUPPORTED;
      else
	goto next_attr;
    case SAI_NEXT_HOP_ATTR_IP:
      n->interface_ip = attr.value.ip4;
      goto next_attr;
    case SAI_NEXT_HOP_ATTR_ROUTER_INTERFACE_ID:
      n->interface_id = attr.value.oid;
      goto next_attr;
    }

  next_attr:;
  }

  return SAI_STATUS_SUCCESS;
}

struct __next_hop* get_next_hop(sai_object_id_t id){
  int i;

  for(i=0;i<_next_hop_id;i++){
    if(next_hops[i].id == id)
      return &next_hops[i];
  }

  return NULL;
}
