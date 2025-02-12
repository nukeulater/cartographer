#pragma once
#include "scenario_definitions.h"

/* public code */

scenario* global_scenario_get(void);

void set_global_scenario(scenario* _scenario);

collision_bsp* global_collision_bsp_get(void);

void scenario_apply_patches(void);

uint32 scenario_netgame_equipment_size(void);

void location_invalidate(s_location* object_location);

void __cdecl scenario_location_from_point(s_location* location, real_point3d* point);

bool __cdecl scenario_location_underwater(s_location* location, real_point3d* point, int16* global_material_index);

void __cdecl scenario_location_from_leaf(s_location* location, int32 leaf_index);
