#ifndef ULTIMATE_H
#define ULTIMATE_H

#include "player.h"
#include "board.h"

typedef struct
{
    Board locals[9]; // row major order
    int next;        // -1 means any board allowed
    Player curr;
} UltimateBoard;

bool init_ultimate(UltimateBoard *);
Player ultimate_winner(UltimateBoard *);
bool ultimate_full(UltimateBoard *);
bool ultimate_move(UltimateBoard *, int, int);

#endif