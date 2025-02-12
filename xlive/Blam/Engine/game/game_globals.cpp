#include "stdafx.h"
#include "game_globals.h"

#include "units/unit_definitions.h"

#include "models/models.h"
#include "scenario/scenario.h"
#include "tag_files/global_string_ids.h"
#include "cache/cache_files.h"

#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/Modules/SpecialEvents/SpecialEvents.h"
#include "tag_files/tag_loader/tag_injection.h"

/* structures */

struct s_game_globals_custom_representation_result
{
	bool success;
	e_character_type fallback_character_type;
	datum third_person_unit;
	datum first_person_hands;
	datum first_person_body;
	string_id variant;
};

/* prototypes */

// Set the masterchief representation to the multiplayer version only in multiplayer
// This prevents server owners from forcing masterchief 
static void game_globals_remove_singleplayer_representation(void);

// Adds new representations to the globals tag
static void game_globals_add_new_player_representations(void);

static void game_globals_prepare_skeleton_representation(s_game_globals_custom_representation_result* result);
static void game_globals_prepare_flood_representation(s_game_globals_custom_representation_result* result);
static void game_globals_prepare_lmao_representation(s_game_globals_custom_representation_result* result);

static void add_new_representations(s_game_globals_custom_representation_result* representations);
static void add_simulation_table_entries(s_game_globals_custom_representation_result* representations);

/* constants */

void (*k_game_globals_custom_representation_function_table[k_cartographer_custom_representation_count])(s_game_globals_custom_representation_result*)
{
	game_globals_prepare_skeleton_representation,
	game_globals_prepare_flood_representation,
	game_globals_prepare_lmao_representation
};

/* public code */

void game_globals_apply_tag_patches(void)
{
	game_globals_add_new_player_representations();
	game_globals_remove_singleplayer_representation();
	return;
}

s_game_globals* scenario_get_game_globals(void)
{
	return *Memory::GetAddress<s_game_globals**>(0x479E70, 0x4A642C);
}

s_camera_track_definition* game_globals_get_default_camera_track()
{
	s_game_globals* game_globals = scenario_get_game_globals();

	return (s_camera_track_definition*)tag_get_fast(game_globals->camera[0]->default_unit_camera_track.index);
}

void scenario_set_game_globals(s_game_globals* globals)
{
	*Memory::GetAddress<s_game_globals**>(0x479E70, 0x4A642C) = globals;
}

s_ui_levels_definition* game_globals_get_ui_levels(void)
{
	s_game_globals* globals = scenario_get_game_globals();
	if (globals->ui_level_data.count > 0 && globals->ui_level_data.data != NONE)
	{
		return globals->ui_level_data[0];
	}

	return NULL;
}

s_game_globals_player_representation* game_globals_get_representation(e_character_type type)
{
	return scenario_get_game_globals()->player_representation[type];
}

/* private code */

static void game_globals_remove_singleplayer_representation(void)
{
	s_game_globals* globals = scenario_get_game_globals();

	s_game_globals_player_representation* masterchief_rep = globals->player_representation[_character_type_masterchief];
	s_game_globals_player_representation* spartan_rep = globals->player_representation[_character_type_spartan];

	masterchief_rep->third_person_unit = spartan_rep->third_person_unit;
	return;
}

static void game_globals_add_new_player_representations(void)
{
	if (!Memory::IsDedicatedServer())
	{
		scenario* scenario_definition = global_scenario_get();

		s_game_globals_custom_representation_result representations[k_cartographer_custom_representation_count]{};
		for (uint32 i = 0; i < k_cartographer_custom_representation_count; ++i)
		{
			ASSERT(k_game_globals_custom_representation_function_table[i]);
			k_game_globals_custom_representation_function_table[i](&representations[i]);
		}

		add_new_representations(representations);
		add_simulation_table_entries(representations);
	}
	return;
}

