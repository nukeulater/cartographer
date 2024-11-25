#pragma once
#include "item_definitions.h"

/* structures */

struct _garbage_definition
{
	int8 pad[168];
};
ASSERT_STRUCT_SIZE(_garbage_definition, 168);

struct garbage_definition
{
	_object_definition object;
	_item_definition item;
	_garbage_definition garbage;
};
ASSERT_STRUCT_SIZE(garbage_definition, 0x1D4);
