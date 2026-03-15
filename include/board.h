#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>

#include "player.h"

typedef struct
{
    Player cells[9]; // row major order
    Player curr;
    Player winner;
} Board;

bool init_board(Board *);
bool board_full(Board *);
void check_win(Board *);
void get_occupancy(Board *, bool *);
bool board_playable(Board *);
bool make_move(Board *, int cell);

#endif