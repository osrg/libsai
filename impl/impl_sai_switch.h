// Copyright (C) 2015 Nippon Telegraph and Telephone Corporation.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
// implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#if !defined(_IMPL_SAI_SWITCH_H)

#define _IMPL_SAI_SWITCH_H

#include "sai.h"
#include "saitypes.h"
#include "saistatus.h"

sai_status_t impl_get_switch_attribute(_In_ uint32_t attr_count, _Inout_ sai_attribute_t *attr_list);
sai_status_t impl_initialize_switch(_In_ sai_switch_profile_id_t profile_id,
				    _In_reads_z_(SAI_MAX_HARDWARE_ID_LEN) char* switch_hardware_id,
				    _In_reads_opt_z_(SAI_MAX_FIRMWARE_PATH_NAME_LEN) char* firmware_path_name,
				    _In_ sai_switch_notification_t* switch_notifications
				    );


#endif
