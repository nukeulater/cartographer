﻿#pragma once

#include "Blam\Maths\Maths.h"
#include "Blam\Cache\DataTypes.h"

enum e_object_team : BYTE
{
	// MP
	Red = 0,
	Blue = 1,
	Yellow = 2,
	Green = 3,
	Purple = 4,
	Orange = 5,
	Bronwn = 6,
	Pink = 7,
	// SP
	Default = 0,
	player = 1,
	Human = 2,
	Covenant = 3,
	Flood = 4,
	Sentinel = 5,
	Heretic = 6,
	Prophet = 7,

	// unasigned team ids
	Unused8 = 8,
	Unused9 = 9,
	Unused10 = 10,
	Unused11 = 11,
	Unused12 = 12,
	Unused13 = 13,
	Unused14 = 14,
	Unused15 = 15,

	// Shared
	None = 255
};

enum e_object_type : signed char
{
	biped = 0,
	vehicle,
	weapon,
	equipment,
	garbage,
	projectile,
	scenery,
	machine,
	control,
	light_fixture,
	sound_scenery,
	crate,
	creature,
};

enum e_unit_weapons
{
	PrimaryWeapon,
	SecondaryWeapon,
	DualWeildWeapon
};

enum e_grenades : BYTE
{
	Fragmentation,
	Plasma
};

enum e_weapon_index : WORD
{
	Primary = 0xFF00,
	Secondary = 0xFF01,
	DualWeild = 0x0201
};

enum e_biped_physics_mode : BYTE
{
	mode_ground = 1,
	mode_flying,
	mode_dead,
	mode_posture,
	mode_climbing,
	mode_melee
};
#pragma pack(push,1)
struct s_object_data_definition
{
	datum tag_definition_index;
	DWORD object_flags;
	char gap_8[4];
	datum next_index;
	datum current_weapon_datum;		//maybe attachment or child
	datum parent_datum;
	WORD unit_in_vehicle_flag;
	INT16 placement_index;
	char gap_1C[12];
	DWORD location[2];
	real_point3d center;
	float radius;
	DWORD gap_3[9];
	real_point3d position;
	real_vector3d orientation;
	real_vector3d up;
	real_point3d translational_velocity;
	real_vector3d angular_velocity;
	float scale;
	datum unique_id;
	__int16 origin_bsp_index;
	e_object_type object_type;//TODO: confirm if its object_type or object_type_flags
	char gap_AB[1];
	__int16 name_list_index;
	char gap_5[1];
	char netgame_equipment_index;
	char placement_policy;
	char gap_6[3];
	datum havok_datum;
	char gap_B8[8];
	WORD field_C0;
	WORD field_C2;
	DWORD field_C4;
	DWORD field_C8;
	unsigned int field_CC;
	__int16 field_D0;
	char variant_index;//tag_block index
	char gap_D3;
	unsigned int field_D4;
	char field_D8;
	char gap_D9[7];
	WORD destroyed_constraints_flag;
	WORD loosened_constraints_flag;
	float body_max_vitality;
	float shield_max_vitality;
	float body_vitality;
	float shield_vitality;
	DWORD gap_F4[4];
	WORD shield_stun_ticks;
	WORD body_stun_ticks;
	char gap_108[2];
	WORD field_10A;		//(field_10A & 4) != 0 -- > object_is_dead
	PAD(32);
};
CHECK_STRUCT_SIZE(s_object_data_definition, 0x12C);

struct s_unit_data_definition :s_object_data_definition
{
	char gap_12C[4];
	DWORD field_130;
	datum controlling_actor_index;
	DWORD unit_flags;		  //(unit_data->unit_flags & 8) != 0   -->active_camo_active
							  //unit_data->unit_flags |= 2         -->unit_is_alive
	e_object_team unit_team;
	char pad[3];
	WORD controlling_player_index;
	char gap_142[14];
	DWORD control_flags;
	DWORD control_flags_2;
	DWORD animation_state;
	real_vector3d desired_facing;
	real_vector3d desired_aiming;
	real_vector3d aiming_vector;
	real_vector3d aiming_vector_velocity;
	real_vector3d desired_looking;
	real_vector3d looking_vector;
	real_vector3d looking_vector_velocity;
	DWORD field_1B0;
	DWORD field_1B4;
	DWORD field_1B8;
	real_vector3d throttle;
	char aiming_speed;			//might not be char
	char gap_1C9[3];
	float trigger;
	float secondary_trigger;
	char target_info[0x24];		 //TODO: add structure
	char gap_1F8[24];
	DWORD parent_seat_index;
	char gap_214[20];
	WORD weapon_set_identifier;
	char gap_22A[39];
	char current_grenade_index;
	WORD grenade_counts_mask;
	char gap_254;
	char zoom_level;
	char gap_256[110];
	float active_camo_power;
	char gap_2C8[4];
	float active_camo_regrowth;
	PAD(144);
};
CHECK_STRUCT_SIZE(s_unit_data_definition, 0x360);

struct s_biped_data_definition :s_unit_data_definition
{
	PAD(0x3F4 - sizeof(s_unit_data_definition));
	e_biped_physics_mode biped_mode;//0x3F4
	PAD(0x480 - 0x3F5);
};
CHECK_STRUCT_SIZE(s_biped_data_definition, 0x480);

struct s_weapon_data_definition : s_object_data_definition
{
	PAD(0x25C - sizeof(s_object_data_definition));
};
CHECK_STRUCT_SIZE(s_weapon_data_definition, 0x25C);

enum e_object_header_flag :BYTE
{
	_object_header_active_bit = 0x1,
	_object_header_requires_motion_bit = 0x2,
	object_header_flags_4 = 0x4,
	_object_header_being_deleted_bit = 0x8,
	object_header_flags_10 = 0x10,
	_object_header_connected_to_map_bit = 0x20,
	_object_header_child_bit = 0x40,
};

struct s_object_header {
	__int16 datum_salt; //0x00
	e_object_header_flag flags; // 0x02
	e_object_type type; // 0x03
	__int16 unk__;  // 0x04
	__int16 unk_size;  //0x06
	char* object; //0x08 - 
};
CHECK_STRUCT_SIZE(s_object_header, 0xC);
#pragma pack(pop)
