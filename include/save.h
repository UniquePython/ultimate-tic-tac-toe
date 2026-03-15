#ifndef SAVE_H
#define SAVE_H

#include <stdbool.h>
#include "ultimate.h"

#define MAX_SLOTS 3
#define MAX_PATH_LEN 512

void get_save_path(const char *dir, int slot, char *out, int outlen);
bool save_exists(const char *dir, int slot);
bool save_game(const UltimateBoard *ub, const char *dir, int slot);
bool load_game(UltimateBoard *ub, const char *dir, int slot);

#endif