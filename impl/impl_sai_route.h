#if !defined(_IMPL_SAI_ROUTE_H)

#define _IMPL_SAI_ROUTE_H

sai_status_t impl_create_route(_In_ const sai_unicast_route_entry_t* unicast_route_entry,
			       _In_ uint32_t attr_count,
			       _In_ const sai_attribute_t *attr_list
			       );

#endif


  
