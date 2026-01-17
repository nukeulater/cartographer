#include "stdafx.h"
#include "screen_cartographer_errors.h"

#include "input/input_windows.h"
#include "interface/user_interface.h"
#include "interface/user_interface_controller.h"
#include "interface/user_interface_memory.h"
#include "interface/user_interface_utilities.h"
#include "main/game_preferences.h"

/* structures */

struct s_cartographer_error_globals
{
	const wchar_t* header_text;
	const wchar_t* subheader_text;
};

/* constants */

static const s_cartographer_error_globals k_cartographer_error_globals[k_language_count][k_cartographer_error_id_count] =
{
	{
		{
			L"Error!",
			L"Generic Error."
		},
		{
			L"Outdated Version!",
			L"You are using an outdated version of Project Cartographer! Please install the latest version.",
		},
		{
			L"Unknown Error!?",
			L"The error that has occured has not been handled. Please contact an admin including any log files you have and what you did."
		},
		{
			L"Something's Broken!",
			L"An internal error has occured. We've probably noticed this already but contact an admin if this persists."
		},
		{
			L"Restart Required",
			L"The setting you have just changed requires that you restart your game for it to take effect."
		},
	},
};

/* public code */

void c_cartographer_error_menu::get_error_label(e_cartographer_error_id error_id, const wchar_t** out_header_text, const wchar_t** out_subheader_text)
{
	*out_header_text = k_cartographer_error_globals[_language_english][error_id].header_text;
	*out_subheader_text = k_cartographer_error_globals[_language_english][error_id].subheader_text;
	return;
}

void* c_cartographer_error_menu::load_by_error_id(e_cartographer_error_id error_id) {

	s_screen_parameters params;
	c_cartographer_error_menu* error_menu = NULL;

	params.m_flags = 0;
	params.m_window_index = _window_4;
	params.m_context = NULL;
	params.m_user_flags = user_interface_controller_get_signed_in_controllers_mask() | FLAG(k_windows_device_controller_index);
	params.m_channel_type = _user_interface_channel_type_game_error;
	params.m_screen_state.field_0 = NONE;
	params.m_screen_state.m_last_focused_item_order = NONE;
	params.m_screen_state.m_last_focused_item_index = NONE;
	params.m_load_function = c_cartographer_error_menu::load;
	error_menu = (c_cartographer_error_menu*)params.m_load_function(&params);

	error_menu->m_error_id = error_id;
	return error_menu;
}

void* c_cartographer_error_menu::load(s_screen_parameters* parameters)
{
	c_cartographer_error_menu* error_menu = nullptr;
	BYTE* ui_buffer = ui_pool_allocate_space(sizeof(c_cartographer_error_menu), 0);
	if (ui_buffer) {
		error_menu = new (ui_buffer) c_cartographer_error_menu(parameters->m_channel_type, parameters->m_window_index, parameters->m_user_flags);
		error_menu->m_allocated	= true;
	}
	user_interface_register_screen_to_channel(error_menu, parameters);
	return error_menu;
}


c_cartographer_error_menu::c_cartographer_error_menu(e_user_interface_channel_type _ui_channel, e_user_interface_render_window _window_index, uint16 _flags) :
	c_screen_widget(_screen_error_dialog_ok, _ui_channel, _window_index, _flags)
{
	m_error_id = _cartpgrapher_error_id_none;
}

void c_cartographer_error_menu::pre_destroy()
{
}

bool c_cartographer_error_menu::handle_event(s_event_record* event)
{
	bool result = false;
	if (event->type == _user_interface_event_type_keyboard_button_pressed
		|| event->type == _user_interface_event_type_gamepad_button_pressed
		|| event->type == _user_interface_event_type_mouse_button_left_click)
	{
		if (event->component == _user_interface_keyboard_component_button_letter_a
			|| event->component == _controller_component_button_a)
		{
			start_widget_animation(3);
			result = true;
		}
	}

	//return c_screen_widget::handle_event(event);
	return result;
}

void c_cartographer_error_menu::initialize(s_screen_parameters* screen_parameters)
{
	s_interface_expected_screen_layout layout;
	csmemset(&layout, 0, sizeof(layout));
	layout.panes_count = 1;

	datum widget_tag_datum = user_interface_get_screen_tag_index_by_id(this->m_screen_id);
	if (widget_tag_datum != NONE)
	{
		this->verify_and_load_from_layout(widget_tag_datum, &layout);
	}
	this->setup_children();

	//update header and subheader labels

	const wchar_t* header_text = L"<unknown-error>";
	const wchar_t* subheader_text = L"<unknown-error-subheader>";

	if (m_error_id != _cartpgrapher_error_id_none)
	{
		get_error_label(m_error_id, &header_text, &subheader_text);
	}

	m_header_text.set_text(header_text);
	c_text_widget* subheader_text_widget = try_find_text_widget(k_sub_header_text_block_index);
	if (subheader_text_widget)
	{
		subheader_text_widget->set_text(subheader_text);
	}
}

const void* c_cartographer_error_menu::load_proc(void) const
{
	return c_cartographer_error_menu::load;
}
