#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH 800
#define HEIGHT 800
#define FONT_SCALE 1.5
#define PAD 16

typedef struct {
    Vector2 velocity;
    Vector2 direction;
    Vector2 position;
    Vector2 size;
    Color color;
} Player;

typedef enum {
    WALL,
    NONE,
} Tile;

Tile **ParseMap(char *mapData, int *row_count, int *col_count) {
    const char **rowsData = TextSplit(mapData, '\n', row_count);
    *col_count = strlen(rowsData[0]);
    Tile **map = malloc(*row_count * sizeof(Tile *));
    if (!map) {
        perror("Failed to allocate memory for map rows");
        return NULL;
    }

    for (int i = 0; i < *row_count; i++) {
        map[i] = malloc(*col_count * sizeof(Tile));
        if (!map[i]) {
            perror("Failed to allocate memory for map columns");
            for (int k = 0; k < i; k++) {
                free(map[k]); // Free already allocated rows
            }
            free(map); // Free row pointers
            return NULL;
        }
    }

    for (int i = 0; i < *row_count; i++) {
        for (int j = 0; j < *col_count; j++) {
            map[i][j] = rowsData[i][j] == '#' ? WALL : NONE;
        }
    }
    return map;
}

void FreeMap(Tile **map, int row_count) {
    for (int i = 0; i < row_count; i++) {
        free(map[i]);
    }
    free(map);
}

void RenderMap(Tile **map, int row_count, int col_count) {
    int width = HEIGHT / row_count;
    int height = WIDTH / col_count;
    for (int i = 0; i < row_count; i++) {
        for (int j = 0; j < col_count; j++) {
            if (map[i][j] == WALL)
                DrawRectangle(i * height, j * width, width, height, BLUE);
        }
    }
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("[ERROR] No Map Files passed in.");
        return -1;
    }
    char *file_path = argv[1];
    if (!FileExists(file_path)) {
        printf("[ERROR] No file %s found. Please make sure the file exists", file_path);
        return -1;
    }
    char *mapData = LoadFileText(file_path);
    int row_count;
    int col_count;
    Tile **map = ParseMap(mapData, &row_count, &col_count);

    InitWindow(WIDTH, HEIGHT, "BaazSim-2d");
    SetTargetFPS(60);

    Player player = {};
    player.size = (Vector2){.x = (float)GetScreenWidth() / col_count,
                            .y = (float)GetScreenHeight() / row_count};
    player.color = RED;
    player.position = (Vector2){.x = 0.0f, .y = 0.0f};
    player.velocity = (Vector2){.x = 1.0f, .y = 1.0f};
    player.direction = (Vector2){.x = 0.0f, .y = 0.0f};

    while (!WindowShouldClose()) {

        switch (GetKeyPressed()) {
        case KEY_W: {
            player.direction.x = 0.0f;
            player.direction.y = -1.0f;
        } break;
        case KEY_S: {
            player.direction.x = 0.0f;
            player.direction.y = 1.0f;
        } break;
        case KEY_A: {
            player.direction.y = 0.0f;
            player.direction.x = -1.0f;
        } break;
        case KEY_D: {
            player.direction.x = 1.0f;
            player.direction.y = 0.0f;
        } break;
        }

        BeginDrawing();
        {
            ClearBackground((Color){.r = 0x18, .g = 0x18, .b = 0x18});
            DrawText("BaazSim", PAD, PAD, 16 * FONT_SCALE, WHITE);

            RenderMap(map, col_count, row_count);
            // render player
            DrawRectangle(player.position.x, player.position.y, player.size.x, player.size.y,
                          player.color);
        }
        EndDrawing();

        player.position.x += player.direction.x * player.velocity.x;
        player.position.y += player.direction.y * player.velocity.y;
    }

    FreeMap(map, row_count);
    UnloadFileText(mapData);
    CloseWindow();
    return 0;
}
