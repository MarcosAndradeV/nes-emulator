#include "raylib.h"
#include "src/Header_files/Bus.h"
#include "src/Header_files/Emulator.h"
#include "src/Header_files/Cartridge.h"
#include "src/Header_files/UI.h"
#include "include/Config.h"
#include <cstdlib>
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
    gameScreen.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    gameScreen.mipmaps = 1;
    gameScreen.data = (void*)bus.ppu.getFrameBuffer();
    Texture2D texture = {};
    memset(&texture, 0, sizeof(Texture2D));

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
                    if (!romPath.empty() && loadGame(romPath)) {
                        if (texture.id > 0) {
                            UnloadTexture(texture); // limpa anterior, se existir
                        }
                        gameScreen.data = (void*)bus.ppu.getFrameBuffer();
                        texture = LoadTextureFromImage(gameScreen);
                        gameLoaded = true;
                    } else {
                        DrawText("Erro ao carregar o jogo. Tente novamente.", 10, 70, 20, RED);
                    }
                } else if (ui->isButtonHovered(exitButton)) {
                    break; // Sair do loop
                }
            }

            if (IsKeyPressed(KEY_ESCAPE)) {
                gameLoaded = false;
            }

        } else {
            // Modo de jogo - Executar emulador
            ClearBackground(BLACK);

            // Executar um quadro do emulador
            for (int i = 0; i < 29780; i++) {
                bus.clock();
            }

            // Atualizar e desenhar frame NES
            auto frameBuffer = bus.ppu.getFrameBuffer();
            if (texture.id > 0) {
                UpdateTexture(texture, frameBuffer);
                DrawTexture(texture, (SCREEN_WIDTH - 256) / 2, (SCREEN_HEIGHT - 240) / 2, WHITE);
            }

            // Verificar se pressionar ESC para voltar ao menu
            if (IsKeyPressed(KEY_ESCAPE)) {
                gameLoaded = false;
            }
        }

        EndDrawing();
    }
    UnloadTexture(texture);
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

    std::cout << "Jogo carregado com sucesso: " << path << std::endl;
    return true;
}
