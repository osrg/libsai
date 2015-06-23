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

// Normal headers
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// SAI headers
#include "sai.h"
#include "saitypes.h"
#include "saiswitch.h"
#include "saistatus.h"
#include "sainexthop.h"
#include "sairoute.h"

// sai_rocker headers
#include "sai_rocker.h"
#include "sai_rocker_port.h"
#include "../impl/impl_sai_vlan.h"
#include "../impl/impl_sai_switch.h"
#include "../impl/impl_sai_route.h"
#include "sai_rocker_routerintf.h"
#include "../impl/impl_sai_nexthop.h"
#include "sai_rocker_router.h"
#include "../netdev/netlink.h"
#include "../netdev/ethtool.h"

struct __port_list* global_plist;

extern sai_switch_api_t rocker_sai_switch_api;
extern sai_port_api_t rocker_sai_port_api;
sai_virtual_router_api_t rocker_virtual_router_api;
sai_router_interface_api_t rocker_router_interface_api;
sai_next_hop_api_t rocker_next_hop_api;
sai_route_api_t rocker_route_api;
extern sai_vlan_api_t rocker_vlan_api;

sai_status_t sai_api_query(_In_ sai_api_t sai_api_id, _Out_ void** api_method_table){
  switch(sai_api_id){
  case SAI_API_SWITCH:
    *api_method_table = &rocker_sai_switch_api;
    return SAI_STATUS_SUCCESS;
  case SAI_API_PORT:
    *api_method_table = &rocker_sai_port_api;
    return SAI_STATUS_SUCCESS;
  case SAI_API_VIRTUAL_ROUTER:
    *api_method_table = &rocker_virtual_router_api;
    return SAI_STATUS_SUCCESS;
  case SAI_API_ROUTER_INTERFACE:
    *api_method_table = &rocker_router_interface_api;
    return SAI_STATUS_SUCCESS;
  case SAI_API_NEXT_HOP:
    *api_method_table = &rocker_next_hop_api;
    return SAI_STATUS_SUCCESS;
  case SAI_API_ROUTE:
    *api_method_table = &rocker_route_api;
    return SAI_STATUS_SUCCESS;
  case SAI_API_VLAN:
    *api_method_table = &rocker_vlan_api;
    return SAI_STATUS_SUCCESS;
  default:
    fprintf(stderr, "Error: value %d of sai_api_t is not supported\n", sai_api_id);
    return SAI_STATUS_NOT_SUPPORTED;
  }
}

struct __port* get_port(sai_object_id_t port_id){
  const struct __port_list* ptr;
  int index = (int)port_id; // port_id is equall to the port index
  
  for(ptr=global_plist;index--;ptr=ptr->remaining_ports){
    ;
  }

  return ptr->this_port;
}

int get_port_number(const struct __port_list* plist){
  int n = 0;
  const struct __port_list* ptr;

  for(ptr=plist;ptr->this_port != NULL;ptr=ptr->remaining_ports){
    n++;
  }

  return n;
}

static int set_mac_address_to_port(sai_mac_t mac, struct __port* port){
  set_mac_address(mac, port->name);
}

struct __port_list* get_dev_list(){
  int n_dev, i;
  char** dev_names;
  struct __port_list *plist, *ptr;

  plist = malloc(sizeof(struct __port_list));
  ptr = plist;
  
  n_dev = get_netdev_names(&dev_names);

  printf("Rocker devices:\n");
  for(i=0;i<n_dev;i++){
    char* driver = get_driver_info(dev_names[i]);

    // pick up rocker eth devices
    if(driver != NULL && !strcmp(driver, "rocker")){
      printf("[%s]\n", dev_names[i]);

      ptr->this_port = malloc(sizeof(struct __port));
      ptr->this_port->name = dev_names[i];
      ptr->this_port->ifi_index = if_nametoindex(dev_names[i]);

      ptr->remaining_ports = malloc(sizeof(struct __port_list));
      ptr = ptr->remaining_ports;
    }
  }

  ptr->this_port = NULL;

  return plist;
}

__attribute__((constructor)) __rocker_init(){
  // port APIs
  rocker_sai_port_api.get_port_attribute = rocker_get_port_attribute;

  // switch APIs
  rocker_sai_switch_api.initialize_switch = impl_initialize_switch;
  rocker_sai_switch_api.get_switch_attribute = impl_get_switch_attribute;

  // virtual router APIs
  rocker_virtual_router_api.create_virtual_router = rocker_create_virtual_router;

  // router interface APIs
  rocker_router_interface_api.create_router_interface = rocker_create_router_interface;

  // next hop APIs
  rocker_next_hop_api.create_next_hop = impl_create_next_hop;

  // route APIs
  rocker_route_api.create_route = impl_create_route;

  // vlan APIs
  rocker_vlan_api.create_vlan = impl_create_vlan;
  rocker_vlan_api.remove_vlan = impl_remove_vlan;
  rocker_vlan_api.add_ports_to_vlan = impl_add_ports_to_vlan;
  rocker_vlan_api.add_ports_to_vlan = impl_remove_ports_from_vlan;
}
