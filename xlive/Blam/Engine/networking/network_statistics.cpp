#include "stdafx.h"

#include "network_statistics.h"

/* constants */

/* declarations */


/* globals */

/* public code */

real32 c_network_window_statistics::average_values_in_window()
{
	return (real32)field_108 / (real32)m_window_size;
}