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

#include "sairoute.h"

#include "rocker_vlan.h"
#include "../../impl/impl_sai_nexthop.h"

int set_route(const sai_unicast_route_entry_t* route_entry, struct __next_hop* next_hop){
  int netmask_bits, ret;
  sai_ip4_t destination = route_entry->destination.addr.ip4;
  struct __rocker_vlan* target_interface;

  target_interface = (struct __rocker_vlan*)(get_vlan(next_hop->interface_id)->data_plane_attributes);

  // Set route for this subnet, to the next hop
  // The below functions does the exactly the same as
  //    ip addr add destination/netmask_bits dev next_hop->br_name
  // which introduces an appropriate ip route automatically
  // (What if the routing is comlex like connecting two virtual routers??)
  netmask_bits = make_netmask_bits(route_entry->destination.mask.ip4);
  ret = set_ip_address(next_hop->interface_ip, netmask_bits, target_interface->br_name);

  return ret;
}
