#if !defined(_SAI_ROCKER_VLAN_H)

#define _SAI_ROCKER_VLAN_H

#include "sai.h"
#include "saitypes.h"
#include "saivlan.h"

struct __vlan{
  sai_vlan_id_t id;
  int number_of_ports;
  sai_vlan_port_t* port_list;
  char* br_name;
  sai_mac_t br_mac;
};

sai_status_t rocker_create_vlan(_In_ sai_vlan_id_t);
sai_status_t rocker_add_ports_to_vlan(_In_ sai_vlan_id_t, _In_ uint32_t, _In_ const sai_vlan_port_t*);
struct __vlan* get_vlan(sai_object_id_t);

#endif
