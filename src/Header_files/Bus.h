#ifndef BUS_H
#define BUS_H

#include "../../include/global.h"

class Bus
{
    public:
        Bus();
        ~Bus();

    public: //Dispositivos no barramento
        CPU6502 cpu;
        Memory memory;
        PPU ppu; //unidade de processamento de imagens
        APU2A03 apu; //unidade de processamento de áudio

    public: //Bus Read & Write
        void cpuWrite(uint16_t addr, uint8_t data);
        uint8_t cpuRead(uint16_t addr, bool bReadOnly = false);

};
#endif // BUS.h
