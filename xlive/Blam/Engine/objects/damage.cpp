#include "stdafx.h"
#include "damage.h"

#include "game/game.h"

/* public code */

void damage_apply_patches(void)
{
	PatchCall(Memory::GetAddress(0x147DB8, 0x172D55), object_cause_damage);
	return;
}

void __cdecl object_cause_damage(s_damage_data* damage_data, datum object_index, int16 node_index, int16 region_index, int16 material_index, real_vector3d* object_normal)
{
	// Hook on call to prevent guardian glitching
	// Used to disable it in Infection only, became a bigger problem so now we have to disable it globally.....
	if (game_is_multiplayer() && (damage_data->owner_player_index == NONE || damage_data->owner_team_index == _game_team_none))
	{
		LOG_TRACE_GAME("GUARDIAN GLITCH PREVENTED");
	}
	else
	{
		INVOKE(0x17AD81, 0x1525E1, object_cause_damage, damage_data, object_index, node_index, region_index, material_index, object_normal);
	}

	return;
}
