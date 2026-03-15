#ifndef SAVE_H
#define SAVE_H

#include <stdbool.h>
#include "ultimate.h"

#define SAVE_PATH "save.dat"

bool save_exists(void);
bool save_game(const UltimateBoard *ub);
bool load_game(UltimateBoard *ub);

#endif