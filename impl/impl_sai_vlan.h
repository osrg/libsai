#if !defined(_IMPL_SAI_VLAN_H)

#define _IMPL_SAI_VLAN_H

#include "saitypes.h"
#include "saistatus.h"
#include "saivlan.h"

#define VLAN_ID_NOT_ASSIGNED (9999)

struct __vlan{
  sai_vlan_id_t id;
  int number_of_ports;
  sai_vlan_port_t* port_list;

  // data plane specific members should be capseralized here 
  void* data_plane_attributes;
};

struct __vlan* get_vlan(sai_object_id_t vlan_id);
sai_status_t impl_create_vlan(_In_ sai_vlan_id_t);
sai_status_t impl_remove_vlan(_In_ sai_vlan_id_t vlant_id);
sai_status_t impl_add_ports_to_vlan(_In_ sai_vlan_id_t vlan_id, 
				    _In_ uint32_t port_count,
				    _In_ const sai_vlan_port_t* port_list);
sai_status_t impl_remove_ports_from_vlan(_In_ sai_vlan_id_t          vlan_id,
                                         _In_ uint32_t               port_count,
                                         _In_ const sai_vlan_port_t* port_list);

#endif
