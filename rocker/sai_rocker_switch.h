#if !defined(_SAI_ROCKER_SWITCH_H)

#define _SAI_ROCKER_SWITCH_H

#include "sai.h"
#include "saiswitch.h"

sai_status_t rocker_get_switch_attribute(_In_ uint32_t, _Inout_ sai_attribute_t*);
sai_status_t rocker_initialize_switch(_In_ sai_switch_profile_id_t,
				      _In_reads_z_(SAI_MAX_HARDWARE_ID_LEN) char*,
				      _In_reads_opt_z_(SAI_MAX_FIRMWARE_PATH_NAME_LEN) char*,
				      _In_ sai_switch_notification_t*);


#endif
