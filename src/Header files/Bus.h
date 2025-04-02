#ifndef BUS_H
#define BUS_H

#include <cstdint>//define vários tipos de inteiros que garantem tamanhos específicos
#include "CPU.h"
#include "Memory.h"
#include "PPU.h"
#include "APU.h"

class Bus
{
    public:
        Bus();
        ~Bus();
    
    public: //Dispositivos no barramento
        CPU cpu;
        Memory memory;
        PPU ppu; //unidade de processamento de imagens
        APU apu; //unidade de processamento de áudio

    public: //Bus Read & Write
        void cpuWrite(uint16_t addr, uint8_t data);
        uint8_t cpuRead(uint16_t addr, bool bReadOnly = false);

};
#endif // BUS.h