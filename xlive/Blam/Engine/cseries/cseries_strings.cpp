#include "stdafx.h"
#include "cseries_strings.h"

size_t csstrnlen(const char* s, size_t size)
{
	ASSERT(s);
	ASSERT(size > 0 && size <= MAXIMUM_STRING_SIZE);
	return strnlen(s, size);
}

char* csstrnzcpy(char* s1, const char* s2, size_t size)
{
	ASSERT(s1 && s2);
	ASSERT(size > 0 && size <= MAXIMUM_STRING_SIZE);
	strncpy_s(s1, size, s2, UINT_MAX);
	return s1;
}

char* csstrnzcat(char* s1, char const* s2, size_t size)
{
	ASSERT(s1 && s2);
	ASSERT(size > 0 && size <= MAXIMUM_STRING_SIZE);
	return strncat(s1, s2, size);
}

int32 csstricmp(const char* s1, const char* s2)
{
	ASSERT(s1 && s2);
	return strcmp(s1, s2);
}

int32 csstrncmp(const char* s1, const char* s2, size_t size)
{
	// TODO: add asserts and logging
	return strncmp(s1, s2, size);
}
