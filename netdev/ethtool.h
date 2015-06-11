#if !defined(_ETHTOOL_H)

#define _ETHTOOL_H

int get_port_speed(const char*);
char* get_driver_info(const char*);
int set_mac_address(unsigned char[6], const char*);

#endif
