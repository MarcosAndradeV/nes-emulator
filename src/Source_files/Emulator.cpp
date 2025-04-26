#include "raylib.h"
#include "src/Header_files/Bus.h"
#include "src/Header_files/Emulator.h"
#include "src/Header_files/Cartridge.h"
#include "src/Header_files/UI.h"
#include "include/Config.h"
#include <cstring>
#include <iostream>
#include <string>
#include <memory>

// Barramento do sistema NES
Bus bus;

/// Inicializa todas as variáveis necessárias
Emulator::Emulator() {
    // Não inicializar a janela aqui, deixar a UI fazer isso
    ui = std::make_unique<UI>();
    ui->init();
}

/// Deinicializa todas as variáveis necessárias
Emulator::~Emulator() {
    // A UI agora se encarrega de limpar seus próprios recursos
}

/// Roda o loop principal da aplicação.
void Emulator::run() {
    std::string romPath = "";
    bool gameLoaded = false;

    // Preparar imagem para renderização do jogo
    Image gameScreen = {};
    memset(&gameScreen, 0, sizeof(Image));
    gameScreen.width = 256;
    gameScreen.height = 240;
    gameScreen.format = PIXELFORMAT_COMPRESSED_DXT1_RGBA;
    gameScreen.mipmaps = 1;

    // Loop principal do emulador
    while (!WindowShouldClose()) {
        BeginDrawing();

        if (!gameLoaded) {
            // Mostrar tela de boas-vindas e obter caminho da ROM
            ClearBackground(ui->backgroundColor);
            DrawTexture(ui->background, 0, 0, WHITE);

            ui->drawWelcomeText();
            ui->drawSelectButton();
            ui->drawExitButton();

            // Verificar interações do usuário
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Rectangle selectButton = { SCREEN_WIDTH / 2.0f - 140, SCREEN_HEIGHT / 2.0f + 40, 280, 35 };
                Rectangle exitButton = { SCREEN_WIDTH / 2.0f - 140, SCREEN_HEIGHT / 2.0f + 90, 280, 35 };

                if (ui->isButtonHovered(selectButton)) {
                    romPath = ui->showFileDialog();
                    if (!romPath.empty()) {
                        if (loadGame(romPath)) {
                            gameLoaded = true;
                        } else {
                            // Mensagem de erro
                            DrawText("Erro ao carregar o jogo. Tente novamente.", 10, 70, 20, RED);
                        }
                    }
                } else if (ui->isButtonHovered(exitButton)) {
                    break; // Sair do loop
                }
            }

            // Verificar tecla ESC
            if (IsKeyPressed(KEY_ESCAPE)) {
                break; // Sair do loop
            }
        }
        else {
            // Modo de jogo - Executar emulador
            ClearBackground(BLACK);

            // Executar um ciclo do sistema
            bus.clock();

            // Renderizar a tela do NES
            auto frameBuffer = bus.ppu.getFrameBuffer();
            gameScreen.data = (void*)frameBuffer;
            Texture2D texture = LoadTextureFromImage(gameScreen);

            // Centralize a textura do jogo na tela
            DrawTexture(texture, (SCREEN_WIDTH - 256) / 2, (SCREEN_HEIGHT - 240) / 2, WHITE);

            UnloadTexture(texture);

            // Verificar se pressionar ESC para voltar ao menu
            if (IsKeyPressed(KEY_ESCAPE)) {
                gameLoaded = false;
            }
        }

        EndDrawing();
    }
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

    std::cout << "Jogo carregado com sucesso: " << path << std::endl;
    return true;
}
