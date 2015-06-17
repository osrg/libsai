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

/*
  Note:
   If this file includes any rocker-specific
   function calls/variable references, that is a bug.
 */
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "libsai.h"

static sai_switch_api_t* switch_api;
static sai_port_api_t* port_api;
static sai_virtual_router_api_t* virtual_router_api;
static sai_router_interface_api_t* router_interface_api;
static sai_route_api_t* route_api;
static sai_vlan_api_t* vlan_api;
static sai_next_hop_api_t* next_hop_api;

// example: make_ip4("192.168.0.1")
sai_ip4_t make_ip4(const char* ipstr){
  int n = 0;
  const char* ptr = ipstr;
  sai_ip4_t ret = 0; // typedef uint32_t sai_ip4_t (sai.h)

  while(1){
      // n th byte of the IP address
      n++;

      // add this byte to ret
      ret = (ret << 8) + atoi(ptr);

      // find next byte
      ptr = strchr(ptr, '.');
      if(ptr == NULL)
	break;
      else
	ptr++; // point the next chacter of '.'
  }

  if(n > 4){
    fprintf(stderr, "Error: given ip4 address (%s) is longer than 4 bytes\n", ipstr);
    return 0;
  }
  else if (n < 4){
    fprintf(stderr, "Error: given ip4 address (%s) is shorter than 4 bytes\n", ipstr);
    return 0;
  }

  return ret;
}

static uint32_t get_port_attribute_u32_single(sai_attr_id_t id, sai_object_id_t port_id){
  sai_attribute_t port_attr_get;
  port_attr_get.id = id;
  port_api->get_port_attribute(port_id, 1, &port_attr_get);
  return port_attr_get.value.u32;
}

static uint32_t get_switch_attribute_u32_single(sai_attr_id_t id){
  sai_attribute_t switch_attr_get;
  switch_attr_get.id = id;
  switch_api->get_switch_attribute(1, &switch_attr_get);
  return switch_attr_get.value.u32;
}

static int get_port_ids(Switch* sw){
  int i, n = sw->number_of_ports;
  sai_status_t ret;
  sai_attribute_t switch_attr_get;

  switch_attr_get.id = SAI_SWITCH_ATTR_PORT_LIST;
  switch_attr_get.value.objlist.count = n;
  switch_attr_get.value.objlist.list = malloc(sizeof(sai_object_id_t) * n);
  ret = switch_api->get_switch_attribute(1, &switch_attr_get);

  if(ret == SAI_STATUS_BUFFER_OVERFLOW){
    fprintf(stderr, "SAT_STATUS_BUFFER_OVERFLOW: Number of ports is %d while Switch->number_of_ports is %d\n", switch_attr_get.value.objlist.count, n);
    return -1;
  }

  for(i=0;i<n;i++){
    sw->ports[i].id = switch_attr_get.value.objlist.list[i];
  }

  return 0;
}

static int get_port_speed(Switch* sw){
  int i;

  for(i=0;i<sw->number_of_ports;i++){
    sw->ports[i].speed = get_port_attribute_u32_single(SAI_PORT_ATTR_SPEED, sw->ports[i].id);
  }
}

void create_new_route(Router* router, sai_ip4_t destination, sai_ip4_t netmask, Vlan* next_hop){
  Route* route = malloc(sizeof(Route));
  static int _internal_id;
  
  route->ip = destination;
  route->netmask = netmask;
  route->next_hop = next_hop;
  route->internal_id = _internal_id++;

  sai_unicast_route_entry_t unicast_route = {
    .vr_id = router->internal_id,
    // sai_ip_prefix_t
    .destination = {
      .addr_family = SAI_IP_ADDR_FAMILY_IPV4,
      .addr.ip4 = destination,
      .mask.ip4 = netmask
    }
  };
  sai_attribute_t attr = {
    .id = SAI_ROUTE_ATTR_NEXT_HOP_ID,
    .value.oid = next_hop->next_hop_id
  };
  route_api->create_route(&unicast_route, 1, &attr);
}

