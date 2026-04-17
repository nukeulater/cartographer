#include "stdafx.h"
#include "font_cache.h"

#include "interface/hud.h"

/* public code */

void font_group_apply_hooks(void)
{
	PatchCall(Memory::GetAddress(0x6AE0D), get_text_size_from_font_cache);
	return;
}

// Multiply font size by the global text scale to fix scaling issues on resolutions higher than 1080 pixels vertically
int16 __cdecl get_text_size_from_font_cache(int32 font_cache_index)
{
	int16 size = INVOKE(0x31865, 0x0, get_text_size_from_font_cache, font_cache_index);
	return (int16)(size * *get_secondary_hud_scale());
}