static void game_globals_prepare_skeleton_representation(s_game_globals_custom_representation_result* result)
{
	result->success = false;
	result->fallback_character_type = _character_type_spartan;

	tag_injection_set_active_map(k_carto_shared_map);
	datum skele_datum = tag_injection_load(_tag_group_biped, "objects\\characters\\masterchief_skeleton\\masterchief_skeleton", true);
	datum skele_fp_datum = tag_injection_load(_tag_group_render_model, "objects\\characters\\masterchief_skeleton\\fp\\fp", true);
	datum skele_body_datum = tag_injection_load(_tag_group_render_model, "objects\\characters\\masterchief_skeleton\\fp_body\\fp_body", true);

	if (skele_datum != NONE && skele_fp_datum != NONE && skele_body_datum != NONE && get_current_special_event() == _special_event_halloween && !H2Config_no_events)
	{
		tag_injection_inject();

		result->success = true;
		result->third_person_unit = skele_datum;
		result->first_person_hands = skele_fp_datum;
		result->first_person_body = skele_body_datum;
		result->variant = NONE;
	}
	return;
}

static void game_globals_prepare_flood_representation(s_game_globals_custom_representation_result* result)
{
	result->success = false;
	result->fallback_character_type = _character_type_elite;

	tag_injection_set_active_map(k_carto_shared_map);

	datum flood_datum = tag_injection_load(_tag_group_biped, "objects\\characters\\floodcombat_elite\\floodcombat_elite_mp", true);
	datum flood_arms_datum = tag_injection_load(_tag_group_render_model, "objects\\characters\\flood_mp\\fp_arms\\fp_arms", true);
	datum flood_body_datum = tag_injection_load(_tag_group_render_model, "objects\\characters\\flood_mp\\fp_body\\fp_body", true);

	if (flood_datum != NONE && flood_arms_datum != NONE && flood_body_datum != NONE)
	{
		tag_injection_inject();

		result->success = true;
		result->third_person_unit = flood_datum;
		result->first_person_hands = flood_arms_datum;
		result->first_person_body = flood_body_datum;
		result->variant = NONE;
	}
	return;
}

static void game_globals_prepare_lmao_representation(s_game_globals_custom_representation_result* result)
{
	result->success = false;
	result->fallback_character_type = _character_type_spartan;

	// Create copy of default variant for chief and add lmao object to head
	unit_definition* mp_chief_unit = (unit_definition*)tag_get_fast(game_globals_get_representation(_character_type_spartan)->third_person_unit.index);
	datum mode_chief_mp_datum = mp_chief_unit->object.model.index;
	if (mode_chief_mp_datum != NONE)
	{
		// Copy the variant
		s_model_definition* mode_chief_mp = (s_model_definition*)tag_get_fast(mode_chief_mp_datum);
		s_model_variant* base_variant = mode_chief_mp->variants[0];
		s_model_variant* new_variant = (s_model_variant*)tag_injection_extend_block(&mode_chief_mp->variants, mode_chief_mp->variants.type_size(), 1);
		new_variant->name = 0xABABABA;
		new_variant->dialogue.group = base_variant->dialogue.group;
		new_variant->dialogue.index = base_variant->dialogue.index;
		csmemcpy(new_variant->runtime_model_region_index, base_variant->runtime_model_region_index, sizeof(new_variant->runtime_model_region_index));

		s_model_variant_region* region_blocks = (s_model_variant_region*)tag_injection_extend_block(&new_variant->regions, new_variant->regions.type_size(), base_variant->regions.count);
		for (auto i = 0; i < base_variant->regions.count; i++)
		{
			s_model_variant_region* region = base_variant->regions[i];
			s_model_variant_region* new_region = &region_blocks[i];

			new_region->region_name = region->region_name;
			new_region->runtime_model_region_index = region->runtime_model_region_index;
			new_region->runtime_flags = region->runtime_flags;
			new_region->parent_variant = region->parent_variant;
			new_region->sort_order = region->sort_order;

			s_model_variant_permutation* permutation_blocks = (s_model_variant_permutation*)tag_injection_extend_block(&new_region->permutations, new_region->permutations.type_size(), region->permutations.count);
			for (auto k = 0; k < region->permutations.count; k++)
			{
				s_model_variant_permutation* permutation = region->permutations[k];
				s_model_variant_permutation* new_permutation = &permutation_blocks[k];

				new_permutation->permutation_name = permutation->permutation_name;
				new_permutation->runtime_model_permutation_index = permutation->runtime_model_permutation_index;
				new_permutation->flags = permutation->flags;
				new_permutation->probability = permutation->probability;
				csmemcpy(new_permutation->runtime_state_permutation_index, permutation->runtime_state_permutation_index, sizeof(new_permutation->runtime_state_permutation_index));
				csmemcpy(new_permutation->pad1, permutation->pad1, sizeof(new_permutation->pad1));
			}
		}

		tag_injection_set_active_map(k_carto_shared_map);
		// Add lmao head as an attachment on the new variant
		datum lmao_datum = tag_injection_load(_tag_group_scenery, "scenarios\\objects\\multi\\carto_shared\\emoji_head\\emoji_head", true);
		if (lmao_datum != NONE)
		{
			tag_injection_inject();
			s_model_variant_object* new_object = (s_model_variant_object*)tag_injection_extend_block(&new_variant->objects, new_variant->objects.type_size(), 1);
			new_object->parent_marker = _string_id_head;
			new_object->child_object.group.group = _tag_group_scenery;
			new_object->child_object.index = lmao_datum;

		}

		result->success = true;
		result->third_person_unit = NONE;
		result->first_person_hands = NONE;
		result->first_person_body = NONE;
		result->variant = new_variant->name;
	}
	return;
}

