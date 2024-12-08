#pragma once

/* english */

const wchar_t k_username_string_eng[] = L"[Username]";
const wchar_t k_password_string_eng[] = L"[Password]";
const wchar_t k_password_confirm_string_eng[] = L"[Confirm Password]";
const wchar_t k_remember_me_string_eng[] = L"%c Remember me";

const wchar_t* k_screen_type_list_add_account_eng[] =
{
	L"Add account",
	L"Enter your account's Username\r\n[or Email] and Password to Login.",
	k_username_string_eng,
	k_password_string_eng,
	k_remember_me_string_eng,
	L"Login",
	L"<unnamed>"
	L"<unknown label>",
};

static const wchar_t* k_screen_type_list_table_eng[] =
{
	L"Online Accounts",
	L"Select an Account to Sign in to or use options to create/add/remove them.",
	L">Play Offline",
	L">Create Account",
	L">Add Account",
	L"-Remove Account",
	L"-Cancel Remove",
	L"<unknown label>"
};

static const wchar_t* k_screen_type_list_create_account_eng[] =
{
	L"Create Account",
	L"Enter a username, email and password for your new account.",
	k_username_string_eng,
	L"[Email]",
	k_password_string_eng,
	k_password_confirm_string_eng,
	L"Create Account",
	L"<unknown label>"
};
