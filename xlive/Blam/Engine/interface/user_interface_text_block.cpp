#include "stdafx.h"
#include "user_interface_text_block.h"

/* public code */

c_small_user_interface_text::c_small_user_interface_text(void)
{
	m_string[0] = L'\0';
}

void  c_small_user_interface_text::set_text(const wchar_t* initial_text)
{
	INVOKE_TYPE(0x20F425, 0x0, void(__thiscall*)(c_small_user_interface_text*, const wchar_t*), this, initial_text);
}
void c_small_user_interface_text::append_text(const wchar_t* update_text)
{
	INVOKE_TYPE(0x20F462, 0x0, void(__thiscall*)(c_small_user_interface_text*, const wchar_t*), this, update_text);
}
const wchar_t* c_small_user_interface_text::get_raw_string(void)
{
	return INVOKE_TYPE(0x20F4A7, 0x0, const wchar_t*(__thiscall*)(c_small_user_interface_text*), this);
}


c_normal_user_interface_text::c_normal_user_interface_text(void)
{
	m_string[0] = L'\0';
	return;
}

void  c_normal_user_interface_text::set_text(const wchar_t* initial_text)
{
	INVOKE_TYPE(0x20F4C9, 0x0, void(__thiscall*)(c_normal_user_interface_text*, const wchar_t*), this, initial_text);
}
void c_normal_user_interface_text::append_text(const wchar_t* update_text)
{
	INVOKE_TYPE(0x20F509, 0x0, void(__thiscall*)(c_normal_user_interface_text*, const wchar_t*), this, update_text);
}
const wchar_t* c_normal_user_interface_text::get_raw_string(void)
{
	return INVOKE_TYPE(0x20F554, 0x0, const wchar_t* (__thiscall*)(c_normal_user_interface_text*), this);
}


c_long_user_interface_text::c_long_user_interface_text(void)
{
	m_string[0] = L'\0';
	return;
}

void c_long_user_interface_text::set_text(const wchar_t* initial_text)
{
	INVOKE_TYPE(0x243DDB, 0x0, void(__thiscall*)(c_long_user_interface_text*, const wchar_t*), this, initial_text);
}
void c_long_user_interface_text::append_text(const wchar_t* update_text)
{
	INVOKE_TYPE(0x243E1B, 0x0, void(__thiscall*)(c_long_user_interface_text*, const wchar_t*), this, update_text);
}
const wchar_t* c_long_user_interface_text::get_raw_string(void)
{
	return INVOKE_TYPE(0x243E66, 0x0, const wchar_t* (__thiscall*)(c_long_user_interface_text*), this);
}
