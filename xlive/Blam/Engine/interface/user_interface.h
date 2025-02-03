#pragma once

/* enums */

//TODO : figure out proper place to keep this enum
enum e_ui_error_types : uint32
{
	_ui_error_unknown = 0x0,
	_ui_error_generic = 0x1,
	_ui_error_generic_networking = 0x2,
	_ui_error_system_link_generic_join_failure = 0x3,
	_ui_error_system_link_no_network_connection = 0x4,
	_ui_error_system_link_connection_lost = 0x5,
	_ui_error_network_game_oos = 0x6,
	_ui_error_xbox_live_sign_out_confirmation = 0x7,
	_ui_error_confirm_revert_to_last_save = 0x8,
	_ui_error_confirm_quit_without_save = 0x9,
	_ui_error_confirm_delete_player_profile = 0xA,
	_ui_error_confirm_delete_variant = 0xB,
	_ui_error_player_profile_creation_failed = 0xC,
	_ui_error_variant_profile_creation_failed = 0xD,
	_ui_error_playlist_creation_failed = 0xE,
	_ui_error_core_file_load_failed = 0xF,
	_ui_error_mu_removed_during_player_profile_save = 0x10,
	_ui_error_mu_removed_during_variant_save = 0x11,
	_ui_error_mu_removed_during_playlist_save = 0x12,
	_ui_error_message_saving_to_mu = 0x13,
	_ui_error_message_saving_file = 0x14,
	_ui_error_message_creating_player_profile = 0x15,
	_ui_error_message_creating_variant_profile = 0x16,
	_ui_error_message_saving_checkpoint = 0x17,
	_ui_error_failed_to_load_player_profile = 0x18,
	_ui_error_failed_to_load_variant = 0x19,
	_ui_error_failed_to_load_playlist = 0x1A,
	_ui_error_failed_to_load_save_game = 0x1B,
	_ui_error_controller1_removed = 0x1C,
	_ui_error_controller2_removed = 0x1D,
	_ui_error_controller3_removed = 0x1E,
	_ui_error_controller4_removed = 0x1F,
	_ui_error_need_more_free_blocks_to_save = 0x20,
	_ui_error_maximum_saved_game_files_already_exist = 0x21,
	_ui_error_dirty_disk = 0x22,
	_ui_error_xblive_cannot_access_service = 0x23,
	_ui_error_xblive_title_update_required = 0x24,
	_ui_error_xblive_servers_too_busy = 0x25,
	_ui_error_xblive_duplicate_logon = 0x26,
	_ui_error_xblive_account_management_required = 0x27,
	_ui_error_warning_xblive_recommended_messages_available = 0x28,
	_ui_error_xblive_invalid_match_session = 0x29,
	_ui_error_warning_xblive_poor_network_performance = 0x2A,
	_ui_error_not_enough_open_slots_to_join_match_session = 0x2B,
	_ui_error_xblive_corrupt_download_content = 0x2C,
	_ui_error_confirm_xblive_corrupt_saved_game_file_removal = 0x2D,
	_ui_error_xblive_invalid_user_account = 0x2E,
	_ui_error_confirm_boot_clan_member = 0x2F,
	_ui_error_confirm_controller_sign_out = 0x30,
	_ui_error_beta_xblive_service_qos_report = 0x31,
	_ui_error_beta_feature_disabled = 0x32,
	_ui_error_beta_network_connection_required = 0x33,
	_ui_error_confirm_friend_removal = 0x34,
	_ui_error_confirm_boot_to_dash = 0x35,
	_ui_error_confirm_launch_xdemos = 0x36,
	_ui_error_confirm_exit_game_session = 0x37,
	_ui_error_xblive_connection_to_xbox_live_lost = 0x38,
	_ui_error_xblive_message_send_failure = 0x39,
	_ui_error_network_link_lost = 0x3A,
	_ui_error_network_link_required = 0x3B,
	_ui_error_xblive_invalid_passcode = 0x3C,
	_ui_error_join_aborted = 0x3D,
	_ui_error_join_session_not_found = 0x3E,
	_ui_error_join_qos_failure = 0x3F,
	_ui_error_join_data_decode_failure = 0x40,
	_ui_error_join_game_full = 0x41,
	_ui_error_join_game_closed = 0x42,
	_ui_error_join_version_mismatch = 0x43,
	_ui_error_join_failed_unknown_reason = 0x44,
	_ui_error_join_failed_friend_in_matchmade_game = 0x45,
	_ui_error_player_profile_name_must_be_unique = 0x46,
	_ui_error_variant_name_must_be_unique = 0x47,
	_ui_error_playlist_name_must_be_unique = 0x48,
	_ui_error_saved_film_name_must_be_unique = 0x49,
	_ui_error_no_free_slots_player_profile = 0x4A,
	_ui_error_no_free_slots_variant = 0x4B,
	_ui_error_no_free_slots_playlist = 0x4C,
	_ui_error_no_free_slots_saved_film = 0x4D,
	_ui_error_need_more_space_for_player_profile = 0x4E,
	_ui_error_need_more_space_for_variant = 0x4F,
	_ui_error_need_more_space_for_playlist = 0x50,
	_ui_error_need_more_space_for_saved_film = 0x51,
	_ui_error_cannot_set_privileges_on_member_whose_data_not_known = 0x52,
	_ui_error_cant_delete_default_profile = 0x53,
	_ui_error_cant_delete_default_variant = 0x54,
	_ui_error_cant_delete_default_playlist = 0x55,
	_ui_error_cant_delete_default_saved_film = 0x56,
	_ui_error_cant_delete_profile_in_use = 0x57,
	_ui_error_player_profile_name_must_have_alphanumeric_characters = 0x58,
	_ui_error_variant_name_must_have_alphanumeric_characters = 0x59,
	_ui_error_playlist_name_must_have_alphanumeric_characters = 0x5A,
	_ui_error_saved_film_name_must_have_alphanumeric_characters = 0x5B,
	_ui_error_teams_not_a_member = 0x5C,
	_ui_error_teams_insufficient_privileges = 0x5D,
	_ui_error_teams_server_busy = 0x5E,
	_ui_error_teams_team_full = 0x5F,
	_ui_error_teams_member_pending = 0x60,
	_ui_error_teams_too_many_requests = 0x61,
	_ui_error_teams_user_already_exists = 0x62,
	_ui_error_teams_user_not_found = 0x63,
	_ui_error_teams_user_teams_full = 0x64,
	_ui_error_teams_no_task = 0x65,
	_ui_error_teams_too_many_teams = 0x66,
	_ui_error_teams_team_already_exists = 0x67,
	_ui_error_teams_team_not_found = 0x68,
	_ui_error_teams_name_contains_bad_words = 0x69,
	_ui_error_teams_description_contains_bad_words = 0x6A,
	_ui_error_teams_motto_contains_bad_words = 0x6B,
	_ui_error_teams_url_contains_bad_words = 0x6C,
	_ui_error_teams_no_admin = 0x6D,
	_ui_error_teams_cannot_set_privileges_on_member_whose_data_not_known = 0x6E,
	_ui_error_live_unknown = 0x6F,
	_ui_error_confirm_delete_profile = 0x70,
	_ui_error_confirm_delete_playlist = 0x71,
	_ui_error_confirm_delete_saved_film = 0x72,
	_ui_error_confirm_live_sign_out = 0x73,
	_ui_error_confirm_confirm_friend_removal = 0x74,
	_ui_error_confirm_promotion_to_superuser = 0x75,
	_ui_error_warn_no_more_clan_superusers = 0x76,
	_ui_error_confirm_corrupt_profile = 0x77,
	_ui_error_confirm_xbox_live_sign_out = 0x78,
	_ui_error_confirm_corrupt_game_variant = 0x79,
	_ui_error_confirm_leave_clan = 0x7A,
	_ui_error_confirm_corrupt_playlist = 0x7B,
	_ui_error_cant_join_gameinvite_without_signon = 0x7C,
	_ui_confirm_proceed_to_crossgame_invite = 0x7D,
	_ui_confirm_decline_crossgame_invite = 0x7E,
	_ui_warn_insert_cd_for_crossgame_invite = 0x7F,
	_ui_error_need_more_space_for_saved_game = 0x80,
	_ui_error_saved_game_cannot_be_loaded = 0x81,
	_ui_error_confirm_controller_signout_with_guests = 0x82,
	_ui_error_warning_party_closed = 0x83,
	_ui_error_warning_party_required = 0x84,
	_ui_error_warning_party_full = 0x85,
	_ui_error_warning_player_in_mm_game = 0x86,
	_ui_error_xblive_failed_to_sign_in = 0x87,
	_ui_error_cant_sign_out_master_with_guests = 0x88,
	_ui_error_this_dot_command_is_obsolete = 0x89,
	_ui_error_this_has_not_been_unlocked = 0x8A,
	_ui_error_confirm_leave_lobby = 0x8B,
	_ui_error_confirm_party_leader_leave_matchmaking = 0x8C,
	_ui_error_confirm_single_box_leave_matchmaking = 0x8D,
	_ui_error_clan_name_not_valid = 0x8E,
	_ui_error_player_list_full = 0x8F,
	_ui_error_recipient_has_blocked_you = 0x90,
	_ui_error_friend_pending = 0x91,
	_ui_error_too_many_requests = 0x92,
	_ui_error_player_already_in_list = 0x93,
	_ui_error_gamertag_not_found = 0x94,
	_ui_error_cannot_message_self = 0x95,
	_ui_error_warning_last_overlord_cant_leave_clan = 0x96,
	_ui_error_confirm_boot_player = 0x97,
	_ui_error_confirm_party_member_leave_pcr = 0x98,
	_ui_error_cannot_sign_in_during_countdown = 0x99,
	_ui_error_xblive_invalid_user = 0x9A,
	_ui_error_xblive_user_not_authorized = 0x9B,
	_ui_error_OBSOLETE = 0x9C,
	_ui_error_OBSOLETE2 = 0x9D,
	_ui_error_xblive_banned_xbox = 0x9E,
	_ui_error_xblive_banned_user = 0x9F,
	_ui_error_xblive_banned_title = 0xA0,
	_ui_error_confirm_exit_game_session_leader = 0xA1,
	_ui_error_message_objectionable_content = 0xA2,
	_ui_error_confirm_enter_downloader = 0xA3,
	_ui_error_confirm_block_user = 0xA4,
	_ui_error_confirm_negative_feedback = 0xA5,
	_ui_error_confirm_change_clan_member_level = 0xA6,
	_ui_error_blank_gamertag = 0xA7,
	_ui_error_confirm_save_and_exit_campaign = 0xA8,
	_ui_error_cant_join_during_matchmaking = 0xA9,
	_ui_error_confirm_restart_level = 0xAA,
	_ui_error_matchmaking_failure_generic = 0xAB,
	_ui_error_matchmaking_failure_missing_content = 0xAC,
	_ui_error_matchmaking_failure_aborted = 0xAD,
	_ui_error_matchmaking_failure_membership_changed = 0xAE,
	_ui_error_confirm_end_game_session = 0xAF,
	_ui_error_confirm_exit_game_session_only_player = 0xB0,
	_ui_error_confirm_exit_game_session_xbox_live_ranked_leader = 0xB1,
	_ui_error_confirm_exit_game_session_xbox_live_ranked = 0xB2,
	_ui_error_confirm_exit_game_session_xbox_live_leader = 0xB3,
	_ui_error_confirm_exit_game_session_xbox_live_only_player = 0xB4,
	_ui_error_confirm_exit_game_session_xbox_live = 0xB5,
	_ui_error_recipient_list_full = 0xB6,
	_ui_error_confirm_exit_campaign = 0xB7,
	_ui_error_xblive_connection_to_xbox_live_lost_save_and_quit = 0xB8,
	_ui_error_booted_from_session = 0xB9,
	_ui_error_confirm_exit_game_session_xbox_live_guest = 0xBA,
	_ui_error_confirm_exit_game_session_xbox_live_ranked_only_player = 0xBB,
	_ui_error_confirm_exit_game_session_xbox_live_unranked_only_player = 0xBC,
	_ui_error_confirm_exit_game_session_xbox_live_unranked_leader = 0xBD,
	_ui_error_confirm_exit_game_session_xbox_live_unranked = 0xBE,
	_ui_error_cant_join_friend_while_in_matchmade_game = 0xBF,
	_ui_error_map_load_failure = 0xC0,
	_ui_error_confirm_campaign_without_achievements = 0xC1,
	_ui_error_no_live_menu_branch_without_signin = 0xC2,
	_ui_error_map_out_of_hard_disk_space = 0xC3,
	_ui_error_device_not_supported = 0xC4,
	_ui_error_achievements_interrupted = 0xC5,
	_confirm_lose_progress = 0xC6,
	_ui_error_beta_achievements_disabled = 0xC7,
	_ui_error_cannot_connect_versions_wrong = 0xC8,
	_ui_error_confirm_booted_from_session = 0xC9,
	_ui_error_confirm_boot_player_from_squad = 0xCA,
	_ui_error_confirm_leave_system_link_lobby = 0xCB,
	_ui_error_confirm_party_member_leave_matchmaking = 0xCC,
	_ui_error_confirm_quit_single_player = 0xCD,
	_ui_error_controller_removed = 0xCE,
	_ui_error_download_in_progress = 0xCF,
	_ui_error_download_fail = 0xD0,
	_ui_error_failed_to_load_map = 0xD1,
	_ui_error_feature_requires_gold = 0xD2,
	_ui_error_keyboard_mapping = 0xD3,
	_ui_error_keyboard_removed = 0xD4,
	_ui_error_live_game_unavailable = 0xD5,
	_ui_error_map_missing = 0xD6,
	_ui_error_matchmaking_failed_generic = 0xD7,
	_ui_error_matchmaking_failed_missing_content = 0xD8,
	_ui_error_mouse_removed = 0xD9,
	_ui_error_party_not_all_on_live = 0xDA,
	_ui_error_party_subnet_not_shared = 0xDB,
	_ui_error_required_game_update = 0xDC,
	_ui_error_saved_game_cannot_be_saved = 0xDD,
	_ui_error_sound_microphone_not_supported = 0xDE,
	_ui_error_system_link_direct_IP = 0xDF,
	_ui_error_text_chat_muted = 0xE0,
	_ui_error_text_chat_parental_controls = 0xE1,
	_ui_error_update_start = 0xE2,
	_ui_error_update_fail = 0xE3,
	_ui_error_update_fail_blocks = 0xE4,
	_ui_error_update_exists = 0xE5,
	_ui_error_insert_original = 0xE6,
	_ui_error_update_fail_network_lost = 0xE7,
	_ui_error_update_mp_out_of_sync = 0xE8,
	_ui_error_update_must_upgrade = 0xE9,
	_ui_error_voice_gold_required = 0xEA,
	_ui_error_voice_parental_controls = 0xEB,
	_ui_error_warning_xblive_poor_network_perofrmance = 0xEC,
	_ui_error_you_missing_map = 0xED,
	_ui_error_someone_missing_map = 0xEE,
	_ui_error_tnp_no_source = 0xEF,
	_ui_error_tnp_disk_read = 0xF0,
	_ui_error_tnp_no_engine_running = 0xF1,
	_ui_error_tnp_signature_verification = 0xF2,
	_ui_error_tnp_drive_removed = 0xF3,
	_ui_error_tnp_disk_full = 0xF4,
	_ui_error_tnp_permissions = 0xF5,
	_ui_error_tnp_unknown = 0xF6,
	_ui_error_continue_install = 0xF7,
	_ui_error_cancel_install = 0xF8,
	_ui_error_confirm_upsell_gold = 0xF9,
	_ui_error_add_to_favorites = 0xFA,
	_ui_error_remove_from_favorites = 0xFB,
	_ui_error_updating_favorites = 0xFC,
	_ui_error_choose_exisiting_checkpoint_location = 0xFD,
	_ui_error_choose_new_checkpoint_location_checkpoints_exist_on_live_and_locally = 0xFE,
	_ui_error_choose_new_checkpoint_location_checkpoints_exist_on_live = 0xFF,
	_ui_error_choose_new_checkpoint_location_checkpoints_exist_locally = 0x100,
	_ui_error_download_map = 0x101,
	_ui_error_want_to_download_map = 0x102,
	_ui_error_ok_download_map = 0x103,
	_ui_error_cancel_download_map = 0x104,
	_ui_error_not_gold_no_map_download = 0x105,
	_ui_error_map_download_connection_lost = 0x106,
	_ui_error_map_download_collision = 0x107,
	_ui_error_map_download_disk_write_error = 0x108,
	_ui_error_matchmaking_failed_no_games = 0x109,
	_ui_error_matchmaking_failed_timeout = 0x10A,
	_ui_error_live_checkpoint_connection_dropped = 0x10B,
	_ui_error_live_checkpoint_hash_mismatch = 0x10C,
	_ui_error_join_gold_game_not_allowed = 0x10D,
	_ui_error_join_locked_game_not_allowed = 0x10E,
	_ui_error_system_link_port_in_use = 0x10F,
	_ui_error_invite_requires_signin = 0x110,
	_ui_error_overwrite_custom_keyboard_mappings = 0x111,
	_ui_error_profile_version_mismatch = 0x112,
	_ui_error_profane_map_name = 0x113,
	_ui_error_profane_variant_name = 0x114,
	_ui_error_demo_version_no_more_for_you = 0x115,
	_ui_error_no_fullscreen_res = 0x116,
	_ui_error_install_not_complete = 0x117,
	_ui_error_lan_fail_download_map = 0x118,
	_ui_error_locater_service_failed = 0x119,
	_ui_error_double_mapping_actions = 0x11A,
	_ui_error_no_multiplayer_achievements_for_silver = 0x11B,
	_ui_error_map_download_in_game = 0x11C,
	_ui_error_locator_service_timed_out = 0x11D,
	_ui_error_connection_to_host_lost = 0x11E,
	_ui_error_map_download_profane_name = 0x11F,
};

