#include "board.h"

bool init_board(Board *board)
{
    if (!board)
        return false;

    for (int i = 0; i < 9; i++)
        board->cells[i] = NONE;

    board->curr = X;
    board->winner = NONE;

    return true;
}

bool board_full(Board *board)
{
    if (!board)
        return false;

    for (int i = 0; i < 9; i++)
        if (board->cells[i] == NONE)
            return false;

    return true;
}

void check_win(Board *board)
{
    if (!board)
        return;

    const int wins[8][3] = {
        {0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {0, 3, 6}, {1, 4, 7}, {2, 5, 8}, {0, 4, 8}, {2, 4, 6}};

    for (int i = 0; i < 8; i++)
    {
        int a = wins[i][0];
        int b = wins[i][1];
        int c = wins[i][2];

        if (board->cells[a] != NONE &&
            board->cells[a] == board->cells[b] &&
            board->cells[b] == board->cells[c])
        {
            board->winner = board->cells[a];
            return;
        }
    }

    board->winner = NONE;
}

void get_occupancy(Board *board, bool *vacant)
{
    if (!board || !vacant)
        return;

    for (int i = 0; i < 9; i++)
        vacant[i] = board->cells[i] == NONE;
}

bool board_playable(Board *b)
{
    if (!b)
        return false;

    if (b->winner != NONE)
        return false;

    return !board_full(b);
}

bool make_move(Board *board, int cell)
{
    if (!board || cell < 0 || cell > 8)
        return false;

    if (board->winner != NONE)
        return false;

    if (board->cells[cell] != NONE)
        return false;

    board->cells[cell] = board->curr;

    check_win(board);

    if (board->winner == NONE)
        board->curr = board->curr == X ? O : X;

    return true;
}