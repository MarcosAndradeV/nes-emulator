
#include "src/Header_files/Emulator.h"
#include "src/Header_files/UI.h"
#include <iostream>
int main() {
  // Inicializa a interface
  UI ui;
  ui.init();

  // Mostra tela de boas-vindas e obtém o arquivo ROM
  std::string filePath = ui.showWelcomeScreen();

  if (filePath.empty()) {
    return 0; // Usuário fechou a janela
  }

  // Cria uma instância do emulador
  Emulator e;

  // Tenta carregar o jogo
  if (!e.loadGame(filePath)) {
    std::cout
        << "Não foi possível carregar o jogo. Verifique se o arquivo existe.\n";
    return 1;
  }

  // Inicia o emulador
  e.run();

  return 0;
}
