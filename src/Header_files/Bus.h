#ifndef BUS_H
#define BUS_H

#include "../../include/global.h"

using namespace std;
class Bus
{
    public:
        Bus();
        ~Bus();

    public: 
    //Dispositivos no barramento
        CPU6502 cpu;
        PPU ppu; 
        APU2A03 apu; 
        shared_ptr<Cartridge> cart;  

        uint8_t cpuRam[2048]; //Memória do CPU

        uint8_t controller[2];

    //Sincronização com o sistema Áudio
    public:
        void SetSampleFrequency(uint32_t sample_rate); //taxa de amostragem
        double dAudioSample = 0.0;//amostra de áudio

    private:
        //Sincronização com o sistema Áudio
        double dAudioTime = 0.0;
        double dAudioGlobalTime = 0.0;
        double dAudioTimePerNESClock = 0.0;
        double dAudioTimePerSystemSample = 0.0f;


    public: // Main Bus Read & Write
        void    cpuWrite(uint16_t addr, uint8_t data);
        uint8_t cpuRead(uint16_t addr, bool bReadOnly = false);

    private:
       //Contador de clock do sistema
        uint32_t nSystemClockCounter = 0;
        uint8_t controller_state[2];//estado do controle

    private:
        uint8_t dma_page = 0x00;//pagina do DMA
        uint8_t dma_addr = 0x00;//endereço do DMA
        uint8_t dma_data = 0x00;//dado do DMA

        bool dma_dummy = true;

        bool dma_transfer = false;

    public: // System Interface
        void insertCartridge(const shared_ptr<Cartridge>& cartridge);// Connects a cartridge object to the internal buses
        void reset();// Resets the system
        bool clock();// Clocks the system - a single whole system tick
};

#endif // BUS.h
