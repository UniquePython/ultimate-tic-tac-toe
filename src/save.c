#include "save.h"

#include <stdio.h>

void get_save_path(const char *dir, int slot, char *out, int outlen)
{
    snprintf(out, outlen, "%s/save_%d.dat", dir, slot);
}

bool save_exists(const char *dir, int slot)
{
    char path[MAX_PATH_LEN];
    get_save_path(dir, slot, path, sizeof(path));

    FILE *f = fopen(path, "rb");
    if (!f)
        return false;
    fclose(f);
    return true;
}

bool save_game(const UltimateBoard *ub, const char *dir, int slot)
{
    if (!ub)
        return false;

    char path[MAX_PATH_LEN];
    get_save_path(dir, slot, path, sizeof(path));

    FILE *f = fopen(path, "wb");
    if (!f)
        return false;

    bool ok = fwrite(ub, sizeof(UltimateBoard), 1, f) == 1;
    fclose(f);
    return ok;
}

bool load_game(UltimateBoard *ub, const char *dir, int slot)
{
    if (!ub)
        return false;

    char path[MAX_PATH_LEN];
    get_save_path(dir, slot, path, sizeof(path));

    FILE *f = fopen(path, "rb");
    if (!f)
        return false;

    bool ok = fread(ub, sizeof(UltimateBoard), 1, f) == 1;
    fclose(f);
    return ok;
}