#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sai_rocker.h"
#include "sai_rocker_vlan.h"
#include "utils.h"

sai_vlan_api_t rocker_vlan_api;

static int number_of_vlans;
struct __vlan* vlans = NULL;

struct __vlan* get_vlan(sai_object_id_t vlan_id){
  int i;

  for(i=0;i<number_of_vlans;i++){
    if(vlans[i].id == vlan_id)
      return &vlans[i];
  }

  return NULL;
}

sai_status_t rocker_create_vlan(_In_ sai_vlan_id_t vlan_id){
  int i, ret;
  char br_name[32];

  sprintf(br_name, "rocker_vlan%u", vlan_id);
  
  for(i=0;i<number_of_vlans;i++){
    if(vlans[i].id == vlan_id){
      fprintf(stderr, "Error: given vlan_id (%d) already exsits.\n", vlan_id);
      return SAI_STATUS_INVALID_VLAN_ID;
    }
  }
  
  number_of_vlans++;
  vlans = realloc(vlans, number_of_vlans * sizeof(struct __vlan));

  if(vlans == NULL){
    fprintf(stderr, "Error: memory allocation for creating a new vlan failed.\n");
    return SAI_STATUS_NO_MEMORY;
  }

  struct __vlan* v = &(vlans[number_of_vlans - 1]);
  v->id = vlan_id;
  v->number_of_ports = 0;
  v->port_list = NULL;

  // A bridge is created for every single VLAN.
  // Rocker automatically assigns the same VLAN id for
  // rocker ports connecetd to the same bridge.
  v->br_name = strdup(br_name);
  ret = create_bridge(br_name);
  if(ret != 0)
    return SAI_STATUS_FAILURE;    
  
  // 0xff is tentative and to be replaced by random values
  sai_mac_t br_mac = {0x52, 0x54, 0, 0xff, 0xff, 0xff};
  for(i=3;i<=5;i++){
    br_mac[i] = get_random_byte();
  }
  memcpy(&v->br_mac, br_mac, 6);
  ret = set_mac_address(v->br_mac, v->br_name);
  if(ret != 0)
    return SAI_STATUS_FAILURE;    

  ret = dev_up(v->br_name);
  if(ret != 0)
    return SAI_STATUS_FAILURE;

  SAI_STATUS_SUCCESS;
}

sai_status_t rocker_add_ports_to_vlan(_In_ sai_vlan_id_t vlan_id, 
				       _In_ uint32_t port_count,
				       _In_ const sai_vlan_port_t* port_list){
  int i, ret, n = -1;

  for(i=0;i<number_of_vlans;i++){
    if(vlans[i].id == vlan_id){
      n = i;
    }
  }

  if(n == -1){
    fprintf(stderr, "Error: a VLAN with the given vlan id (%d) does not exit.\n", vlan_id);
    return SAI_STATUS_INVALID_VLAN_ID;
  }

  struct __vlan* v = &vlans[n];
  int old_size = v->number_of_ports, new_size = old_size + port_count;
  v->port_list = realloc(v->port_list, new_size * sizeof(sai_vlan_port_t));

  if(v->port_list == NULL){
    fprintf(stderr, "Error: memory allocation for adding vlan ports failed.\n");
    return SAI_STATUS_NO_MEMORY;
  }

  v->number_of_ports = new_size;
  memcpy(v->port_list + old_size, port_list, port_count);

  // add devices associated to the given ports to the VLAN
  for(i=0;i<port_count;i++){
    struct __port* p = get_port(port_list[i].port_id);
    ret = join_bridge(vlans[n].br_name, p->ifi_index);

    if(ret < 0)
      return SAI_STATUS_FAILURE;
  }
  
  return SAI_STATUS_SUCCESS;
}
