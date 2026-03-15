#include "ultimate.h"

bool init_ultimate(UltimateBoard *ub)
{
    if (!ub)
        return false;

    for (int i = 0; i < 9; i++)
        init_board(&ub->locals[i]);

    ub->next = -1;
    ub->curr = X;

    return true;
}

Player ultimate_winner(UltimateBoard *ub)
{
    if (!ub)
        return NONE;

    const int wins[8][3] = {
        {0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {0, 3, 6}, {1, 4, 7}, {2, 5, 8}, {0, 4, 8}, {2, 4, 6}};

    for (int i = 0; i < 8; i++)
    {
        int a = wins[i][0];
        int b = wins[i][1];
        int c = wins[i][2];

        Player pa = ub->locals[a].winner;
        Player pb = ub->locals[b].winner;
        Player pc = ub->locals[c].winner;

        if (pa != NONE && pa == pb && pb == pc)
            return pa;
    }

    return NONE;
}

bool ultimate_full(UltimateBoard *ub)
{
    if (!ub)
        return false;

    for (int i = 0; i < 9; i++)
        if (board_playable(&ub->locals[i]))
            return false;

    return true;
}

bool ultimate_move(UltimateBoard *ub, int board_index, int cell)
{
    if (!ub)
        return false;

    if (board_index < 0 || board_index > 8)
        return false;

    if (cell < 0 || cell > 8)
        return false;

    if (ub->next != -1 && board_index != ub->next)
        return false;

    Board *b = &ub->locals[board_index];

    if (!board_playable(b))
        return false;

    if (!make_move(b, cell))
        return false;

    check_win(b);

    if (board_playable(&ub->locals[cell]))
        ub->next = cell;
    else
        ub->next = -1;

    ub->curr = ub->curr == X ? O : X;

    return true;
}