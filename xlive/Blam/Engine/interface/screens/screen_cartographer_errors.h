#pragma once
#include "interface/user_interface_widget_window.h"

/* enums */

enum e_cartographer_error_id
{
	_cartographer_error_id_generic_error,
	_cartographer_error_id_error_outdated_version,
	_cartographer_error_id_unknown_unhandled_error,
	_cartographer_error_id_internal_error,
	_cartographer_error_id_setting_requiring_game_restart,
	_cartographer_error_id_missing_resources,

	k_cartographer_error_id_count,
	_cartpgrapher_error_id_none = NONE,
};

/* classes */

class c_cartographer_error_menu : protected c_screen_widget
{
protected:
	e_cartographer_error_id m_error_id;
	static void get_error_label(e_cartographer_error_id error_id, const wchar_t** out_header_text, const wchar_t** out_subheader_text);

public:
	static void* load_by_error_id(e_cartographer_error_id error_id);
	static void* load(s_screen_parameters* a1);

	c_cartographer_error_menu(e_user_interface_channel_type _ui_channel, e_user_interface_render_window _window_index, uint16 _flags);


	// c_cartographer_error_menu virtual functions

	virtual ~c_cartographer_error_menu() = default;
	virtual void pre_destroy() override;
	virtual bool handle_event(s_event_record* event) override;
	virtual void initialize(s_screen_parameters* screen_parameters) override;
	virtual const void* load_proc(void) const override;

private:
};
