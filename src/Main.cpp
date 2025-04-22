#include "../include/global.h"

int main() {
    // Cria uma instância do emulador
    Emulator e;
    
    // Tenta carregar um jogo
    // Substitua "jogo.nes" pelo caminho do seu arquivo ROM
    if (!e.loadGame("Jogos/Super_Mario_Bros_2 .nes")) {
        cout << "Não foi possível carregar o jogo. Verifique se o arquivo existe." << endl;
        return 1;
    }
    
    // Inicia o emulador
    e.run();
    
    return 0;
}
