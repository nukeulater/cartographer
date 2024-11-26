#pragma once
#include "tag_files/tag_groups.h"

namespace tags
{
	struct tag_instance
	{
		tag_group type;
		datum datum_index;
		size_t data_offset;
		size_t size;
	};
	ASSERT_STRUCT_SIZE(tag_instance, 16);
	
	/*
		Tag Interface

		These functions shouldn't be called while a new cache is being loaded and as such it's not recommended you call them from any thread other than the main one.
		If you want to run code just after a map load register a callback using tags::on_map_load
	*/

	/* tag data in currently loaded map (merged cache and shared cache data afaik) */
	uint8* get_tag_data();

	/* Returns a handle to the map file currently loaded */
	HANDLE get_cache_handle();

	/* Is a cache loaded? */
	bool cache_file_loaded();

	/* helper function for getting a pointer to data at offset in tag data */
	template <typename T>
	T* get_at_tag_data_offset(size_t offset)
	{
		return reinterpret_cast<T*>(&get_tag_data()[offset]);
	}

	/* Returns a pointer to the tag instance array */
	tag_instance* get_tag_instance(datum tag_index);
}
