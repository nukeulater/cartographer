#include "stdafx.h"
#include "cseries.h"

#ifdef ASSERTS_ENABLED
#include "shell/shell.h"
#endif

/* constants */

enum
{
	MAXIMUM_MEMCPY_SIZE = 0x20000000,
	MAXIMUM_MEMMOVE_SIZE = 0x20000000,
	MAXIMUM_MEMSET_SIZE = 0x20000000
};

/* globals */

// TODO: figure out a decent way to strip this from release builds
char g_temporary[256] = {};

#ifdef ASSERTS_ENABLED
bool g_catch_exceptions = true;
#endif

/* macros */

#ifdef ASSERTS_ENABLED
#define CSERIES_ASSERT(STATEMENT)	\
if (!(STATEMENT))					\
{									\
	if (!is_debugger_present())		\
	{								\
		error(3, "");				\
		/* TODO: stalk walk call here */	\
	}								\
	error(3, "");					\
	if (is_debugger_present())		\
	{								\
		error(3, "%s(%d): %s: %s", __FILE__, __LINE__, "ASSERT", #STATEMENT);	\
	}											\
	else										\
	{											\
		error(3, "%s", shell_get_version());	\
		error(3, "%s at %s,#%d", "### ASSERTION FAILED: ", __FILE__, __LINE__);	\
		error(3, "  %s", ##STATEMENT);			\
	}											\
	/* TODO: error callback call here */		\
	if (!is_debugger_present())					\
	{											\
		RaiseException(0x73746Bu, 0, 0, 0);		\
		/* halt_and_catch_fire(); */			\
		/* unk_cseries_windows_call(-1, ##STATEMENT); */		\
	}											\
	ASSERT_TRIGGER_EXCEPTION();					\
}												\
(void)0
#else
#define CSERIES_ASSERT(STATEMENT) (void)0
#endif

/* public code */

void display_assert(char const* condition, char const* file, int32 line, bool assertion_failed)
{
	return;
}

void* csmemmove(void* destination, void* source, size_t size)
{
	CSERIES_ASSERT(size == 0 || (destination && source));
	CSERIES_ASSERT(size >= 0 && size <= MAXIMUM_MEMMOVE_SIZE);
	return memmove(destination, source, size);
}

void* csmemset(void* destination, int32 val, size_t size)
{
	CSERIES_ASSERT(size == 0 || destination);
	CSERIES_ASSERT(size >= 0 && size <= MAXIMUM_MEMSET_SIZE);
	return memset(destination, val, size);
}

void* csmemcpy(void* destination, const void* source, size_t size)
{
	CSERIES_ASSERT(size == 0 || (destination && source));
	CSERIES_ASSERT(size >= 0 && size < MAXIMUM_MEMCPY_SIZE);
	ASSERT((byte*)source + size <= (byte*)destination || (byte*)destination + size <= (byte*)source);

	return memcpy(destination, source, size);
}

int32 vsprintf(char* buffer, size_t size, const char* format, va_list va_args)
{
	CSERIES_ASSERT(buffer);
	CSERIES_ASSERT(format);
	CSERIES_ASSERT(size > 0);

	const int32 result = (int32)_vsnprintf_s(buffer, size, _TRUNCATE, format, va_args);
	return result;
}

int32 vsnprintf(char* buffer, size_t size, size_t max_count, const char* format, va_list va_args)
{
	CSERIES_ASSERT(buffer);
	CSERIES_ASSERT(format);
	CSERIES_ASSERT(size > 0);

	const int32 result = (int32)_vsnprintf_s(buffer, size, max_count, format, va_args);
	return result;
}

const char* csprintf(char* buffer, size_t size, const char* format, ...)
{
	va_list va_args;
	va_start(va_args, format);
	(void)vsprintf(buffer, size, format, va_args);
	va_end(va_args);
	return buffer;
}

const char* csnprintf(char* buffer, size_t size, size_t max_count, const char* format, ...)
{
	va_list va_args;
	va_start(va_args, format);
	(void)vsnprintf(buffer, size, max_count, format, va_args);
	va_end(va_args);
	return buffer;
}
