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

struct __port_list* get_dev_list();
struct __port* get_port(sai_object_id_t);

#endif
