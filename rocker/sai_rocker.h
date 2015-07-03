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

#if !defined(_SAI_ROCKER_H)

#define _SAI_ROCKER_H

#include "saitypes.h"

struct __port{
  int ifi_index; // interface index, assigined inside linux kernel
  char* name;
};

struct __port_list{
  struct __port* this_port;
  struct __port_list* remaining_ports;
};

int get_port_number_plist(const struct __port_list*);
struct __port_list* get_dev_list();
struct __port* get_port(sai_object_id_t);

#endif
