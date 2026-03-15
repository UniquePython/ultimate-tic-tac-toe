#include "save.h"

#include <stdio.h>

bool save_exists(void)
{
    FILE *f = fopen(SAVE_PATH, "rb");
    if (!f)
        return false;
    fclose(f);
    return true;
}

bool save_game(const UltimateBoard *ub)
{
    if (!ub)
        return false;

    FILE *f = fopen(SAVE_PATH, "wb");
    if (!f)
        return false;

    bool ok = fwrite(ub, sizeof(UltimateBoard), 1, f) == 1;
    fclose(f);
    return ok;
}

bool load_game(UltimateBoard *ub)
{
    if (!ub)
        return false;

    FILE *f = fopen(SAVE_PATH, "rb");
    if (!f)
        return false;

    bool ok = fread(ub, sizeof(UltimateBoard), 1, f) == 1;
    fclose(f);
    return ok;
}