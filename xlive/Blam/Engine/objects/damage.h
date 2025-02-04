#pragma once
#include "game/game_allegiance.h"
#include "objects/object_location.h"
#include "tag_files/string_id.h"
#include "tag_files/tag_block.h"

struct s_damage_owner
{
	datum owner_index;
	datum entity_index;
	uint16 target_model_abs_index;
	int16 pad;
};
ASSERT_STRUCT_SIZE(s_damage_owner, 12);

struct s_damage_data
{
	datum definition_index;
	uint32 flags;
	datum owner_player_index;
	datum owner_object_index;
	e_game_team owner_team_index;
	int16 pad;
	s_location location;
	int field_1C;					// 0x1C
	BYTE gap_20[4];					// 0x20
	DWORD field_24;					// 0x24
	DWORD field_28;					// 0x28
	DWORD field_2C;					// 0x2C
	DWORD field_30;					// 0x30
	DWORD field_34;					// 0x34
	DWORD field_38;					// 0x38
	real_point3d origin;
	real_point3d epicenter;
	real_point3d direction;
	BYTE gap_60[24];				// 0x60
	float field_78;					// 0x78
	__int16 field_7C;				// 0x7C
	signed __int16 field_7E;		// 0x7E
	BYTE gap_80[4];					// 0x80
	char field_84;					// 0x84
};
ASSERT_STRUCT_SIZE(s_damage_data, 0x88);

// max_count: LONG_MAX
struct s_armor_modifier_definition
{
	string_id name;
	real32 damage_multiplier;
};
ASSERT_STRUCT_SIZE(s_armor_modifier_definition, 8);

// max_count: LONG_MAX
struct s_damage_group_definition
{
	string_id name;
	tag_block<s_armor_modifier_definition> armor_modifiers;
	
	// Runtime field?
	int32 pad;
};
ASSERT_STRUCT_SIZE(s_damage_group_definition, 16);

/* prototypes */

void damage_apply_patches(void);

void __cdecl object_cause_damage(s_damage_data* damage_data, datum object_index, int16 node_index, int16 region_index, int16 material_index, real_vector3d* object_normal);
