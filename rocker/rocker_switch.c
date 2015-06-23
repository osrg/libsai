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

#include <stdio.h> // NULL
#include "sai_rocker.h"

extern struct __port_list* global_plist;

int rocker_get_port_number(){
  static int first_call = 1;
  static int n_ports_cached;
  
  if(first_call){
    n_ports_cached = get_port_number(global_plist);
    first_call = 0;
  }

  return n_ports_cached;
}

int set_ports_up(struct __port_list* plist){
  int i;
  int n_ports = get_port_number(plist);

  for(i=0;i<n_ports;i++){
    struct __port* p = get_port(i);
    dev_up(p->name);
  }
}
