#include "../../include/global.h"


void CPU6502::clock(){
    uint8_t opcode = bus->cpuRead(PC++, false);
    executeInstruction(opcode);
}

void CPU6502::executeInstruction(uint8_t opcode){
    switch(opcode){
        case 0xA9:
            A = bus->cpuRead(PC++, false);
            setFlag(Z,A == 0);
            setFlag(N,A & 0x80);
            break;
        case 0x00:
            break;

        default:
            cerr << "Instrução desconhecida: 0x" << hex << (int)opcode << "\n";
            break;
    }
}

void CPU6502::reset(){
    A = X = Y = 0;
    SP = 0xFD;
    Status = 0x34;
    PC = bus->cpuRead(0xFFFC, false) | (bus->cpuRead(0xFFFD, false) << 8);//endereço do reset
}

void CPU6502::interrupt(){
    if(!getFlag(I)){
        bus->cpuWrite(0x0100 + SP--, (PC >> 8) & 0xFF);//salva PC alto
        bus->cpuWrite(0x0100 + SP--, PC & 0xFF);//salva PC baixo
        bus->cpuWrite(0x0100 + SP--, Status);//salva status

        setFlag(I, true);

        PC = bus->cpuRead(0xFFFE, false) | (bus->cpuRead(0xFFFF, false) << 8);//vetor de IRQ
    }
}

void CPU6502::nmi(){
    bus->cpuWrite(0x0100 + SP--, (PC >> 8) & 0xFF);
    bus->cpuWrite(0x0100 + SP--, PC & 0xFF);
    bus->cpuWrite(0x0100 + SP--, Status);

    setFlag(I,true);

    PC = bus->cpuRead(0xFFFA, false) | (bus->cpuRead(0xFFFB, false) << 8);//vetor de NMI
}
