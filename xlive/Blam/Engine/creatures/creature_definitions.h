#pragma once
#include "game/game_allegiance.h"
#include "objects/object_definition.h"
#include "physics/character_physics_definitions.h"

/* enums */

enum e_creature_definition_flags : uint32
{
	_creature_definition_unused_bit = 0,
	_creature_definition_infection_form = 1,
	_creature_definition_immune_to_falling_damage = 2,
	_creature_definition_rotate_while_airborne = 3,
	_creature_definition_zapped_by_shields = 4,
	_creature_definition_attach_upon_impact = 5,
	_creature_definition_not_on_motion_sensor = 6,
	k_creature_definition_flag_count
};

enum e_creature_motion_sensor_blip_size : int16
{
	_creature_motion_sensor_blip_size_medium = 0,
	_creature_motion_sensor_blip_size_small = 1,
	_creature_motion_sensor_blip_size_large = 2
};

/* structures */

struct _creature_definition
{
	// Explaination("$$$ CREATURE $$$", "")

	c_flags_no_init<e_creature_definition_flags, uint32, k_creature_definition_flag_count> flags;
	e_game_team default_team;
	e_creature_motion_sensor_blip_size motion_sensor_blip_size;
	real32 turning_velocity_maximum;		// Degrees Per Second
	real32 turning_acceleration_maximum;	// Degrees Per Second Squared
	real32 casual_turning_modifier;
	real32 autoaim_width;					// World Units
	s_character_physics_definition physics;

	tag_reference impact_damage;		// jpt!
	
	// if not specified, uses 'impact damage'
	tag_reference impact_shield_damage;	// jpt!
	
	// Explaination("death and destruction", "")
	
	// if non-zero, the creature will destroy itself upon death after this much time
	real_vector2d destroy_after_death_time;	// Seconds
};

struct creature_definition
{
	_object_definition object;
	_creature_definition creature;
};
ASSERT_STRUCT_SIZE(creature_definition, 384);

/* prototypes */

void __cdecl creature_definitions_fixup(datum creature_datum);
