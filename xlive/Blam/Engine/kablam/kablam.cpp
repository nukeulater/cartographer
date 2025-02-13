#include "stdafx.h"

#include "kablam.h"

#include "cseries/cseries_strings.h"
#include "shell/shell_windows.h"

#include "H2MOD/GUI/ImGui_Integration/Console/CommandHandler.h"
#include "H2MOD/Modules/EventHandler/EventHandler.hpp"

typedef void* (__cdecl* dedi_command_t)(wchar_t** a1, int32 a2, bool a3);
dedi_command_t p_kablam_command_handler;

typedef int32(__cdecl* kablam_vip_add_t)(LPCWSTR gamer_tag);
kablam_vip_add_t p_kablam_vip_add;

typedef int32(__cdecl* kablam_vip_clear_t)();
kablam_vip_clear_t p_kablam_vip_clear;

static std::map<const wchar_t*, e_server_console_commands> commands_map;

void* __cdecl kablam_command_handler_hook(wchar_t** command_line_split_wide, int split_count, bool a3) {

	wchar_t* command = command_line_split_wide[0];
	if (command[0] == L'$') {
		ServerConsole::LogToDedicatedServerConsoleWide(L"# running custom command: ");
		
		c_static_string<256> command_line;
		c_static_wchar_string<256> token_wide;

		for (int32 i = 0; i < split_count; i++)
		{
			if (i > 0)
			{
				token_wide.set(command_line_split_wide[i]);
			}
			else
			{
				// skip $ character
				token_wide.set(&command_line_split_wide[i][1]);
			}

			utf8 converted[256];
			wchar_string_to_utf8_string(token_wide.get_string(), converted, NUMBEROF(converted));

			command_line.append(converted);
			command_line.append(" ");
		}

		ConsoleCommand::HandleCommandLine(command_line.get_string(), command_line.length(), ServerConsole::OutputCb);
		return 0;
	}

	// Transform the command into lower invariant so that we can convert the string to it's enum value
	c_static_wchar_string<256> lower_command(command);
	lower_command.to_lower();

	// Execute any events that may be linked to that command.
	// EventHandler::executeServerCommandCallback(ServerConsole::s_commandsMap[LowerCommand]);

	// Commented out because it's unused but the functionality of it remains, incase it is ever needed in the future for modifying the behaviour of the commands.
	/*switch(ServerConsole::s_commandsMap[LowerCommand])
	{
		case ServerConsole::ServerConsoleCommands::skip: {
			break;
		}
		case ServerConsole::ban: break;
		case ServerConsole::description: break;
		case ServerConsole::exit: break;
		case ServerConsole::kick: break;
		case ServerConsole::live: break;
		case ServerConsole::name: break;
		case ServerConsole::play: break;
		case ServerConsole::players: break;
		case ServerConsole::playing: break;
		case ServerConsole::privacy: break;
		case ServerConsole::sendmsg: break;
		case ServerConsole::statsfolder: break;
		case ServerConsole::status: break;
		case ServerConsole::unban: break;
		case ServerConsole::vip: break;
		default: ;
	}*/

	// Temporary if statement to prevent double calling events,
	// all server command functions will be hooked in the future and these event executes will be removed.

	bool playCommand = false;
	auto playCommandFind = commands_map.find(lower_command.get_string());
	if (playCommandFind != commands_map.end()
		&& playCommandFind->second == _kablam_command_play)
		playCommand = true;

	if (!playCommand)
		EventHandler::ServerCommandEventExecute(EventExecutionType::execute_before, commands_map[lower_command.get_string()]);

	void* result = p_kablam_command_handler(command_line_split_wide, split_count, a3);

	// Temporary if statement to prevent double calling events,
	// all server command functions will be hooked in the future and these executes will be removed.
	
	if (!playCommand)
		EventHandler::ServerCommandEventExecute(EventExecutionType::execute_after, commands_map[lower_command.get_string()]);

	return result;
}

typedef bool(__cdecl kablam_command_play_t)(wchar_t* playlist_file_path, int32 a2);
kablam_command_play_t* p_kablam_command_play;

bool __cdecl kablam_command_play(wchar_t* playlist_file_path, int32 a2)
{
	LOG_INFO_GAME("[{}]: {}", __FUNCTION__, "");
	EventHandler::ServerCommandEventExecute(EventExecutionType::execute_before, _kablam_command_play);
	auto res = p_kablam_command_play(playlist_file_path, a2);
	EventHandler::ServerCommandEventExecute(EventExecutionType::execute_after, _kablam_command_play);
	return res;
}

