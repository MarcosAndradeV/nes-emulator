#include "Emulator.h"
#include "Config.h"
#include "raylib.h"

/// Inicializa todas as variaveis necessárias
Emulator::Emulator() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, TITLE);
}

/// Deinicializa todas as variaveis necessárias
Emulator::~Emulator() {
    CloseWindow();
}

/// Roda o loop principal da aplicação.
void Emulator::run() {
    while (!WindowShouldClose()) {
        // NOTE: Todo codigo relacionado com renderização deve esta entre `BeginDrawing` e `EndDrawing`
        BeginDrawing();
            ClearBackground(DEFAULT_BACKGROUND_COLOR);
        EndDrawing();
    }
}
