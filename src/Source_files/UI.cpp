#include "src/Header_files/UI.h"
#include "tinyfiledialogs.h"
UI::UI()
{
    // Inicializa a janela
    InitWindow(screenWidth, screenHeight, "NES Emulator");
    SetTargetFPS(60);

    // Carrega recursos
    font = LoadFontEx("assets/font.ttf", 32, 0, 0);
    background = LoadTexture("assets/background.png");
}

UI::~UI() {
    cleanup();
}

void UI::init() {
    // Configurações iniciais
    SetExitKey(KEY_NULL); // Desabilita ESC para sair
}

std::string UI::showWelcomeScreen()
{
    while (!WindowShouldClose()) {
        BeginDrawing();

        // Fundo
        ClearBackground(backgroundColor);
        DrawTexture(background, 0, 0, WHITE);

        // Texto de boas-vindas
        drawWelcomeText();

        // Botões
        drawSelectButton();
        drawExitButton();

        EndDrawing();

        // Verifica clique nos botões
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Rectangle selectButton = { screenWidth/2.0f - 140, screenHeight/2.0f + 40, 280, 35 };
            Rectangle exitButton = { screenWidth/2.0f - 140, screenHeight/2.0f + 90, 280, 35 };

            if (isButtonHovered(selectButton))
            {
                // Abre diálogo para selecionar arquivo ROM
                const char* filterPatterns[] = { "*.nes" };
                const char* filePath = tinyfd_openFileDialog(
                    "Selecione um arquivo ROM do NES",
                    "",
                    1,
                    filterPatterns,
                    "Arquivos NES (*.nes)",
                    0
                );

                if (filePath != nullptr) {
                    cleanup();
                    return std::string(filePath);
                }
            }
            else if (isButtonHovered(exitButton)) {
                cleanup();
                return "";
            }
        }

        // Verifica tecla ESC
        if (IsKeyPressed(KEY_ESCAPE)) {
            cleanup();
            return "";
        }
    }

    cleanup();
    return "";
}

void UI::cleanup() {
    UnloadTexture(background);
    UnloadFont(font);
}

void UI::drawWelcomeText() {
    const char* title = "NES Emulator";
    const char* subtitle = "Escolha a ROM pra jogar";

    // Ajustando o título para ficar mais largo
    float titleSpacing = 8.0f;  // Espaçamento entre letras do título
    int titleSize = 48;         // Tamanho ajustado para a largura correta
    int titleWidth = MeasureTextEx(font, title, titleSize, titleSpacing).x;

    // Subtítulo com proporção correta
    float subtitleSpacing = 2.0f;  // Aumentei o espaçamento do subtítulo
    int subtitleSize = 30;         // Ajustei o tamanho
    int subtitleWidth = MeasureTextEx(font, subtitle, subtitleSize, subtitleSpacing).x;

    // Desenha o título
    DrawTextEx(font, title,
        Vector2{ (screenWidth - titleWidth)/2.0f, screenHeight/2.0f - 120 },
        titleSize, titleSpacing, textColor);

    // Desenha o subtítulo
    DrawTextEx(font, subtitle,
        Vector2{ (screenWidth - subtitleWidth)/2.0f, screenHeight/2.0f - 40 },
        subtitleSize, subtitleSpacing, textColor);
}

void UI::drawSelectButton()
{
    Rectangle button = { screenWidth/2.0f - 140, screenHeight/2.0f + 40, 280, 35 };
    Color currentColor = isButtonHovered(button) ? buttonHoverColor : buttonColor;

    DrawRectangleRec(button, currentColor);
    DrawRectangleLinesEx(button, 1, WHITE);

    const char* buttonText = "Carregar ROM";
    float buttonSpacing = 2.0f;  // Aumentei o espaçamento
    int buttonFontSize = 20;     // Ajustei o tamanho
    int textWidth = MeasureTextEx(font, buttonText, buttonFontSize, buttonSpacing).x;

    DrawTextEx(font, buttonText,
        Vector2{ button.x + (button.width - textWidth)/2, button.y + 7 },
        buttonFontSize, buttonSpacing, textColor);
}

void UI::drawExitButton() {
    Rectangle button = { screenWidth/2.0f - 140, screenHeight/2.0f + 90, 280, 35 };
    Color currentColor = isButtonHovered(button) ? buttonHoverColor : buttonColor;

    DrawRectangleRec(button, currentColor);
    DrawRectangleLinesEx(button, 1, WHITE);

    const char* buttonText = "Sair";
    float buttonSpacing = 6.0f;  // Aumentei o espaçamento
    int buttonFontSize = 22;     // Ajustei o tamanho
    int textWidth = MeasureTextEx(font, buttonText, buttonFontSize, buttonSpacing).x;

    DrawTextEx(font, buttonText,
        Vector2{ button.x + (button.width - textWidth)/2, button.y + 7 },
        buttonFontSize, buttonSpacing, textColor);
}

bool UI::isButtonHovered(Rectangle button) {
    Vector2 mousePos = GetMousePosition();
    return CheckCollisionPointRec(mousePos, button);
}
