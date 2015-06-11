#if !defined(_SAI_ROCKER_NEXTHOP_H)

#define _SAI_ROCKER_NEXTHOP_H

sai_status_t rocker_create_next_hop(_Out_ sai_object_id_t*, _In_ uint32_t, _In_ const sai_attribute_t*);

struct __next_hop{
  sai_object_id_t id;
  sai_ip4_t interface_ip;
  char* br_name;
};

struct __next_hop* get_next_hop(sai_object_id_t);

#endif
