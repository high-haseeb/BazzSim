#include <raylib.h>
#include <stdio.h>

#define WIDTH 800
#define HEIGHT 800
#define FONT_SCALE 1.5
#define PAD 16
#define ROWS 10
#define COLS 10

int main(int argc, char *argv[]) {

    /* if (argc < 1) { */
    /*     printf("[ERROR]: You should provide a map file\n"); */
    /*     return -1; */
    /* } */
    /**/
    /* char *mapFile = argv[1]; */
    /**/
    /* if (!FileExists(mapFile)) { */
    /*     printf("[ERROR] Map file not found\n"); */
    /*     return -1; */
    /* } */
    /* char *mapData = LoadFileText(mapFile); */

    InitWindow(WIDTH, HEIGHT, "BaazSim");
    SetTargetFPS(60);

    Rectangle player = {.width = 100, .height = 100};
    float speed = 10.0f; // fps * speed px/sec
    Vector2 dir = (Vector2){.x = 1, .y = 1};

    int width = WIDTH / COLS;
    int height = HEIGHT / ROWS;

    while (!WindowShouldClose()) {
        BeginDrawing();
        {
            ClearBackground((Color){.r = 0x18, .g = 0x18, .b = 0x18});
            DrawText("Hello World", PAD, PAD, 16 * FONT_SCALE, WHITE);
            if (player.x + player.width > WIDTH || player.x < 0) {
                dir.x *= -1;
            }
            if (player.y + player.height > HEIGHT || player.y < 0) {
                dir.y *= -1;
            }
            player.x += speed * dir.x;
            player.y += speed * dir.y;

            DrawRectangleRec(player, YELLOW);
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
