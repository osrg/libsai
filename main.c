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
#include "libsai.h"

main(){
  Switch* sw;
  int i;

  // initialize the switch
  sw = switch_init();

  printf("Number of rocker ports: %d\n", sw->number_of_ports);
  for(i=0;i<sw->number_of_ports;i++)
    printf("Speed of ports[%d]: %d\n", i, sw->ports[i].speed);

  // create two vlans
  Vlan* vlan1 = create_vlan(2, &sw->ports[0], &sw->ports[1]);
  Vlan* vlan2 = create_vlan(2, &sw->ports[2], &sw->ports[3]);

  // create a virtual router
  Router* router = create_router(sw);

  // assign 192.168.1.0/24 to vlan1
  add_vlan_to_router(router, vlan1, make_ip4("192.168.1.1"));
  create_new_route(router, make_ip4("192.168.1.0"), make_ip4("255.255.255.0"), vlan1);

  // assign 192.168.2.0/24 to vlan2
  add_vlan_to_router(router, vlan2, make_ip4("192.168.2.1"));
  create_new_route(router, make_ip4("192.168.2.0"), make_ip4("255.255.255.0"), vlan2);
}

