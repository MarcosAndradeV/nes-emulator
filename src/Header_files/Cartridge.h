#pragma once

#include "../../include/global.h"

class Mapper; // Declaração antecipada da classe Mapper

class Cartridge {
public:
    // Construtor e destrutor
    Cartridge(const std::string& filename); // Carrega ROM do arquivo
    ~Cartridge();

    // Interface com a CPU - permite ler/escrever na memória do cartucho
    bool cpuRead(uint16_t addr, uint8_t& data);  
    bool cpuWrite(uint16_t addr, uint8_t data);  

    // Interface com a PPU - permite acessar gráficos do cartucho
    bool ppuRead(uint16_t addr, uint8_t& data);  
    bool ppuWrite(uint16_t addr, uint8_t data); 
    
    // Controle do sistema
    void reset();  // Reinicializa o cartucho
    std::shared_ptr<Mapper> GetMapper() { return mapper; }  // Retorna o mapeador de memória

private:
    std::vector<uint8_t> prgROM;  // Memória ROM do programa (código do jogo)
    std::vector<uint8_t> chrROM;  // Memória ROM de caracteres (gráficos)
    std::shared_ptr<Mapper> mapper;  // Gerenciador de mapeamento de memória
    bool imageValid = false;  // Indica se o cartucho foi carregado corretamente
};