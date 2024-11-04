#pragma once
#include "render/render_cameras.h"

// Get global camera data
render_camera* get_global_camera(void);

render_camera* get_effect_camera();

void camera_apply_patches(void);