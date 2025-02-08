#pragma once
#include "rasterizer_dx9_targets.h"

/* enums */

enum e_vertex_output_type : int32
{
    _vertex_output_type_position = 0,
    _vertex_output_type_texcoord = 1,
    _vertex_output_type_color = 2
};

/* prototypes */

void rasterizer_dx9_fullscreen_passes_apply_patches(void);

void rasterizer_dx9_viewport_calculate_position(const real_vector4d* location, real32 z_far, real_vector4d* output);

void rasterizer_dx9_fullscreen_texture_calculate_texcoords(const real_rectangle2d* bounds, const real_point2d* location, real_point2d* output);

void rasterizer_dx9_fullscreen_texture_window_calculate_texcoords(const real_rectangle2d* bounds, const real_point2d* location, real_point2d* output, int32 fullscreen_texture_stage = 0);

void __cdecl rasterizer_dx9_render_fullscreen_overlay_geometry(
    real_rectangle2d* a1,
    bool(__cdecl* a2)(void*),
    bool(__cdecl* a3)(
        int32 output_type,
        real_rectangle2d* bounds,
        real_vector4d* location,
        void* output,
        void* ctx),
    int32(__cdecl* a4)(int32),
    int32 a5,
    int16 a6,
    bool a7);

void __cdecl rasterizer_dx9_apply_gamma_and_brightness(e_rasterizer_target rasterizer_target);

bool __cdecl rasterizer_dx9_fullscreen_default_with_window_location_build_vertex_buffer(
    int32 output_type,
    real_rectangle2d* bounds,
    real_vector4d* location,
    void* output,
    void* ctx
);

bool __cdecl rasterizer_fullscreen_effects_build_vertex_buffer_color_ctx_cb(
    int32 output_type,
    real_rectangle2d* bounds,
    real_vector4d* location,
    void* output,
    void* ctx
);

