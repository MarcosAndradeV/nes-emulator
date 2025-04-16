#include "../../include/global.h"


void CPU6502::clock(){
    uint8_t opcode = memory[PC++];
    executeInstruction(opcode);
}

void CPU6502::executeInstruction(uint8_t opcode){
    switch(opcode){
        case 0xA9:
            A = memory[PC++];
            setFlag(Z,A == 0);
            setFlag(N,A & 0x80);
        case 0x00:
            break;

        default:
            cerr << "Instrução desconhecida: 0x" << hex << (int)opcode << "\n";
            break;
    }
}

void CPU6502::reset(){
    A = X = Y = 0;
    SP= 0xFD;
    Status = 0x34;
    PC = memory[0xFFFC] | (memory[0xFFFD] << 8);//endereço do reset
}

void CPU6502::interrupt(){
    if(!getFlag(I)){
        memory[0x0100 + SP--] = (PC<<8) & 0xFF;//salva PC alto
        memory[0x0100 + SP--] = PC & 0xFF;//salva PC baixo
        memory[0x0100 + SP--] = Status;//salva status

        setFlag(I, true);

        PC = memory[0xFFFE] | (memory[0xFFFF] << 8);//vetor de IRQ
    }
}

void CPU6502::nmi(){
    memory[0x0100 + SP--] = (PC >> 8) & 0xFF;
    memory[0x0100 + SP--] = PC & 0xFF;
    memory[0x0100 + SP--] = Status;

    setFlag(I,true);

    PC = memory[0xFFFA] | (memory[0xFFFB] << 8);//vetor de NMI
}
