#include "stdafx.h"
#include "SpecialEventHelpers.h"

#include "cache/cache_files.h"
#include "items/weapon_definitions.h"
#include "models/models.h"
#include "models/render_model_definitions.h"

#include "tag_files/tag_loader/tag_injection.h"

void replace_fp_and_3p_models_from_weapon(datum weapon_datum, datum fp_model_datum, datum _3p_model_datum)
{
	weapon_definition* weapon = (weapon_definition*)tag_get_fast(weapon_datum);
	weapon->object.sweetener_size = _sweetener_size_medium;
	weapon->weapon.player_interface.first_person[0]->model.index = fp_model_datum;
	weapon->weapon.player_interface.first_person[1]->model.index = fp_model_datum;

	datum model_datum = weapon->object.model.index;
	s_model_definition* model = (s_model_definition*)tag_get_fast(model_datum);
	model->render_model.index = _3p_model_datum;
	return;
}

