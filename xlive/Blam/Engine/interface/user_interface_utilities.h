#pragma once

enum e_user_interface_screen_id : uint32;

datum __cdecl user_interface_get_widget_tag_index_from_screen_id(e_user_interface_screen_id screen_id);

void user_interface_transition_to_offline();

void __cdecl user_interface_get_cursor_position(point2d* out_position);

void __cdecl user_interface_get_cursor_position_scaled(point2d* out_position);

void user_interface_utilities_apply_patches();