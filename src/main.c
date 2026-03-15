#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

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
#define LABEL_FONT_SIZE 28

#define SAVE_MSG_DURATION 1.5f

typedef enum
{
    SCREEN_MENU,
    SCREEN_SETTINGS,
    SCREEN_PLAYING,
    SCREEN_GAME_OVER
} Screen;

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
    return CheckCollisionPointRec(mouse, btn.rect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
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

    int tw = MeasureText(btn.label, BUTTON_FONT_SIZE);
    DrawText(btn.label,
             (int)(btn.rect.x + (btn.rect.width - tw) / 2),
             (int)(btn.rect.y + (btn.rect.height - BUTTON_FONT_SIZE) / 2),
             BUTTON_FONT_SIZE, text_c);
}

static Button centered_btn(int y, int w, int h, const char *label, bool disabled)
{
    return (Button){
        .rect = {(WINDOW_WIDTH - w) / 2, y, w, h},
        .label = label,
        .disabled = disabled};
}

#define TEXT_INPUT_MAX 511

typedef struct
{
    char buf[TEXT_INPUT_MAX + 1];
    int len;
} TextInput;

static void ti_set(TextInput *ti, const char *s)
{
    strncpy(ti->buf, s, TEXT_INPUT_MAX);
    ti->buf[TEXT_INPUT_MAX] = '\0';
    ti->len = (int)strlen(ti->buf);
}

static void ti_update(TextInput *ti)
{
    int key = GetCharPressed();
    while (key > 0)
    {
        if (key >= 32 && key <= 126 && ti->len < TEXT_INPUT_MAX)
        {
            ti->buf[ti->len++] = (char)key;
            ti->buf[ti->len] = '\0';
        }
        key = GetCharPressed();
    }
    if (IsKeyPressed(KEY_BACKSPACE) && ti->len > 0)
        ti->buf[--ti->len] = '\0';
}

static void ti_draw(TextInput *ti, Rectangle rect, int font_size)
{
    DrawRectangleRec(rect, Fade(WHITE, 0.07f));
    DrawRectangleLinesEx(rect, 2, WHITE);

    bool cursor_on = (int)(GetTime() * 2) % 2 == 0;

    char display[TEXT_INPUT_MAX + 2];
    snprintf(display, sizeof(display), "%s%s", ti->buf, cursor_on ? "|" : "");

    DrawText(display,
             (int)rect.x + 10,
             (int)(rect.y + (rect.height - font_size) / 2),
             font_size, WHITE);
}

