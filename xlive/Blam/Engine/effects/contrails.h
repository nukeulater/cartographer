#pragma once
#include "objects/object_placement.h"

/* enums */

enum e_contrail_render_type : uint32
{
	_contrail_render_type_vertical = 0x0,
	_contrail_render_type_horizontal = 0x1,
	_contrail_render_type_media = 0x2,
	_contrail_render_type_ground = 0x3,
	_contrail_render_type_viewer = 0x4,
	_contrail_render_type_linked = 0x5,
	NUMBER_OF_CONTRAIL_DEFINITION_RENDER_TYPES,

	CONTRAIL_RENDER_TYPE_GROUND_FLAGS = FLAG(_contrail_render_type_ground),
	CONTRAIL_RENDER_TYPE_MEDIA_FLAGS = FLAG(_contrail_render_type_media),
	CONTRAIL_RENDER_TYPE_IMMEDIATE_FLAGS = FLAG(_contrail_render_type_media) | FLAG(_contrail_render_type_ground),
	CONTRAIL_RENDER_TYPE_NORMAL_FLAGS = ~(FLAG(_contrail_render_type_media) | FLAG(_contrail_render_type_ground))
};

enum e_contrail_datum_flags : uint16
{
	_contrail_active_bit = 0,
	NUMBER_OF_CONTRAIL_FLAGS
};

enum e_contrail_point_datum_flags : uint8
{
	_contrail_point_new_bit = 0,
	_contrail_point_transitioning_bit = 1,
	_contrail_point_living_dead_bit = 2,
	NUMBER_OF_CONTRAIL_POINT_FLAGS
};

/* structures */

struct contrail_datum
{
	int16 identifier;
	e_contrail_datum_flags flags;
	datum definition_index;
	datum object_index;
	int16 attachment_index;
	int16 pad;
	string_id density_function_index;
	real32 density;
	int16 sequence_index;
	int16 frame_index;
	real32 texture_offset_u;
	real32 texture_offset_v;
	real32 time_until_point;
	real32 frame_time;
	real32 expired_dt;
	int16 contrail_point_counts[4];
	int32 first_contrail_point_indices[4];
};

struct contrail_point_datum
{
	int16 identifier;
	e_contrail_point_datum_flags flags;
	int8 state_index;
	real32 time;
	real32 delta;
	real32 density;
	real32 width;
	s_location location;
	real_point3d position;
	real_vector3d velocity;
	int32 next_contrail_point_index;
};

/* prototypes */

void contrails_apply_patches(void);
