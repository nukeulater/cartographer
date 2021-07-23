#pragma once

#include "Blam/Engine/DataArray/DataArray.h"

struct s_game_state_objects
{
    static s_datum_array* getArray();
    static bool indexValid(datum objectDatum);

    template <typename T = void>
    static T* getObject(datum objectDatum)
    {
        if (!indexValid(objectDatum))
            return nullptr;
        return (T*)&getArray()->datum[objectDatum.ToAbsoluteIndex() * getArray()->datum_element_size];
    }
};