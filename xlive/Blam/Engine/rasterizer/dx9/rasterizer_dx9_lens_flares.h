#pragma once
#include "rasterizer_dx9_targets.h"

/* public code */

void rasterizer_dx9_lens_flares_apply_patches(void);

void __cdecl lens_flares_submit_occlusions(void);

e_rasterizer_target rasterizer_dx9_sun_glow_draw(datum tag_index, real_point3d* point, e_rasterizer_target rasterizer_target);

e_rasterizer_target rasterizer_dx9_sun_glow_occlude(datum tag_index, real_point3d* point, e_rasterizer_target rasterizer_target);