void ServerConsole::ApplyHooks()
{
	if (!Memory::IsDedicatedServer())
		return;
	commands_map[L"ban"] = _kablam_command_ban;
	commands_map[L"description"] = _kablam_command_description;
	commands_map[L"exit"] = _kablam_command_exit;
	commands_map[L"kick"] = _kablam_command_kick;
	commands_map[L"live"] = _kablam_command_live;
	commands_map[L"name"] = _kablam_command_name;
	commands_map[L"play"] = _kablam_command_play;
	commands_map[L"players"] = _kablam_command_players;
	commands_map[L"playing"] = _kablam_command_playing;
	commands_map[L"privacy"] = _kablam_command_privacy;
	commands_map[L"sendmsg"] = _kablam_command_sendmsg;
	commands_map[L"skip"] = _kablam_command_skip;
	commands_map[L"statsfolder"] = _kablam_command_statsfolder;
	commands_map[L"status"] = _kablam_command_status;
	commands_map[L"unban"] = _kablam_command_unban;
	commands_map[L"vip"] = _kablam_command_vip;
	commands_map[L"any"] = _kablam_command_any;

	p_kablam_command_handler = (dedi_command_t)DetourFunc(Memory::GetAddress<BYTE*>(0, 0x1CCFC), (BYTE*)kablam_command_handler_hook, 7);
	p_kablam_vip_add = Memory::GetAddress<kablam_vip_add_t>(0, 0x1D932);
	p_kablam_vip_clear = Memory::GetAddress<kablam_vip_clear_t>(0, 0x1DB16);

	p_kablam_command_play = Memory::GetAddress<kablam_command_play_t*>(0, 0xE7FA);
	PatchCall(Memory::GetAddress(0, 0x724B), kablam_command_play);
}

int ServerConsole::LogToDedicatedServerConsoleWide(const wchar_t* fmt, ...) 
{
	if (!Memory::IsDedicatedServer())
		return 0;

	int result;

	va_list ap;
	va_start(ap, fmt);
	result = vwprintf(fmt, ap);
	va_end(ap);

	return result;
}

int ServerConsole::LogToDedicatedServerConsole(const char* fmt, ...)
{
	if (!Memory::IsDedicatedServer())
		return 0;

	int result;

	va_list ap;
	va_start(ap, fmt);
	result = vprintf(fmt, ap);
	va_end(ap);

	return result;
}

void ServerConsole::SendCommand(wchar_t** command, int32 split_commands_size, bool a3)
{
	typedef void(__cdecl* unk_func1_t)(void* a1);
	auto p_fn_unk1 = Memory::GetAddress<unk_func1_t>(0, 0x1D6EA);

	typedef void(__cdecl* free_memory_game_t)(void* mem);
	auto p_free_memory_game = Memory::GetAddress<free_memory_game_t>(0x0, 0x2344B8);

	typedef void(__thiscall* async_set_atomic_long_value_t)(void* thisx, long value);
	auto p_async_set_atomic_long_value = Memory::GetAddress<async_set_atomic_long_value_t>(0, 0x6E00);

	typedef void(__cdecl* unk_func3_t)(wchar_t* a1);
	auto p_fn_unk3 = Memory::GetAddress<unk_func3_t>(0, 0x19C93);

	uint8* unk1 = (uint8*)p_kablam_command_handler(command, split_commands_size, a3);
	uint8* threadparams = Memory::GetAddress<uint8*>(0, 0x450680);

	if (unk1)
	{
		if (*(BYTE*)(unk1 + 8))
		{
			p_async_set_atomic_long_value(*(BYTE**)(threadparams + 8), (LONG)unk1);
			p_fn_unk1(unk1);
		}
		else
		{
			p_fn_unk1(unk1);
			p_fn_unk3(*command);
		}

		// free allocated command resources
		p_free_memory_game(unk1);
	}

	BYTE v8 = (*(BYTE**)(threadparams + 8))[20];
	*(BYTE*)(threadparams + 4) = v8;
	if (!v8)
		LogToDedicatedServerConsoleWide(L"\r\n");
}

void ServerConsole::AddVip(std::wstring gamerTag)
{
	p_kablam_vip_add(gamerTag.c_str());
}

void ServerConsole::ClearVip()
{
	p_kablam_vip_clear();
}

static int32 message_timeout = 0;
void ServerConsole::SendMsg(const wchar_t* message, bool timeout)
{
	bool should_execute = !timeout;
	if (system_milliseconds() - message_timeout > 10 * 1000)
	{
		message_timeout = system_milliseconds();
		should_execute = true;
	}

	if (should_execute) {

		// first we construct kablam_command_send_msg, by manually passing the vtable pointer, and the message to be copied
		c_kablam_command_send_msg sendMsgCommand(message);

		// send the message
		sendMsgCommand.execute();
	}
}

static int __cdecl ServerConsole::OutputCb(StringHeaderFlags flags, const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vprintf(fmt, ap);
	printf("\n");
	va_end(ap);
	return 0;
}