int add_vlan_to_router(Router* router, Vlan* vlan, sai_ip4_t interface_ip){
  sai_status_t ret;

  fprintf(stderr, "add_vlan_to_router\n");
  
  // create an router interface for this vlan
  sai_attribute_t routerif_attrs[3] = {
    {
      .id = SAI_ROUTER_INTERFACE_ATTR_VIRTUAL_ROUTER_ID,
      .value.oid = router->internal_id
    },
    {
      .id = SAI_ROUTER_INTERFACE_ATTR_TYPE,
      .value.u32 = SAI_ROUTER_INTERFACE_TYPE_VLAN
    },
    {
      .id = SAI_ROUTER_INTERFACE_ATTR_VLAN_ID,
      .value.oid = vlan->internal_id
    }
  };
  sai_object_id_t rif_id;
  ret = router_interface_api->create_router_interface(&rif_id, 3, routerif_attrs);

  if(ret != SAI_STATUS_SUCCESS){
    fprintf(stderr, "router_interface_api->create_router_interface failed with status %u\n", ret);
    return -1;
  }
  
  // attach the created router interface to the router  
  sai_attribute_t next_hop_attrs[3] = {
    {
      .id = SAI_NEXT_HOP_ATTR_TYPE,
      .value.u32 = SAI_NEXT_HOP_IP
    },
    {
      .id = SAI_NEXT_HOP_ATTR_IP,
      .value.ip4 = interface_ip
    },
    {
      .id = SAI_NEXT_HOP_ATTR_ROUTER_INTERFACE_ID,
      .value.oid = rif_id
    }
  };
  sai_object_id_t next_hop_id;
  ret = next_hop_api->create_next_hop(&next_hop_id, 3, next_hop_attrs);

  if(ret != SAI_STATUS_SUCCESS){
    fprintf(stderr, "next_hop_api->create_next_hop failed with status %d\n", ret);
    return -1;
  }

  vlan->next_hop_id = next_hop_id;

  return 0;
}

//Router* create_router(int number_of_vlans, .../* Vlan* v1, Vlan* v2, ... */){
Router* create_router(){
  Router* router = malloc(sizeof(Router));
  int i;
  va_list ap;
  
  // create a virtual router
  virtual_router_api->create_virtual_router(&router->internal_id, 0, NULL);  
  return router;
}

Vlan* create_vlan(int number_of_ports, .../* Port* p1, Port* p2, ... */){
  va_list ap;
  static int _internal_id;
  Vlan* v;
  int i;

  v = malloc(sizeof(Vlan));
  if(v == NULL)
    return NULL;

  // create a vlan with an unique id (assigned automatically)
  v->internal_id = _internal_id++;
  vlan_api->create_vlan(v->internal_id);

  // add given ports to the created vlan
  va_start(ap, number_of_ports);
  for(i=0;i<number_of_ports;i++){
    Port* p = va_arg(ap, Port*);
    sai_vlan_port_t vlan_port = { p->id, SAI_VLAN_PORT_TAGGED };
    vlan_api->add_ports_to_vlan(v->internal_id, 1, &vlan_port);
  }
  va_end(ap);
  
  return v;
}

Switch* switch_init(){
  int i;
  Switch* sw = malloc(sizeof(Switch));
  
  switch_api = malloc(sizeof(sai_switch_api_t));
  port_api = malloc(sizeof(sai_port_api_t));
  virtual_router_api = malloc(sizeof(sai_virtual_router_api_t));
  router_interface_api = malloc(sizeof(sai_router_interface_api_t));
  route_api = malloc(sizeof(sai_route_api_t));
  vlan_api = malloc(sizeof(sai_vlan_api_t));

  sai_api_query(SAI_API_SWITCH, (void**)&switch_api);
  sai_api_query(SAI_API_PORT, (void**)&port_api);
  sai_api_query(SAI_API_VIRTUAL_ROUTER, (void**)&virtual_router_api);
  sai_api_query(SAI_API_ROUTER_INTERFACE, (void**)&router_interface_api);
  sai_api_query(SAI_API_ROUTE, (void**)&route_api);
  sai_api_query(SAI_API_VLAN, (void**)&vlan_api);
  sai_api_query(SAI_API_NEXT_HOP, (void**)&next_hop_api);

  // initialize the switch
  switch_api->initialize_switch(0, NULL, NULL, NULL);

  sw->number_of_ports = get_switch_attribute_u32_single(SAI_SWITCH_ATTR_PORT_NUMBER);
  sw->ports = malloc(sizeof(Port) * sw->number_of_ports);
  get_port_ids(sw);
  get_port_speed(sw);
  
  return sw;
}
