#include "stdafx.h"

#include "rasterizer_memory.h"

uint32 rasterizer_pool_allocate(int32 type, int32 element_count, int32 element_size)
{
	return INVOKE(0x27D0FF, 0x0, rasterizer_pool_allocate, type, element_count, element_size);
}

void* rasterizer_pool_get_from_offset(uint32 offset)
{
	return INVOKE(0x27D188, 0x0, rasterizer_pool_get_from_offset, offset);
}