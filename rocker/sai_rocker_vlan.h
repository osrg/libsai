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

#if !defined(_SAI_ROCKER_VLAN_H)

#define _SAI_ROCKER_VLAN_H

#include "sai.h"
#include "saitypes.h"
#include "saivlan.h"

struct __vlan{
  sai_vlan_id_t id;
  int number_of_ports;
  sai_vlan_port_t* port_list;
  char* br_name;
  sai_mac_t br_mac;
};

sai_status_t rocker_create_vlan(_In_ sai_vlan_id_t);
sai_status_t rocker_add_ports_to_vlan(_In_ sai_vlan_id_t, _In_ uint32_t, _In_ const sai_vlan_port_t*);
struct __vlan* get_vlan(sai_object_id_t);

#endif
