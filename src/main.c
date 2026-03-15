#include "raylib.h"
#include <stdbool.h>
#include <stddef.h>

#include "player.h"
#include "board.h"
#include "ultimate.h"

#define WINDOW_WIDTH 900
#define WINDOW_HEIGHT 900

#define BOARD_DIM 3
#define BOARD_CELLS (BOARD_DIM * BOARD_DIM)

#define GRID_DIM (BOARD_DIM * BOARD_DIM)

#define CELL_WIDTH (WINDOW_WIDTH / GRID_DIM)
#define CELL_HEIGHT (WINDOW_HEIGHT / GRID_DIM)

#define SUBBOARD_WIDTH (WINDOW_WIDTH / BOARD_DIM)
#define SUBBOARD_HEIGHT (WINDOW_HEIGHT / BOARD_DIM)

#define GRID_THICK 4
#define GRID_THIN 1

#define MARK_FONT_SIZE 100

#define NO_BOARD (-1)

void handle_input(UltimateBoard *game)
{
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mouse = GetMousePosition();

        int col = mouse.x / CELL_WIDTH;
        int row = mouse.y / CELL_HEIGHT;

        int big_col = col / BOARD_DIM;
        int big_row = row / BOARD_DIM;

        int small_col = col % BOARD_DIM;
        int small_row = row % BOARD_DIM;

        int board_index = big_row * BOARD_DIM + big_col;
        int cell_index = small_row * BOARD_DIM + small_col;

        ultimate_move(game, board_index, cell_index);
    }
}

void draw_grid(void)
{
    for (int i = 0; i <= GRID_DIM; i++)
    {
        int x = i * CELL_WIDTH;
        int y = i * CELL_HEIGHT;

        int thick = (i % BOARD_DIM == 0) ? GRID_THICK : GRID_THIN;

        DrawLineEx((Vector2){x, 0}, (Vector2){x, WINDOW_HEIGHT}, thick, WHITE);
        DrawLineEx((Vector2){0, y}, (Vector2){WINDOW_WIDTH, y}, thick, WHITE);
    }
}

void draw_marks(UltimateBoard *game)
{
    for (int b = 0; b < BOARD_CELLS; b++)
    {
        int bx = b % BOARD_DIM;
        int by = b / BOARD_DIM;

        Board board = game->locals[b];

        for (int c = 0; c < BOARD_CELLS; c++)
        {
            int cx = c % BOARD_DIM;
            int cy = c / BOARD_DIM;

            int col = bx * BOARD_DIM + cx;
            int row = by * BOARD_DIM + cy;

            int x = col * CELL_WIDTH;
            int y = row * CELL_HEIGHT;

            const char *text = NULL;

            if (board.cells[c] == X)
                text = "X";
            else if (board.cells[c] == O)
                text = "O";

            if (text)
            {
                int text_w = MeasureText(text, MARK_FONT_SIZE);

                int text_x = x + (CELL_WIDTH - text_w) / 2;
                int text_y = y + (CELL_HEIGHT - MARK_FONT_SIZE) / 2;

                DrawText(text, text_x, text_y, MARK_FONT_SIZE, board.cells[c] == X ? RED : BLUE);
            }
        }
    }
}

void draw_active_board(UltimateBoard *game)
{
    if (!game)
        return;

    if (game->next != NO_BOARD)
    {
        int bx = game->next % BOARD_DIM;
        int by = game->next / BOARD_DIM;

        Rectangle r = {
            bx * SUBBOARD_WIDTH,
            by * SUBBOARD_HEIGHT,
            SUBBOARD_WIDTH,
            SUBBOARD_HEIGHT};

        // DrawRectangleLinesEx(r, GRID_THICK, YELLOW);
        DrawRectangleRec(r, Fade(YELLOW, 0.15f));
    }
    else
    {
        for (int b = 0; b < BOARD_CELLS; b++)
        {
            if (!board_playable(&game->locals[b]))
                continue;

            int bx = b % BOARD_DIM;
            int by = b / BOARD_DIM;

            Rectangle r = {
                bx * SUBBOARD_WIDTH,
                by * SUBBOARD_HEIGHT,
                SUBBOARD_WIDTH,
                SUBBOARD_HEIGHT};

            // DrawRectangleLinesEx(r, GRID_THICK, YELLOW);
            DrawRectangleRec(r, Fade(YELLOW, 0.15f));
        }
    }
}

void draw(UltimateBoard *game)
{
    draw_grid();
    draw_marks(game);
    draw_active_board(game);
}

int main(void)
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Ultimate Tic Tac Toe");
    SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));

    UltimateBoard game;
    init_ultimate(&game);

    while (!WindowShouldClose())
    {
        handle_input(&game);

        BeginDrawing();
        ClearBackground(BLACK);
        draw(&game);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
