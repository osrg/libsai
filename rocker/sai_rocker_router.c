#include "sai.h"
#include "saistatus.h"
#include "saitypes.h"
#include "sairouter.h"

#include "sai_rocker_router.h"

sai_status_t rocker_create_virtual_router(_Out_ sai_object_id_t* vr_id,
							  _In_ uint32_t attr_count,
							  _In_ const sai_attribute_t *attr_list){
  static int _vr_id;
  
  *vr_id = _vr_id;
  _vr_id++;

  return SAI_STATUS_SUCCESS;
}
