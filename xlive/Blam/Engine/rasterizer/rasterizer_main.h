#pragma once
#include "rasterizer_globals.h"

#include "bitmaps/bitmap_group.h"

/* prototypes */

void rasterizer_main_apply_patches(void);

void rasterizer_present_frame_wrapper(bitmap_data* bitmap);

void rasterizer_present_frame_screenshot_wrapper(bitmap_data* bitmap);

e_display_type rasterizer_get_display_type(void);
