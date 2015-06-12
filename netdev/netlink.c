#include <stdio.h>

#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <net/if.h>
#include <net/if_arp.h>
#include <linux/netlink.h> 
#include <linux/rtnetlink.h>

int rtnetlink_sock_fd;

__attribute__((constructor)) __netlink_init(){
  rtnetlink_sock_fd = socket(AF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE);

  if(rtnetlink_sock_fd < 0){
    fprintf(stderr, "Error: cannot open a socket for netlink messaging. Force abort.");
    exit(1);
  }
}

// To understand this function, see the link below:
// http://linuxjm.osdn.jp/html/LDP_man-pages/man3/rtnetlink.3.html
void add_netlink_msg(struct nlmsghdr* nh, int type, const void* data, int raw_data_length){
  struct rtattr *rta;
  int rta_length = RTA_LENGTH(raw_data_length);
  
  rta = (struct rtattr*)((char*)nh + NLMSG_ALIGN(nh->nlmsg_len));

  rta->rta_type = type;
  rta->rta_len = rta_length;
  nh->nlmsg_len =  NLMSG_ALIGN(nh->nlmsg_len) + RTA_ALIGN(rta_length);

  memcpy(RTA_DATA(rta), data, raw_data_length);
}

int create_bridge(const char* name){
  struct {
    struct nlmsghdr n;
    struct ifinfomsg i;
    char _[1024];
    /* `_' exists to let the complier allocate a sequential memory right after n and i,
       and it is never accessed directly via its name */
  } r;
  int size = sizeof(r);
  const char* dev_type = "bridge";
  int ret;

  memset(&r, 0, size);

  r.n.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifinfomsg));
  r.n.nlmsg_flags = NLM_F_REQUEST | NLM_F_CREATE | NLM_F_EXCL;
  r.n.nlmsg_type = RTM_NEWLINK;
  r.i.ifi_family = AF_PACKET;

  r.i.ifi_index = 0;

  add_netlink_msg(&r.n, IFLA_IFNAME, name, strlen(name)+1);

  struct rtattr *linkinfo  = (struct rtattr*)((char*)&r.n + NLMSG_ALIGN(r.n.nlmsg_len));
  add_netlink_msg(&r.n, IFLA_LINKINFO, NULL, 0);
  add_netlink_msg(&r.n, IFLA_INFO_KIND, dev_type, strlen(dev_type)+1);

  linkinfo->rta_len = (int)((char*)&r.n + NLMSG_ALIGN(r.n.nlmsg_len) - (char*)linkinfo);

  send(rtnetlink_sock_fd, &r, r.n.nlmsg_len, 0);

  return 0;
}

int join_bridge(const char* brname, int port_ifiindex){
  struct {
    struct nlmsghdr n;
    struct ifinfomsg i;
    char _[1024];
    /* `_' exists to let the complier allocate a sequential memory right after n and i,
       and it is never accessed directly via its name */
  } r;
  const char* dev_type = "bridge";
  int ret;
  int br_ifiindex = if_nametoindex(brname);

  memset(&r, 0, sizeof(r));

  r.n.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifinfomsg));
  r.n.nlmsg_flags = NLM_F_REQUEST;
  r.n.nlmsg_type = RTM_NEWLINK;
  r.i.ifi_family = AF_PACKET;
  r.i.ifi_index = port_ifiindex;

  add_netlink_msg(&r.n, IFLA_MASTER, &br_ifiindex, 4);

  send(rtnetlink_sock_fd, &r, r.n.nlmsg_len, 0);

  return 0;
}

