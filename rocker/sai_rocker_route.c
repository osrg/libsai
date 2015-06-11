#include "sai.h"
#include "saistatus.h"
#include "saitypes.h"
#include "sairoute.h"

#include "sai_rocker_route.h"
#include "sai_rocker_vlan.h"
#include "sai_rocker_nexthop.h"

#include <stdio.h>

sai_status_t rocker_create_route(_In_ const sai_unicast_route_entry_t* unicast_route_entry,
				  _In_ uint32_t attr_count,
				  _In_ const sai_attribute_t *attr_list
				  ){
  sai_ip4_t destination = unicast_route_entry->destination.addr.ip4;
  int netmask_bits = make_netmask_bits(unicast_route_entry->destination.mask.ip4);
  struct __next_hop* next_hop;
  int i;
  sai_object_id_t id;
  
  /*
    Should check if the given virtual router (vr_id) is actually connected
    to the given next hop (SAI_ROUTE_ATTR_NEXT_HOP_ID), but let's just
    trust the caller in this preliminary version.
  */

  for(i=0;i<attr_count;i++){
    switch(attr_list[i].id){
    case SAI_ROUTE_ATTR_NEXT_HOP_ID:
      id = attr_list[i].value.oid;
      next_hop = get_next_hop(attr_list[i].value.oid);
      goto next_attr;
    default:
      return SAI_STATUS_NOT_SUPPORTED;
    }

  next_attr:;
  }

  fprintf(stderr, "netmask_bits: %d\n", netmask_bits);
  
  // Set route for this subnet, to the next hop
  // The below functions does the exactly the same as
  //    ip addr add destination/netmask_bits dev next_hop->br_name
  // which introduces an appropriate ip route automatically
  // (What if the routing is comlex like connecting two virtual routers??)
  set_ip_address(next_hop->interface_ip, netmask_bits, next_hop->br_name);
  
  return SAI_STATUS_SUCCESS;
}
