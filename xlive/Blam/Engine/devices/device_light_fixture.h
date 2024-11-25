#pragma once
#include "devices.h"

/* structures */

struct _light_fixture_definition
{
	// Explaination("$$$ LIGHT FIXTURE $$$", "")
};

struct light_fixture_definition
{
	_object_definition object;
	_device_definition device;
	_light_fixture_definition light_fixture;
};
ASSERT_STRUCT_SIZE(light_fixture_definition, 284);
