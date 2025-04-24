#pragma once

#include <raylib.h>
#include <string>

class UI {
public:
    UI();
    ~UI();

    void init();
    std::string showFileDialog(); // Novo método apenas para diálogo
    void drawWelcomeText();
    void drawSelectButton();
    void drawExitButton();
    bool isButtonHovered(Rectangle button);
    void cleanup();
    
    // Tornamos alguns membros públicos para acessar no Emulator
    Texture2D background;
    Font font;
    Color backgroundColor = BLACK;
    Color buttonColor = DARKBLUE;
    Color buttonHoverColor = BLUE;
    Color textColor = WHITE;
};