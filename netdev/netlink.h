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

#if !defined(_NETLINK_H)

#define _NETLINK_H

#include <linux/netlink.h> 

void add_netlink_msg(struct nlmsghdr*, int, const void*, int);
int create_bridge(const char*);
int join_bridge(const char*, int);
int set_ip_address(unsigned int, int, const char*);
int dev_up(const char*);
int get_netdev_names(char***);

struct fib{
  unsigned char addr[6];
  char* destination;
};

#endif
