#include "stdafx.h"
#include "user_interface_guide.h"

/* public code */

c_user_interface_guide_state_manager* user_interface_guide_state_manager_get(void)
{
	return Memory::GetAddress<c_user_interface_guide_state_manager*>(0x9712C8, 0x994A18);
}

bool c_user_interface_guide_state_manager::signed_in_to_live() const
{
	return m_sign_in_state == eXUserSigninState_SignedInToLive;
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
