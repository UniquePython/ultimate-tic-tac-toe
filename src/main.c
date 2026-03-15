#include "raylib.h"
#include <stdbool.h>
#include <stddef.h>

#include "player.h"
#include "board.h"
#include "ultimate.h"
#include "save.h"

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
#define OVERLAY_FONT_SIZE 80
#define TITLE_FONT_SIZE 72
#define BUTTON_FONT_SIZE 36
#define HINT_FONT_SIZE 24

// How long the "Saved!" confirmation stays on screen (in seconds)
#define SAVE_MSG_DURATION 1.5f

// ---- State ----

typedef enum
{
    SCREEN_MENU,
    SCREEN_PLAYING,
    SCREEN_GAME_OVER
} Screen;

// ---- Button ----

typedef struct
{
    Rectangle rect;
    const char *label;
    bool disabled;
} Button;

static bool button_clicked(Button btn)
{
    if (btn.disabled)
        return false;
    Vector2 mouse = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mouse, btn.rect);
    return hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

static void draw_button(Button btn)
{
    Vector2 mouse = GetMousePosition();
    bool hovered = !btn.disabled && CheckCollisionPointRec(mouse, btn.rect);

    Color fill = btn.disabled ? Fade(WHITE, 0.03f) : hovered ? Fade(WHITE, 0.2f)
                                                             : Fade(WHITE, 0.08f);
    Color border = btn.disabled ? Fade(GRAY, 0.3f) : hovered ? WHITE
                                                             : GRAY;
    Color text_c = btn.disabled ? Fade(GRAY, 0.4f) : hovered ? WHITE
                                                             : LIGHTGRAY;

    DrawRectangleRec(btn.rect, fill);
    DrawRectangleLinesEx(btn.rect, 2, border);

    int text_w = MeasureText(btn.label, BUTTON_FONT_SIZE);
    int text_x = btn.rect.x + (btn.rect.width - text_w) / 2;
    int text_y = btn.rect.y + (btn.rect.height - BUTTON_FONT_SIZE) / 2;

    DrawText(btn.label, text_x, text_y, BUTTON_FONT_SIZE, text_c);
}

static Button make_centered_button(int y, int w, int h, const char *label, bool disabled)
{
    return (Button){
        .rect = {(WINDOW_WIDTH - w) / 2, y, w, h},
        .label = label,
        .disabled = disabled};
}

// ---- Menu ----

static void update_menu(Screen *screen, UltimateBoard *game)
{
    Button play = make_centered_button(460, 300, 64, "New Game", false);
    Button load = make_centered_button(550, 300, 64, "Load Game", !save_exists());
    Button quit = make_centered_button(640, 300, 64, "Quit", false);

    if (button_clicked(play))
    {
        init_ultimate(game);
        *screen = SCREEN_PLAYING;
    }

    if (button_clicked(load) && load_game(game))
        *screen = SCREEN_PLAYING;

    if (button_clicked(quit))
        CloseWindow();
}

static void draw_menu(void)
{
    const char *title = "Ultimate";
    const char *subtitle = "Tic Tac Toe";

    int title_w = MeasureText(title, TITLE_FONT_SIZE);
    int subtitle_w = MeasureText(subtitle, TITLE_FONT_SIZE);

    DrawText(title, (WINDOW_WIDTH - title_w) / 2, 260, TITLE_FONT_SIZE, WHITE);
    DrawText(subtitle, (WINDOW_WIDTH - subtitle_w) / 2, 345, TITLE_FONT_SIZE, GRAY);

    draw_button(make_centered_button(460, 300, 64, "New Game", false));
    draw_button(make_centered_button(550, 300, 64, "Load Game", !save_exists()));
    draw_button(make_centered_button(640, 300, 64, "Quit", false));
}

// ---- Game ----

static void handle_input(UltimateBoard *game, float *save_timer)
{
    // Save on S key
    if (IsKeyPressed(KEY_S))
    {
        if (save_game(game))
            *save_timer = SAVE_MSG_DURATION;
    }

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

static void draw_grid(void)
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

static void draw_marks(UltimateBoard *game)
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

                DrawText(text, text_x, text_y, MARK_FONT_SIZE,
                         board.cells[c] == X ? RED : BLUE);
            }
        }
    }
}

