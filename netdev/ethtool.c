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
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/if_arp.h>

#include <linux/ethtool.h>
#include <linux/sockios.h>

#include "ethtool.h"

int ethtool_sock_fd;

__attribute__((constructor)) __ethtool_init(){
  ethtool_sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

  if(ethtool_sock_fd < 0){
    fprintf(stderr, "Error: cannot open a socket for ethtool ioctl. Force abort.\n");
    exit(1);
  }
}

int get_port_speed(const char* devname){
  struct ethtool_value ethval;
  struct ethtool_cmd ethcmd;
  struct ifreq ifr;
  int ret;

  memset(&ifr, 0, sizeof(struct ifreq));
  strncpy(ifr.ifr_name, devname, strlen(devname));

  ethcmd.cmd = ETHTOOL_GSET;
  ifr.ifr_data = (caddr_t)&ethcmd;
  ret = ioctl(ethtool_sock_fd, SIOCETHTOOL, &ifr);

  if(ret == -1){
    perror("ioctl(ethtool_sock_fd, SIOCETHTOOL, &ifr)");
    return;
  }

  return ethtool_cmd_speed(&ethcmd);
}

char* get_driver_info(const char* devname){
  struct ethtool_drvinfo drvinfo;
  struct ifreq ifr;
  int ret;

  memset(&ifr, 0, sizeof(struct ifreq));
  strncpy(ifr.ifr_name, devname, strlen(devname));

  drvinfo.cmd = ETHTOOL_GDRVINFO;
  ifr.ifr_data = (caddr_t)&drvinfo;
  ret = ioctl(ethtool_sock_fd, SIOCETHTOOL, &ifr);

  if(ret == -1){
    perror("ioctl(ethtool_sock_fd, SIOCETHTOOL, &ifr)");
    return NULL;
  }

  strdup(drvinfo.driver);
}

/*
  The ioctl called in this function is not an ETHTOOL one,
  but let me just put this here as it can reuse the same socket...
*/
int set_mac_address(unsigned char mac[6], const char* devname){
  struct ifreq ifr;
  int ret;
  
  strncpy(ifr.ifr_name, devname, IFNAMSIZ);
  ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
  memcpy(ifr.ifr_hwaddr.sa_data, mac, 6);
  
  ret = ioctl(ethtool_sock_fd, SIOCSIFHWADDR, &ifr);
  if(ret != 0){
    perror("ioctl(ethtool_sock_fd, SIOCSIFHWADDR, &ifr)");
    return -1;
  }
  
  return 0;
}
