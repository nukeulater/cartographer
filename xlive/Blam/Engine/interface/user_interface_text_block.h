#pragma once
#include "user_interface_text.h"

/* classes */

class c_small_user_interface_text : public c_user_interface_text
{
protected:
	wchar_t m_string[32];

public:
	c_small_user_interface_text(void);

	// c_small_user_interface_text virtual functions

	virtual ~c_small_user_interface_text(void) = default;
	virtual void  set_text(const wchar_t* initial_text) override;
	virtual void  append_text(const wchar_t* update_text) override;
	virtual const wchar_t* get_raw_string(void) override;
};
ASSERT_STRUCT_SIZE(c_small_user_interface_text, 132);

class c_normal_user_interface_text : public c_user_interface_text
{
protected:
	wchar_t m_string[512];

public:
	c_normal_user_interface_text(void);

	// c_normal_user_interface_text virtual functions

	virtual ~c_normal_user_interface_text(void) = default;
	virtual void  set_text(const wchar_t* initial_text) override;
	virtual void  append_text(const wchar_t* update_text) override;
	virtual const wchar_t* get_raw_string(void) override;
};
ASSERT_STRUCT_SIZE(c_normal_user_interface_text, 1092);

class c_long_user_interface_text : public c_user_interface_text
{
protected:
	wchar_t m_string[1024];

public:
	c_long_user_interface_text(void);

	// c_normal_user_interface_text virtual functions

	virtual ~c_long_user_interface_text(void) = default;
	virtual void  set_text(const wchar_t* initial_text) override;
	virtual void  append_text(const wchar_t* update_text) override;
	virtual const wchar_t* get_raw_string(void) override;
};
ASSERT_STRUCT_SIZE(c_long_user_interface_text, 2116);