enum e_user_interface_channel_type
{
	_user_interface_channel_type_hardware_error = 0,
	_user_interface_channel_type_game_error,
	_user_interface_channel_type_keyboard,
	_user_interface_channel_type_dialog,
	_user_interface_channel_type_online_menu,
	_user_interface_channel_type_gameshell_screen,
	_user_interface_channel_type_gameshell_background,
	k_number_of_user_interface_channels
};

enum e_user_interface_render_window
{
	_window_0 = 0x0,
	_window_1 = 0x1,
	_window_2 = 0x2,
	_window_3 = 0x3,
	_window_4 = 0x4,

	k_number_of_render_windows = 4
};

/* forward declations*/

struct s_screen_parameters;
enum e_user_interface_screen_id : uint32;

/* structures */

struct s_screen_state
{
	int32 field_0;
	int32 m_last_focused_item_order;
	int32 m_last_focused_item_index;
};

typedef void* (__cdecl* proc_ui_screen_load_cb_t)(s_screen_parameters*);

struct s_screen_parameters
{
	uint16 m_flags;
	uint16 user_flags;
	e_user_interface_channel_type m_channel_type;
	e_user_interface_render_window m_window_index;
	void* m_context;
	s_screen_state m_screen_state;
	proc_ui_screen_load_cb_t m_load_function;

