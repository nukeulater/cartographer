#pragma once

enum e_special_event_type
{
	_special_event_none = 0,
	k_special_event_count
};

e_special_event_type get_current_special_event();
void load_special_event();
