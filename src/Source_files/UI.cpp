#include "src/Header_files/UI.h"
#include "tinyfiledialogs.h"
#include "include/Config.h"
#include <raylib.h>
#include <string>

UI::UI()
{
    // Inicializa a janela
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "NES Emulator");
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

// Método separado para exibir apenas o diálogo de seleção de arquivo
std::string UI::showFileDialog() {
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
        return std::string(filePath);
    }
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
        Vector2{ (SCREEN_WIDTH - titleWidth)/2.0f, SCREEN_HEIGHT/2.0f - 120 },
        titleSize, titleSpacing, textColor);

    // Desenha o subtítulo
    DrawTextEx(font, subtitle,
        Vector2{ (SCREEN_WIDTH - subtitleWidth)/2.0f, SCREEN_HEIGHT/2.0f - 40 },
        subtitleSize, subtitleSpacing, textColor);
}

void UI::drawSelectButton()
{
    Rectangle button = { SCREEN_WIDTH/2.0f - 140, SCREEN_HEIGHT/2.0f + 40, 280, 35 };
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
    Rectangle button = { SCREEN_WIDTH/2.0f - 140, SCREEN_HEIGHT/2.0f + 90, 280, 35 };
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