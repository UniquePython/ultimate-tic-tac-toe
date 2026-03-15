#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>

#include "player.h"

typedef struct
{
    Player cells[9]; // row major order
    Player winner;
} Board;

bool init_board(Board *);
bool board_full(Board *);
void check_win(Board *);
bool board_playable(Board *);
bool make_move(Board *, int cell, Player curr);

#endif