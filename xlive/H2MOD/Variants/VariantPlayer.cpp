#include "stdafx.h"
#include "Globals.h"
#include "VariantPlayer.h"

XUID VariantPlayer::GetXUID(datum _datum, bool player)
{
	DatumIterator<s_object_header> objectIt(game_state_objects_header);
	s_biped_data_definition* playerUnit = (s_biped_data_definition*)objectIt.get_data_at_index(_datum.ToAbsoluteIndex())->object;

	if (player)
		return Player::getPlayer(_datum.ToAbsoluteIndex())->identifier;
	else
	{
		short player_index = playerUnit->controlling_player_index;
		return Player::getPlayer(player_index)->identifier;
	}
}

datum VariantPlayer::GetPlayerDatum(datum unit_datum)
{
	DatumIterator<s_object_header> objectIt(game_state_objects_header);
	s_biped_data_definition* playerUnit = (s_biped_data_definition*)objectIt.get_data_at_index(unit_datum.ToAbsoluteIndex())->object;

	return playerUnit->controlling_player_index;
}

datum VariantPlayer::GetPlayerDatum(XUID xuid)
{
	if (xuid_to_player_datum.find(xuid) == xuid_to_player_datum.end())
	{
		PlayerIterator playersIt;
		while (playersIt.get_next_active_player())
		{
			if (playersIt.get_current_player_data()->identifier == xuid)
			{
				datum player_datum;
				player_datum.Index = playersIt.get_current_player_index();

				SetPlayerDatum(xuid, player_datum);
				return player_datum;
			}
		}
	}
	else
		return xuid_to_player_datum[xuid];


	return -1;
}

datum VariantPlayer::GetUnitDatum(datum player_datum)
{
	PlayerIterator playersIt;
	return playersIt.get_data_at_index(player_datum.Index)->controlled_unit_index;
}

datum VariantPlayer::GetUnitDatum(XUID xuid)
{
	if (xuid_to_unit_datum.find(xuid) == xuid_to_unit_datum.end())
	{
		PlayerIterator playersIt;
		while (playersIt.get_next_active_player())
		{
			if (playersIt.get_current_player_data()->identifier == xuid)
			{
				datum unit_datum = playersIt.get_current_player_data()->controlled_unit_index;
				datum player_datum;
				player_datum.Index = playersIt.get_current_player_index();

				SetPlayerDatum(xuid, player_datum);
				SetUnitDatum(xuid, unit_datum);

				return unit_datum;
			}
		}
	}
	else
		return xuid_to_unit_datum[xuid];


	return -1; // avoid warning.
}

void VariantPlayer::SetPlayerDatum(XUID xuid, datum player_datum)
{
	xuid_to_player_datum[xuid] = player_datum;
}

void VariantPlayer::SetUnitDatum(XUID xuid, datum unit_datum)
{
	xuid_to_unit_datum[xuid] = unit_datum;
}

void VariantPlayer::Deinitialize()
{
	xuid_to_player_datum.clear();
	xuid_to_unit_datum.clear();
}