	void data_new(uint16 flags, uint16 user_flags, e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, proc_ui_screen_load_cb_t load_cb)
	{
		this->m_flags = flags;
		this->user_flags = user_flags;
		this->m_channel_type = channel_type;
		this->m_window_index = window_index;
		m_screen_state.field_0 = NONE;
		m_screen_state.m_last_focused_item_order = NONE;
		m_screen_state.m_last_focused_item_index = NONE;
		this->m_load_function = load_cb;
	}

	void* ui_screen_load_proc_exec()
	{
		return m_load_function(this);
	}
};
ASSERT_STRUCT_SIZE(s_screen_parameters, 0x20);


/* prototypes */

bool __cdecl user_interface_automation_is_active(void);
uint32 __cdecl user_interface_milliseconds(void);
bool __cdecl user_interface_error_display_allowed(void);
bool __cdecl user_interface_has_responding_controller(int32 user_index);
bool __cdecl user_interface_channel_is_busy(e_user_interface_channel_type channel_type);
bool __cdecl user_interface_back_out_from_channel_by_id(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, e_user_interface_screen_id id);
bool __cdecl user_interface_in_screen(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, e_user_interface_screen_id screen_id);
bool __cdecl user_interface_error_screen_is_active(e_user_interface_channel_type channel_index, e_user_interface_render_window window_index, e_ui_error_types error_code);

void __cdecl screen_error_ok_dialog_show(e_user_interface_channel_type channel_type, e_ui_error_types ui_error_index, e_user_interface_render_window window_index, uint16 user_flags, void* ok_callback, void* fallback);
void __cdecl screen_error_ok_dialog_with_custom_text(e_user_interface_channel_type channel_type, e_ui_error_types ui_error_index, e_user_interface_render_window window_index, uint16 user_flags, void* ok_callback, void* fallback, const wchar_t* custom_title, const wchar_t* custom_body);

void __cdecl user_interface_error_display_ok_cancel_dialog_with_ok_callback(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, uint16 user_flags, void* ok_callback_handle, e_ui_error_types error_type);
void __cdecl user_interface_back_out_from_channel(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index);
void __cdecl user_interface_enter_game_shell(int32 context);

void __cdecl render_menu_user_interface(int32 controller_index, e_user_interface_render_window render_window, rectangle2d* out_rect2d);

void __cdecl user_interface_return_to_mainmenu(bool a1);

uint32 user_interface_set_context_presence(e_context_presence game_mode);
