#include "src/Header_files/CPU.h"
#include "src/Header_files/Bus.h"
#include <iostream>

CPU6502::CPU6502()
{
    // Inicializa os registradores
    A = 0;
    X = 0;
    Y = 0;
    PC = 0;
    SP = 0xFD;
    Status = 0x34;
    bus = nullptr;
}

CPU6502::~CPU6502()
{
    // Limpa recursos se necessário
}

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
            std::cerr << "Instrução desconhecida: 0x" << std::hex << (int)opcode << "\n";
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
/*
// void CPU::execute() {
//   if (do_nmi) {
//     NMI();
//     return;
//   }//Executa uma interrupção não mascarável.

//   if (do_irq && !P.I) {
//     IRQ();
//     return;
//   }//Se (do_irq)está ativo e a flag (P.I)está desativada, executa uma interrupção normal.

//   uint8_t op = mem_read(PC++);
//   //Lê o próximo byte de memória apontado por PC e incrementa o PC para apontar para a próxima instrução.

//   uint16_t addr;
//   switch (op) {//Define um padrão para expandir casos de switch.
// #define X(opcode, op, mode) \
//   case opcode:              \
//     addr = mode();          \
//     op(addr);               \
//     break;

// #include "instructions.h"

// #undef X
//   }
// }
*/