static bool dir_exists(const char *path)
{
    if (!path || path[0] == '\0')
        return false;
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

#define SLOT_X 218
#define SLOT_W_LOAD 300
#define SLOT_GAP 16
#define SLOT_W_NEW 148
#define SLOT_H 60
#define SLOT_Y0 390
#define SLOT_STRIDE 76

#define MENU_SETTINGS_Y (SLOT_Y0 + MAX_SLOTS * SLOT_STRIDE + 14)
#define MENU_QUIT_Y (MENU_SETTINGS_Y + SLOT_STRIDE)

static void update_menu(Screen *screen, UltimateBoard *game, const char *save_dir,
                        int *current_slot, TextInput *settings_input)
{
    for (int i = 0; i < MAX_SLOTS; i++)
    {
        int y = SLOT_Y0 + i * SLOT_STRIDE;
        bool exists = save_exists(save_dir, i);

        Button load_btn = {
            .rect = {SLOT_X, y, SLOT_W_LOAD, SLOT_H},
            .label = "",
            .disabled = !exists};
        Button new_btn = {
            .rect = {SLOT_X + SLOT_W_LOAD + SLOT_GAP, y, SLOT_W_NEW, SLOT_H},
            .label = "",
            .disabled = false};

        if (button_clicked(load_btn))
        {
            if (load_game(game, save_dir, i))
            {
                *current_slot = i;
                *screen = SCREEN_PLAYING;
            }
        }

        if (button_clicked(new_btn))
        {
            init_ultimate(game);
            *current_slot = i;
            *screen = SCREEN_PLAYING;
        }
    }

    Button settings_btn = centered_btn(MENU_SETTINGS_Y, 300, SLOT_H, "Settings", false);
    Button quit_btn = centered_btn(MENU_QUIT_Y, 300, SLOT_H, "Quit", false);

    if (button_clicked(settings_btn))
    {
        ti_set(settings_input, save_dir);
        *screen = SCREEN_SETTINGS;
    }
    if (button_clicked(quit_btn))
        CloseWindow();
}

static void draw_menu(const char *save_dir)
{
    const char *title = "Ultimate";
    const char *subtitle = "Tic Tac Toe";

    DrawText(title, (WINDOW_WIDTH - MeasureText(title, TITLE_FONT_SIZE)) / 2, 200, TITLE_FONT_SIZE, WHITE);
    DrawText(subtitle, (WINDOW_WIDTH - MeasureText(subtitle, TITLE_FONT_SIZE)) / 2, 285, TITLE_FONT_SIZE, GRAY);

    for (int i = 0; i < MAX_SLOTS; i++)
    {
        int y = SLOT_Y0 + i * SLOT_STRIDE;
        bool exists = save_exists(save_dir, i);

        char load_label[32];
        snprintf(load_label, sizeof(load_label), "Slot %d: %s", i + 1, exists ? "Resume" : "Empty");

        Button load_btn = {
            .rect = {SLOT_X, y, SLOT_W_LOAD, SLOT_H},
            .label = load_label,
            .disabled = !exists};
        Button new_btn = {
            .rect = {SLOT_X + SLOT_W_LOAD + SLOT_GAP, y, SLOT_W_NEW, SLOT_H},
            .label = "New",
            .disabled = false};

        draw_button(load_btn);
        draw_button(new_btn);
    }

    draw_button(centered_btn(MENU_SETTINGS_Y, 300, SLOT_H, "Settings", false));
    draw_button(centered_btn(MENU_QUIT_Y, 300, SLOT_H, "Quit", false));
}

static void update_settings(Screen *screen, char *save_dir, TextInput *ti)
{
    ti_update(ti);

    bool valid = dir_exists(ti->buf);

    Button confirm = centered_btn(600, 300, SLOT_H, "Confirm", !valid);
    Button back = centered_btn(676, 300, SLOT_H, "Back", false);

    if (button_clicked(confirm))
    {
        strncpy(save_dir, ti->buf, MAX_PATH_LEN - 1);
        save_dir[MAX_PATH_LEN - 1] = '\0';
        *screen = SCREEN_MENU;
    }
    if (button_clicked(back))
        *screen = SCREEN_MENU;
}

static void draw_settings(TextInput *ti)
{
    const char *title = "Settings";
    DrawText(title, (WINDOW_WIDTH - MeasureText(title, TITLE_FONT_SIZE)) / 2, 200, TITLE_FONT_SIZE, WHITE);

    DrawText("Save directory", 200, 390, LABEL_FONT_SIZE, GRAY);

    Rectangle input_rect = {200, 428, 500, 60};
    ti_draw(ti, input_rect, BUTTON_FONT_SIZE);

    bool valid = dir_exists(ti->buf);
    const char *status = valid ? "Directory found" : "Directory not found";
    Color status_color = valid ? GREEN : RED;
    DrawText(status, 200, 500, HINT_FONT_SIZE, status_color);

    draw_button(centered_btn(600, 300, SLOT_H, "Confirm", !valid));
    draw_button(centered_btn(676, 300, SLOT_H, "Back", false));
}

static void handle_input(UltimateBoard *game, float *save_timer,
                         const char *save_dir, int current_slot)
{
    if (IsKeyPressed(KEY_S))
    {
        if (save_game(game, save_dir, current_slot))
            *save_timer = SAVE_MSG_DURATION;
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mouse = GetMousePosition();

        int col = (int)mouse.x / CELL_WIDTH;
        int row = (int)mouse.y / CELL_HEIGHT;

        int board_index = (row / BOARD_DIM) * BOARD_DIM + (col / BOARD_DIM);
        int cell_index = (row % BOARD_DIM) * BOARD_DIM + (col % BOARD_DIM);

        ultimate_move(game, board_index, cell_index);
    }
}

static void draw_grid(void)
{
    for (int i = 0; i <= GRID_DIM; i++)
    {
        int thick = (i % BOARD_DIM == 0) ? GRID_THICK : GRID_THIN;
        DrawLineEx((Vector2){i * CELL_WIDTH, 0}, (Vector2){i * CELL_WIDTH, WINDOW_HEIGHT}, thick, WHITE);
        DrawLineEx((Vector2){0, i * CELL_HEIGHT}, (Vector2){WINDOW_WIDTH, i * CELL_HEIGHT}, thick, WHITE);
    }
}

static void draw_marks(UltimateBoard *game)
{
    for (int b = 0; b < BOARD_CELLS; b++)
    {
        int bx = b % BOARD_DIM;
        int by = b / BOARD_DIM;
        Board brd = game->locals[b];

        for (int c = 0; c < BOARD_CELLS; c++)
        {
            int col = bx * BOARD_DIM + (c % BOARD_DIM);
            int row = by * BOARD_DIM + (c / BOARD_DIM);
            int x = col * CELL_WIDTH;
            int y = row * CELL_HEIGHT;

            const char *text = NULL;
            if (brd.cells[c] == X)
                text = "X";
            else if (brd.cells[c] == O)
                text = "O";

            if (text)
            {
                int tw = MeasureText(text, MARK_FONT_SIZE);
                DrawText(text,
                         x + (CELL_WIDTH - tw) / 2,
                         y + (CELL_HEIGHT - MARK_FONT_SIZE) / 2,
                         MARK_FONT_SIZE,
                         brd.cells[c] == X ? RED : BLUE);
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
        Rectangle r = {
            (game->next % BOARD_DIM) * SUBBOARD_WIDTH,
            (game->next / BOARD_DIM) * SUBBOARD_HEIGHT,
            SUBBOARD_WIDTH, SUBBOARD_HEIGHT};
        DrawRectangleRec(r, Fade(YELLOW, 0.15f));
    }
    else
    {
        for (int b = 0; b < BOARD_CELLS; b++)
        {
            if (!board_playable(&game->locals[b]))
                continue;
            Rectangle r = {
                (b % BOARD_DIM) * SUBBOARD_WIDTH,
                (b / BOARD_DIM) * SUBBOARD_HEIGHT,
                SUBBOARD_WIDTH, SUBBOARD_HEIGHT};
            DrawRectangleRec(r, Fade(YELLOW, 0.15f));
        }
    }
}

static void draw_game(UltimateBoard *game, float save_timer)
{
    draw_active_board(game);
    draw_grid();
    draw_marks(game);

    DrawText("S: Save", 12, WINDOW_HEIGHT - HINT_FONT_SIZE - 8, HINT_FONT_SIZE, Fade(GRAY, 0.6f));

    if (save_timer > 0.0f)
    {
        float alpha = save_timer / SAVE_MSG_DURATION;
        const char *msg = "Saved!";
        int mw = MeasureText(msg, HINT_FONT_SIZE);
        DrawText(msg, WINDOW_WIDTH - mw - 12, WINDOW_HEIGHT - HINT_FONT_SIZE - 8,
                 HINT_FONT_SIZE, Fade(GREEN, alpha));
    }
}

static void update_game_over(Screen *screen, UltimateBoard *game, int current_slot,
                             const char *save_dir)
{
    Button menu_btn = centered_btn(520, 300, SLOT_H, "Main Menu", false);
    Button play_btn = centered_btn(596, 300, SLOT_H, "Play Again", false);

    if (button_clicked(menu_btn))
        *screen = SCREEN_MENU;

    if (button_clicked(play_btn))
    {
        char path[MAX_PATH_LEN];
        get_save_path(save_dir, current_slot, path, sizeof(path));
        remove(path);

        init_ultimate(game);
        *screen = SCREEN_PLAYING;
    }
}

static void draw_game_over(Player winner)
{
    DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, Fade(BLACK, 0.65f));

    const char *result = winner == X ? "X Wins!" : winner == O ? "O Wins!"
                                                               : "Draw!";
    Color result_col = winner == X ? RED : winner == O ? BLUE
                                                       : WHITE;

    int rw = MeasureText(result, OVERLAY_FONT_SIZE);
    DrawText(result,
             (WINDOW_WIDTH - rw) / 2,
             WINDOW_HEIGHT / 2 - OVERLAY_FONT_SIZE - 40,
             OVERLAY_FONT_SIZE, result_col);

    draw_button(centered_btn(520, 300, SLOT_H, "Main Menu", false));
    draw_button(centered_btn(596, 300, SLOT_H, "Play Again", false));
}

int main(void)
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Ultimate Tic Tac Toe");
    SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));

    UltimateBoard game;
    init_ultimate(&game);

    Screen screen = SCREEN_MENU;
    Player winner = NONE;
    float save_timer = 0.0f;
    int current_slot = 0;
    char save_dir[MAX_PATH_LEN] = ".";
    TextInput settings_input = {0};

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        switch (screen)
        {
        case SCREEN_MENU:
            update_menu(&screen, &game, save_dir, &current_slot, &settings_input);
            break;

        case SCREEN_SETTINGS:
            update_settings(&screen, save_dir, &settings_input);
            break;

        case SCREEN_PLAYING:
            if (save_timer > 0.0f)
                save_timer -= dt;
            handle_input(&game, &save_timer, save_dir, current_slot);
            winner = ultimate_winner(&game);
            if (winner != NONE || ultimate_full(&game))
                screen = SCREEN_GAME_OVER;
            break;

        case SCREEN_GAME_OVER:
            update_game_over(&screen, &game, current_slot, save_dir);
            break;
        }

        BeginDrawing();
        ClearBackground(BLACK);

        switch (screen)
        {
        case SCREEN_MENU:
            draw_menu(save_dir);
            break;

        case SCREEN_SETTINGS:
            draw_settings(&settings_input);
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