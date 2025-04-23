#pragma once

#include "../../include/global.h"

using namespace std;

class UI {
public:
    UI();
    ~UI();

    // Inicializa a interface
    void init();
    
    // Mostra a tela de boas-vindas e seleção de arquivo
    string showWelcomeScreen();
    
    // Limpa recursos
    void cleanup();

private:
    // Configurações da tela
    const int screenWidth = 256 * 3;  // 768 pixels - proporção do NES
    const int screenHeight = 240 * 3;  // 720 pixels - proporção do NES
    
    // Texturas e fontes
    Texture2D background;
    Font font;
    
    // Cores
    Color backgroundColor = { 17, 0, 34, 255 };     // Roxo muito escuro
    Color textColor = { 255, 255, 255, 255 };       // Branco
    Color buttonColor = { 34, 0, 17, 255 };         // Vermelho muito escuro
    Color buttonHoverColor = { 68, 0, 34, 255 };    // Vermelho escuro
    
    // Funções auxiliares
    void drawWelcomeText();
    void drawSelectButton();
    void drawExitButton();
    bool isButtonHovered(Rectangle button);
}; 