static void add_new_representations(s_game_globals_custom_representation_result* representations)
{
	s_game_globals* globals = scenario_get_game_globals();
	s_game_globals_player_representation* new_blocks = (s_game_globals_player_representation*)tag_injection_extend_block(&globals->player_representation, globals->player_representation.type_size(), k_cartographer_custom_representation_count);

	for (uint32 index = 0; index < k_cartographer_custom_representation_count; index++)
	{
		const s_game_globals_custom_representation_result* representation_result = &representations[index];
		const e_character_type fallback_character = representation_result->fallback_character_type;

		s_game_globals_player_representation* new_representation = &new_blocks[index];

		if (representation_result->success)
		{
			const tag_reference fp_hands_tag_ref = { _tag_group_render_model, representation_result->first_person_hands };
			const tag_reference fp_body_tag_ref = { _tag_group_render_model, representation_result->first_person_body };
			const tag_reference unit_tag_ref = { _tag_group_biped, representation_result->third_person_unit };

			new_representation->first_person_hands = representation_result->first_person_hands != NONE ? fp_hands_tag_ref : globals->player_representation[fallback_character]->first_person_hands;
			new_representation->first_person_body = representation_result->first_person_body != NONE ? fp_body_tag_ref : globals->player_representation[fallback_character]->first_person_body;
			new_representation->third_person_unit = representation_result->third_person_unit != NONE ? unit_tag_ref : globals->player_representation[fallback_character]->third_person_unit;
			new_representation->third_person_variant = representation_result->variant != NONE ? representation_result->variant : globals->player_representation[fallback_character]->third_person_variant;
		}
		else
		{
			new_representation->first_person_hands = globals->player_representation[fallback_character]->first_person_hands;
			new_representation->first_person_body = globals->player_representation[fallback_character]->first_person_body;
			new_representation->third_person_unit = globals->player_representation[fallback_character]->third_person_unit;
			new_representation->third_person_variant = globals->player_representation[fallback_character]->third_person_variant;
		}
	}
	return;
}

static void add_simulation_table_entries(s_game_globals_custom_representation_result* representations)
{
	scenario* scenario = global_scenario_get();

	// do an initial loop through the representation results to determine which ones need new blocks added
	uint32 new_entry_count = 0;

	for(uint32 i = 0; i < k_cartographer_custom_representation_count; ++i)
	{
		if (representations[i].success && representations[i].third_person_unit != NONE)
			++new_entry_count;
	}

	// do a second pass to place them into the new table
	s_scenario_simulation_definition_table_element* new_blocks = (s_scenario_simulation_definition_table_element*)tag_injection_extend_block(&scenario->simulation_definition_table, scenario->simulation_definition_table.type_size(), new_entry_count);
	new_entry_count = 0;

	for(uint32 i = 0; i < k_cartographer_custom_representation_count; ++i)
	{
		if(representations[i].success && representations[i].third_person_unit != NONE)
		{
			new_blocks[new_entry_count].tag_datum = representations[i].third_person_unit;
			++new_entry_count;
		}
	}
	return;
}
