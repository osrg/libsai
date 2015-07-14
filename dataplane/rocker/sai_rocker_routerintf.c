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

#include "saistatus.h"
#include "sai_rocker_routerintf.h"

struct __router_interface* routerintfs;

sai_status_t rocker_create_router_interface(_Out_ sai_object_id_t* rif_id,
					    _In_ uint32_t attr_count,
					    _In_ sai_attribute_t *attr_list
					    ){
  static int _rif_id;
  int i;
  
  *rif_id = _rif_id;
  _rif_id++;

  routerintfs = realloc(routerintfs, _rif_id * sizeof(struct __router_interface));
  if(routerintfs == NULL){
    fprintf(stderr, "Error: allocating memory for a new route interface failed.\n");
    return SAI_STATUS_NO_MEMORY;
  }

  for(i=0;i<attr_count;i++){
    sai_attribute_t attr = attr_list[i];

    switch(attr.id){
    case SAI_ROUTER_INTERFACE_ATTR_VIRTUAL_ROUTER_ID:
      goto next_attr;
    case SAI_ROUTER_INTERFACE_ATTR_TYPE:
      if(attr.value.u32 == SAI_ROUTER_INTERFACE_TYPE_VLAN)
	goto next_attr;
      else if(attr.value.u32 == SAI_ROUTER_INTERFACE_TYPE_PORT)
	return SAI_STATUS_NOT_SUPPORTED;
    case SAI_ROUTER_INTERFACE_ATTR_VLAN_ID:
      routerintfs[_rif_id].vlan_id = attr.value.oid;
      goto next_attr;
    default:
      return SAI_STATUS_NOT_SUPPORTED;
    }

  next_attr:;
  }

  return SAI_STATUS_SUCCESS;
}
