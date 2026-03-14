#include "raylib.h"

#define WIDTH  900
#define HEIGHT 600

int main(void) {
    InitWindow(WIDTH, HEIGHT, "ultimate-tic-tac-toe");
    SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));

    while (!WindowShouldClose()) {
        // Update
        
        // Draw
        BeginDrawing();
            ClearBackground(BLACK);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
