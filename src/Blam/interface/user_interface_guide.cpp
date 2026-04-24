#include "stdafx.h"
#include "user_interface_guide.h"

/* public code */

static bool g_xlive_capturing_input = false;
static int g_input_shell_capturing_count = 0;

c_user_interface_guide_state_manager* user_interface_guide_state_manager_get(void)
{
	return Memory::GetAddress<c_user_interface_guide_state_manager*>(0x9712C8, 0x994A18);
}

bool c_user_interface_guide_state_manager::signed_in_to_live() const
{
	return m_sign_in_state == eXUserSigninState_SignedInToLive;
}

XUSER_SIGNIN_STATE c_user_interface_guide_state_manager::get_signin_state() const
{
	return m_sign_in_state;
}

void c_user_interface_guide_state_manager::add_user_signin_task(bool sign_to_live, void* signin_callback)
{
	//INVOKE_TYPE(0xDD7550, 0x0, int(__thiscall*)(c_user_interface_guide_state_manager*, bool, void*), this, sign_to_live, signin_callback);
	this->m_callback_task = signin_callback;

	DWORD flags;
	if (sign_to_live)
		flags = XSSUI_FLAGS_SHOWONLYONLINEENABLED;
	else
		flags = 0;

	XShowSigninUI(1, flags);
	return;
}

bool c_user_interface_guide_state_manager::should_keep_input_captured_by_shell()
{
	return g_xlive_capturing_input || g_input_shell_capturing_count > 0;
}

void c_user_interface_guide_state_manager::set_xlive_capturing_input(bool state)
{
	g_xlive_capturing_input = state;
}

void c_user_interface_guide_state_manager::set_input_captured_by_shell(bool state)
{
	g_input_shell_capturing_count += state ? 1 : -1;
	ASSERT(g_input_shell_capturing_count >= 0);

	if (!g_xlive_capturing_input)
	{
		if (g_input_shell_capturing_count == 0)
		{
			m_guide_open = false;
		}
		else
		{
			m_guide_open = true;
		}
	}
}