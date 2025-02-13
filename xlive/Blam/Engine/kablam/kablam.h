#pragma once

#include "H2MOD/GUI/ImGui_Integration/Console/CommandsUtil.h"

enum e_server_console_commands {
	_kablam_command_ban,
	_kablam_command_description,
	_kablam_command_exit,
	_kablam_command_kick,
	_kablam_command_live,
	_kablam_command_name,
	_kablam_command_play,
	_kablam_command_players,
	_kablam_command_playing,
	_kablam_command_privacy,
	_kablam_command_sendmsg,
	_kablam_command_skip,
	_kablam_command_statsfolder,
	_kablam_command_status,
	_kablam_command_unban,
	_kablam_command_vip,
	_kablam_command_any,

	k_kablam_command_count
};

// TODO: implement this properly
class c_kablam_interface
{
	virtual void execute() = 0;
};

struct c_kablam_command_send_msg // : c_kablam_interface
{
	void* vtbl; // we set the vtable manually for now
	DWORD field_4;
	DWORD field_8;
	wchar_t message[121];
	int32 unk_3;

	c_kablam_command_send_msg(void* _vtbl, const wchar_t* _message) : vtbl(_vtbl)
	{
		csmemset((void*)&field_4, 0, sizeof(c_kablam_command_send_msg) - sizeof(vtbl));
		wcscpy_s(message, ARRAYSIZE(message), _message);
	}

	void send_message()
	{
		INVOKE_TYPE(0x0, 0x7175, void(__thiscall*)(c_kablam_command_send_msg*), this);
	}
};
ASSERT_STRUCT_SIZE(c_kablam_command_send_msg, 260);

namespace ServerConsole
{
	static int __cdecl OutputCb(StringHeaderFlags flags, const char* fmt, ...);

	void ApplyHooks();
	void LogToDedicatedServerConsole(const wchar_t* fmt, ...);
	void SendCommand(wchar_t** command, int split_commands_size, char unk);
	void AddVip(std::wstring gamerTag);
	void ClearVip();
	void SendMsg(const wchar_t* message, bool timeout = false);
}