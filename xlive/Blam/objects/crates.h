#pragma once
#include "object_definition.h"

/* enums */

enum e_crate_definition_flags : uint16
{
	_crate_does_not_block_aoe_bit = 0,
	k_number_of_crate_definition_flags
};

/* structures */

struct _crate_definition
{
	c_flags_no_init<e_crate_definition_flags, uint16, k_number_of_crate_definition_flags> flags;
	int16 pad;
};
ASSERT_STRUCT_SIZE(_crate_definition, 4);

struct crate_definition
{
	_object_definition object;
	_crate_definition crate;
};
ASSERT_STRUCT_SIZE(crate_definition, 192);
