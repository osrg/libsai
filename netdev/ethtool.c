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
