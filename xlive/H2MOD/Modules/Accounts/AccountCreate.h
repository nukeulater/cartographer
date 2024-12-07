#pragma once

#include "interface/screens/screen_cartographer_errors.h"

bool HandleGuiAccountCreate(char* username, char* email, char* password, e_cartographer_error_id* out_account_create_cartographer_error_id);
