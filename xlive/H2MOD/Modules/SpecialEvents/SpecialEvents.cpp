#include "stdafx.h"
#include "SpecialEvents.h"
#include "SpecialEventHelpers.h"

#include "game/game.h"
#include "networking/session/network_session.h"
#include "tag_files/tag_loader/tag_injection.h"

#include "H2MOD/Modules/Shell/Config.h"

// Enables event if the current date and time line up with an event time
e_special_event_type get_current_special_event()
{
	e_special_event_type event = _special_event_none;

	if (H2Config_no_events)
		return event;

	s_date_and_time date;
	game_time_get_date_and_time(&date);

#ifndef NDEBUG
	if (H2Config_forced_event != _special_event_none)
		return (e_special_event_type)H2Config_forced_event;
#endif

	switch (date.month)
	{
	case 0:
	default:
		break;
	}

	return event;
}

void load_special_event()
{
	if (tag_injection_check_map_exists(k_events_map))
	{
		switch (get_current_special_event())
		{
		case 0:
		default:
			break;
		}
	}
	else
	{
#ifndef IMGUI_DISABLE
		// *Memory::GetAddress<byte*>(0x46DCF1) = 1;
#endif
	}
}

