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

#if !defined (_SAI_ROCKER_ROUTE_H)

#define _SAI_ROCKER_ROUTE_H

#include "saistatus.h"

sai_status_t rocker_create_route(_In_ const sai_unicast_route_entry_t*, _In_ uint32_t, _In_ const sai_attribute_t*);

#endif
