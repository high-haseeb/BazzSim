#include <math.h>
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
    *row_count -= 1;
    *col_count = strlen(rowsData[0]);
    Tile **map = malloc(*row_count * sizeof(Tile *));
    if (!map) {
        perror("Failed to allocate memory for map rows");
        return NULL;
    }

    for (int i = 0; i < *row_count; i++) {
        map[i] = malloc(*col_count * sizeof(Tile));
        if (!map[i]) {
            perror("[ERROR] Failed to allocate memory for map columns");
            for (int k = 0; k < i; k++) {
                free(map[k]);
            }
            free(map);
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
    int width = GetScreenHeight() / col_count;
    int height = GetScreenWidth() / row_count;
    for (int i = 0; i < row_count; ++i) {
        for (int j = 0; j < col_count; ++j) {
            if (map[i][j] == WALL)
                DrawRectangle(i * height, j * width, width, height, BLUE);
        }
    }
}

bool isHit(int x, int y, Tile **map) {
    int xPos = x / (int)(GetScreenHeight() / 10);
    int yPos = y / (int)(GetScreenWidth() / 10);
    /* printf("x : %d | y : %d \n", xPos, yPos); */
    if (xPos >= 10 || xPos < 0)
        return true;
    if (yPos >= 10 || yPos < 0)
        return true;
    if (map[xPos][yPos] == WALL) {
        DrawCircle(x, y, 10.0, PINK);
        return true;
    }
    return false;
}

void updatePlayerManually(Player *player) {
    switch (GetKeyPressed()) {
    case KEY_W: {
        player->direction.x = 0.0f;
        player->direction.y = -1.0f;
    } break;
    case KEY_S: {
        player->direction.x = 0.0f;
        player->direction.y = 1.0f;
    } break;
    case KEY_A: {
        player->direction.y = 0.0f;
        player->direction.x = -1.0f;
    } break;
    case KEY_D: {
        player->direction.x = 1.0f;
        player->direction.y = 0.0f;
    } break;
    }
}

void RenderPlayer(Player *player, Tile **map, Image *image) {
    /* DrawRectangleV(player->position, player->size, player->color); */
    float r = 20.0f;
    DrawCircleV(player->position, r, player->color);
    float angle = 0;
    for (int i = 0; i < 12; ++i) {
        // simulate ultrasonic sensors using raycasting technique
        angle += 360.0f / 12 * 0.0174533;
        int startX = player->position.x + cos(angle) * r;
        int startY = player->position.y + sin(angle) * r;
        float step = 1.0f;
        int endX = player->position.x + cos(angle) * r * step;
        int endY = player->position.y + sin(angle) * r * step;
        int i = 0;
        bool hit = true;
        while (!isHit((int)endX, (int)endY, map)) {
            endX = player->position.x + cos(angle) * r * step * i;
            endY = player->position.y + sin(angle) * r * step * i;
            ++i;
            if (i > 100) {
                hit = false;
                break;
            }
        }
        if (hit) {
            ImageDrawPixel(image, endX, endY, GREEN);
            DrawLine(startX, startY, endX, endY, GREEN);
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
        printf("\x1b[30;41m[ERROR] No file %s found. Please make sure the file exists\n",
               file_path);
        return -1;
    }
    char *mapData = LoadFileText(file_path);
    int row_count;
    int col_count;
    Tile **map = ParseMap(mapData, &row_count, &col_count);
    printf("\x1b[39;42m[INFO] rows : %d, cols : %d\n\x1b[39;49m", row_count, col_count);

    InitWindow(WIDTH, HEIGHT, "BaazSim-2d");
    SetTargetFPS(60);

    Player player = {};
    player.size = (Vector2){.x = (float)GetScreenWidth() / (col_count * 2),
                            .y = (float)GetScreenHeight() / (row_count * 2)};
    player.color = RED;
    player.position =
        (Vector2){.x = GetScreenWidth() / 2.0f - 10.0f, .y = GetScreenHeight() / 2.0f - 10.0f};
    player.velocity = (Vector2){.x = 1.0f, .y = 1.0f};
    player.direction = (Vector2){.x = 0.0f, .y = 0.0f};

    Image slamImage = GenImageColor(WIDTH, HEIGHT, BLACK);

    while (!WindowShouldClose()) {
        BeginDrawing();
        {
            ClearBackground((Color){.r = 0x18, .g = 0x18, .b = 0x18});
            /* DrawText("BaazSim", PAD, PAD, 16 * FONT_SCALE, WHITE); */

            RenderMap(map, row_count, col_count);
            updatePlayerManually(&player);
            Texture tex = LoadTextureFromImage(slamImage);
            DrawTexture(tex, 0, 0, WHITE);
            RenderPlayer(&player, map, &slamImage);
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
