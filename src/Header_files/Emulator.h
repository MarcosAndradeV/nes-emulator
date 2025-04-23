#pragma once

#include <string>
/// Responsavel por inicialização, deinicialização e event loop.
class Emulator {
public:
    Emulator();
    ~Emulator();

    /// Carrega um jogo NES
    /// @param path Caminho para o arquivo ROM
    /// @return true se o jogo foi carregado com sucesso
    bool loadGame(const std::string& path);

    void run();
};
