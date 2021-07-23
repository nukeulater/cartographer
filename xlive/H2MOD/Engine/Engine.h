#pragma once
#include "Blam/Cache/DataTypes/DatumIndex.h"
#include "Blam/Engine/Objects/ObjectPlacementData.h"

namespace Engine
{
	game_life_cycle get_game_life_cycle();
	bool __cdecl IsGameMinimized();

	namespace Unit
	{
		int __cdecl remove_equipment(datum unit_datum_index);
		signed int __cdecl inventory_next_weapon(unsigned short unit);
		bool __cdecl assign_equipment_to_unit(datum unit, int object_index, short unk);
	}

	namespace Objects
	{
		char* __cdecl try_and_get_data_with_type(datum object_datum_index, int object_type_flags);
		void __cdecl create_new_placement_data(s_object_placement_data* placement, datum object_definition_index, datum object_owner, int unk);
		int __cdecl call_object_new(s_object_placement_data* placement);
	}
}