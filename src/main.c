#include "raylib.h"
#include <stdbool.h>

#include "player.h"
#include "board.h"

#define WIDTH 900
#define HEIGHT 600

int main(void)
{
    InitWindow(WIDTH, HEIGHT, "Ultimate Tic Tac Toe");
    SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));

    while (!WindowShouldClose())
    {
        // Update

        // Draw
        BeginDrawing();
        ClearBackground(BLACK);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
