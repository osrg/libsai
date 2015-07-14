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
#include "saitypes.h"
#include "sairoute.h"

#include "impl_sai_route.h"
#include "impl_sai_nexthop.h"
#include "../dataplane/rocker/rocker_vlan.h"

#include <stdio.h>

sai_status_t impl_create_route(_In_ const sai_unicast_route_entry_t* unicast_route_entry,
				  _In_ uint32_t attr_count,
				  _In_ const sai_attribute_t *attr_list
				  ){
  struct __next_hop* next_hop;
  int i;
  
  /*
    Should check if the given virtual router (vr_id) is actually connected
    to the given next hop (SAI_ROUTE_ATTR_NEXT_HOP_ID), but let's just
    trust the caller in this preliminary version.
  */
  for(i=0;i<attr_count;i++){
    switch(attr_list[i].id){
    case SAI_ROUTE_ATTR_NEXT_HOP_ID:
      next_hop = get_next_hop(attr_list[i].value.oid);
      goto next_attr;
    default:
      return SAI_STATUS_NOT_SUPPORTED;
    }

  next_attr:;
  }

  // data plane
  set_route(unicast_route_entry, next_hop);
  
  return SAI_STATUS_SUCCESS;
}
