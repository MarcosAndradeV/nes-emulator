#include "../../include/global.h"

// Tamanho da RAM do NES (2 KB = 2048 bytes)
#define RAM_SIZE 2048

// Construtor: inicializa a RAM com zeros
Memory::Memory() : ram(RAM_SIZE, 0) {}

// Lê um byte da memória
uint8_t Memory::read(uint16_t address) {
    if (address < RAM_SIZE)
    {
        return ram[address];  // Retorna da RAM principal
    }

    // Endereço fora da RAM → Pode ser um registro especial (PPU, APU, etc.)
    // Para um NES completo, aqui precisa de mais lógica.
    return 0;
}

// Escreve um byte na memória
void Memory::write(uint16_t address, uint8_t data)
{
    if (address < RAM_SIZE)
    {
        ram[address] = data;  // Escreve na RAM principal
    }

    // Se for um endereço especial (PPU, APU), precisa de mais lógica.
}
