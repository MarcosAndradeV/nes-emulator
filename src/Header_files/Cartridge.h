#pragma once

#include "../../include/global.h"

using namespace std;

class Mapper; // Declaração antecipada da classe Mapper

class Cartridge {
public:
    // Construtor e destrutor
    Cartridge(const string& filename); // Carrega ROM do arquivo
    ~Cartridge();

    // Interface com a CPU - permite ler/escrever na memória do cartucho
    bool cpuRead(uint16_t addr, uint8_t& data);  
    bool cpuWrite(uint16_t addr, uint8_t data);  

    // Interface com a PPU - permite acessar gráficos do cartucho
    bool ppuRead(uint16_t addr, uint8_t& data);  
    bool ppuWrite(uint16_t addr, uint8_t data); 
    
    // Controle do sistema
    void reset();  // Reinicializa o cartucho
    bool ImageValid() { return bImageValid; }  // Verifica se o cartucho é válido
    MIRROR Mirror();  // Retorna o modo de espelhamento
    shared_ptr<Mapper> GetMapper() { return pMapper; }  // Retorna o mapeador de memória

private:
    vector<uint8_t> vPRGMemory;  // Memória ROM do programa (código do jogo)
    vector<uint8_t> vCHRMemory;  // Memória ROM de caracteres (gráficos)
    shared_ptr<Mapper> pMapper;  // Gerenciador de mapeamento de memória
    bool bImageValid = false;  // Indica se o cartucho foi carregado corretamente
    uint8_t nMapperID = 0;  // ID do mapeador
    uint8_t nPRGBanks = 0;  // Número de bancos PRG
    uint8_t nCHRBanks = 0;  // Número de bancos CHR
    MIRROR hw_mirror = HORIZONTAL;  // Modo de espelhamento padrão
};