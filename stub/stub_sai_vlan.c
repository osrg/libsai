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

#include "../impl/impl_sai_vlan.h"

sai_vlan_api_t rocker_vlan_api;

sai_status_t stub_create_vlan(_In_ sai_vlan_id_t vlan_id){
  return impl_create_vlan(vlan_id);
}

sai_status_t stub_add_ports_to_vlan(_In_ sai_vlan_id_t vlan_id, 
				    _In_ uint32_t port_count,
				    _In_ const sai_vlan_port_t* port_list){
  return impl_add_ports_to_vlan(vlan_id, port_count, port_list);
}
