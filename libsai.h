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

#if !defined(_C_SDK_H)
#define _C_SDK_H

#include <stdlib.h>

#include "sai.h"
#include "saistatus.h"
#include "saiswitch.h"
#include "saitypes.h"
#include "sairouter.h"
#include "saivlan.h"

typedef struct {
  int id;
  int speed;
  sai_object_id_t interface_id; // preliminary: a port and an interface are mapped 1 by 1
} Port;

typedef struct {
  Port* ports;
  int tag;
  int internal_id;
  sai_object_id_t next_hop_id;
} Vlan;

typedef struct {
  sai_ip4_t ip;
  sai_ip4_t netmask;
  Vlan* next_hop;
  sai_object_id_t internal_id;
} Route;

typedef struct {
  int number_of_vlans;
  Vlan* vlans;
  Route* routes;
  sai_object_id_t internal_id;
} Router;

typedef struct {
  int number_of_ports;
  Router* router;
  Port* ports;
} Switch;

sai_ip4_t make_ip4(const char*);
Switch* switch_init();
Vlan* create_vlan(int, ...);
Router* create_router();
int add_vlan_to_router(Router*, Vlan*, sai_ip4_t);

#endif
