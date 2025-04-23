

// Variáveis globais para o emulador
#include "src/Header_files/Bus.h"
#include "src/Header_files/Emulator.h"
#include "src/Header_files/Cartridge.h"
#include "include/Config.h"
#include <cstring>
#include <iostream>
#include <string>
#include <memory>
Bus bus;  // Barramento do sistema NES
std::string romPath = "";  // Caminho para o arquivo ROM

/// Inicializa todas as variaveis necessárias
Emulator::Emulator() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, TITLE);
}

/// Deinicializa todas as variaveis necessárias
Emulator::~Emulator() {
    CloseWindow();
}

/// Carrega um jogo NES
bool Emulator::loadGame(const std::string& path) {
    // Cria um novo cartucho com o arquivo ROM
    auto cart = std::make_shared<Cartridge>(path);

    // Verifica se o cartucho foi carregado corretamente
    if (!cart->ImageValid()) {
        std::cout << "Erro ao carregar o jogo: " << path << std::endl;
        return false;
    }

    // Insere o cartucho no barramento
    bus.insertCartridge(cart);

    // Reseta o sistema
    bus.reset();

    // Salva o caminho do jogo
    romPath = path;

    std::cout << "Jogo carregado com sucesso: " << path << std::endl;
    return true;
}

/// Roda o loop principal da aplicação.
void Emulator::run() {
    Image img = {};
    memset(&img, 0, sizeof(Image));
    img.width = 256;
    img.height = 240;
    img.format = PIXELFORMAT_COMPRESSED_DXT1_RGBA;
    img.mipmaps = 1;
    // Loop principal do emulador
    while (!WindowShouldClose()) {
        // Se um jogo estiver carregado, executa um ciclo do sistema
        if (!romPath.empty()) {
            bus.clock();
        }
        // Renderização
        BeginDrawing();
            ClearBackground(DEFAULT_BACKGROUND_COLOR);
            // Aqui você pode adicionar código para renderizar a tela do NES
            auto frameBuffer = bus.ppu.getFrameBuffer();
            img.data = (void*)frameBuffer;
            Texture2D texture = LoadTextureFromImage(img);
            DrawTexture(texture, 256, 240, WHITE);
            UnloadTexture(texture);
        EndDrawing();
    }
}
