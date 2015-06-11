#if !defined(_NETLINK_H)

#define _NETLINK_H

#include <linux/netlink.h> 

void add_netlink_msg(struct nlmsghdr*, int, const void*, int);
int create_bridge(const char*);
int join_bridge(const char*, int);
int set_ip_address(unsigned int, int, const char*);
int dev_up(const char*);
int get_netdev_names(char***);

#endif
