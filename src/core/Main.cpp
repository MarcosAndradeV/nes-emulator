#include "include/raylib.h"

int main() {
    InitWindow(800, 600, "NES Emulator");
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RED);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