static void draw_active_board(UltimateBoard *game)
{
    if (!game)
        return;

    if (game->next != NO_BOARD)
    {
        int bx = game->next % BOARD_DIM;
        int by = game->next / BOARD_DIM;

        Rectangle r = {bx * SUBBOARD_WIDTH, by * SUBBOARD_HEIGHT, SUBBOARD_WIDTH, SUBBOARD_HEIGHT};
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

            Rectangle r = {bx * SUBBOARD_WIDTH, by * SUBBOARD_HEIGHT, SUBBOARD_WIDTH, SUBBOARD_HEIGHT};
            DrawRectangleRec(r, Fade(YELLOW, 0.15f));
        }
    }
}

static void draw_game(UltimateBoard *game, float save_timer)
{
    draw_active_board(game);
    draw_grid();
    draw_marks(game);

    // S-key hint, bottom left
    const char *hint = "S: Save";
    DrawText(hint, 12, WINDOW_HEIGHT - HINT_FONT_SIZE - 8, HINT_FONT_SIZE, Fade(GRAY, 0.6f));

    // "Saved!" confirmation fades out
    if (save_timer > 0.0f)
    {
        float alpha = save_timer / SAVE_MSG_DURATION;
        const char *msg = "Saved!";
        int msg_w = MeasureText(msg, HINT_FONT_SIZE);
        DrawText(msg, WINDOW_WIDTH - msg_w - 12, WINDOW_HEIGHT - HINT_FONT_SIZE - 8,
                 HINT_FONT_SIZE, Fade(GREEN, alpha));
    }
}

// ---- Game Over ----

static void update_game_over(Screen *screen, UltimateBoard *game)
{
    Button menu_btn = make_centered_button(520, 300, 64, "Main Menu", false);
    Button play_btn = make_centered_button(610, 300, 64, "Play Again", false);

    if (button_clicked(menu_btn))
        *screen = SCREEN_MENU;

    if (button_clicked(play_btn))
    {
        init_ultimate(game);
        *screen = SCREEN_PLAYING;
    }
}

static void draw_game_over(Player winner)
{
    DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, Fade(BLACK, 0.65f));

    const char *result = winner == X ? "X Wins!" : winner == O ? "O Wins!"
                                                               : "Draw!";
    Color result_color = winner == X ? RED : winner == O ? BLUE
                                                         : WHITE;

    int result_w = MeasureText(result, OVERLAY_FONT_SIZE);
    DrawText(result,
             (WINDOW_WIDTH - result_w) / 2,
             WINDOW_HEIGHT / 2 - OVERLAY_FONT_SIZE - 40,
             OVERLAY_FONT_SIZE, result_color);

    draw_button(make_centered_button(520, 300, 64, "Main Menu", false));
    draw_button(make_centered_button(610, 300, 64, "Play Again", false));
}

// ---- Main ----

int main(void)
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Ultimate Tic Tac Toe");
    SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));

    UltimateBoard game;
    init_ultimate(&game);

    Screen screen = SCREEN_MENU;
    Player winner = NONE;
    float save_timer = 0.0f;

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        switch (screen)
        {
        case SCREEN_MENU:
            update_menu(&screen, &game);
            break;

        case SCREEN_PLAYING:
            if (save_timer > 0.0f)
                save_timer -= dt;

            handle_input(&game, &save_timer);

            winner = ultimate_winner(&game);
            if (winner != NONE || ultimate_full(&game))
                screen = SCREEN_GAME_OVER;
            break;

        case SCREEN_GAME_OVER:
            update_game_over(&screen, &game);
            break;
        }

        BeginDrawing();
        ClearBackground(BLACK);

        switch (screen)
        {
        case SCREEN_MENU:
            draw_menu();
            break;

        case SCREEN_PLAYING:
            draw_game(&game, save_timer);
            break;

        case SCREEN_GAME_OVER:
            draw_game(&game, 0.0f);
            draw_game_over(winner);
            break;
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}