int set_ip_address(unsigned int ip, int netmask, const char* devname){
  struct {
    struct nlmsghdr n;
    struct ifaddrmsg ifa;
    char _[256];
    /* `_' exists to let the complier allocate a sequential memory right after n and i,
       and it is never accessed directly via its name */
  } r;
  int size = sizeof(r);
  int ret, b;
  unsigned int broadcast = ip;

  memset(&r, 0, sizeof(r));

  for(b = (32-netmask-1);b >= 0;b--){
    broadcast |= (1<<b);
  }

  ip = htonl(ip);
  broadcast = htonl(broadcast);
  
  r.n.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifaddrmsg));
  r.n.nlmsg_flags = NLM_F_REQUEST | NLM_F_CREATE | NLM_F_EXCL;
  r.n.nlmsg_type = RTM_NEWADDR;
  r.ifa.ifa_family = AF_INET;
  r.ifa.ifa_prefixlen = netmask;
  r.ifa.ifa_flags = 0;
  r.ifa.ifa_index = if_nametoindex(devname);
  
  add_netlink_msg(&r.n, IFA_LOCAL, &ip, 4);
  add_netlink_msg(&r.n, IFA_ADDRESS, &ip, 4);
  add_netlink_msg(&r.n, IFA_BROADCAST, &broadcast, 8);

  send(rtnetlink_sock_fd, &r, r.n.nlmsg_len, 0);

  return 0;
}

int dev_up(const char* devname){
  struct {
    struct nlmsghdr n;
    struct ifinfomsg i;
    char _[1024];
    /* `_' exists to let the complier allocate a sequential memory right after n and i,
       and it is never accessed directly via its name */
  } r;
  int port_ifiindex = if_nametoindex(devname);

  memset(&r, 0, sizeof(r));

  r.n.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifinfomsg));
  r.n.nlmsg_flags = NLM_F_REQUEST;
  r.n.nlmsg_type = RTM_NEWLINK;
  r.i.ifi_change = IFF_UP;
  r.i.ifi_flags = IFF_UP;
  r.i.ifi_family = AF_PACKET;
  r.i.ifi_index = port_ifiindex;

  send(rtnetlink_sock_fd, &r, r.n.nlmsg_len, 0);

  return 0;
}

/*
  results: a pointer to a list of device names
           ~~~~~~~~~    ~~~~~~    ~~~~~~~~~~~
	   each "~~~" should be a pointer thus the type of `results' is char***
*/
int get_netdev_names(char*** results){
  static int seq;
  char recv_buff[1024*1024];
  int len;
  int n_devices = 0;
  char** device_names = NULL;
  
  struct {
    struct nlmsghdr nh;
    struct ifinfomsg ifm;
  } r;

  r.nh.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifinfomsg));
  r.nh.nlmsg_type = RTM_GETLINK;
  r.nh.nlmsg_flags = NLM_F_ROOT | NLM_F_REQUEST;
  r.nh.nlmsg_pid = 0;
  r.nh.nlmsg_seq = seq++;
  r.ifm.ifi_family = AF_PACKET;

  // send request to dump the root table
  send(rtnetlink_sock_fd, &r, r.nh.nlmsg_len, 0);

  while(1){
    // receive the reply
    len = read(rtnetlink_sock_fd, recv_buff, sizeof(recv_buff));

    // parse the reply
    struct nlmsghdr* nh = (struct nlmsghdr*)recv_buff;

    while(NLMSG_OK(nh, len)){
      if(nh->nlmsg_type == NLMSG_DONE){
	goto end_of_netlink_messages;
      } 

      n_devices++;
      device_names = realloc(device_names, sizeof(char*) * n_devices);
      
      struct rtattr* rta = IFLA_RTA(NLMSG_DATA(nh));

      int l = nh->nlmsg_len;
      while(RTA_OK(rta, l)){
	if(rta->rta_type == IFLA_IFNAME){
	  device_names[n_devices - 1] = strdup((char*)RTA_DATA(rta));
	}

	rta = RTA_NEXT(rta,l);
      }

      nh = NLMSG_NEXT(nh, len);
    }
  }

 end_of_netlink_messages:;

  *results = device_names;

  return n_devices;
}
