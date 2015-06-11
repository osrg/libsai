#if !defined(_C_SDK_H)
#define _C_SDK_H

#include <stdlib.h>

#include "sai.h"
#include "saistatus.h"
#include "saiswitch.h"
#include "saitypes.h"
#include "sairouter.h"
#include "saivlan.h"

typedef struct {
  int id;
  int speed;
  sai_object_id_t interface_id; // preliminary: a port and an interface are mapped 1 by 1
} Port;

typedef struct {
  Port* ports;
  int tag;
  int internal_id;
  sai_object_id_t next_hop_id;
} Vlan;

typedef struct {
  sai_ip4_t ip;
  sai_ip4_t netmask;
  Vlan* next_hop;
  sai_object_id_t internal_id;
} Route;

typedef struct {
  int number_of_vlans;
  Vlan* vlans;
  Route* routes;
  sai_object_id_t internal_id;
} Router;

typedef struct {
  int number_of_ports;
  Router* router;
  Port* ports;
} Switch;

sai_ip4_t make_ip4(const char*);
Switch* switch_init();
Vlan* create_vlan(int, ...);
Router* create_router();
int add_vlan_to_router(Router*, Vlan*, sai_ip4_t);

#endif
