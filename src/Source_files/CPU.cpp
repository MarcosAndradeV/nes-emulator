#include "../../include/global.h"

class CPU6502{
    public:
        uint8_t A = 0;
        uint8_t X = 0;
        uint8_t Y = 0;
        uint16_t PC = 0;//program counter
        uint8_t SP = 0xFD;
        uint8_t Status = 0x34;//registrador de status(flags)

        uint8_t memory[0x100000]={0};//Memória simulada de 64kb

        void reset();//reinicia
        void clock();//executa um ciclo
        void interrupt();//trata uma interrupção IRQ
        void nmi();//trata uma interrupção NMI
        void executeInstruction(uint8_t opcode);//executa uma Instrução


        enum FLAGS{
            C = (1 << 0),//Carry
            Z = (1 << 1),//Zero
            I = (1 << 2),//Interrupt Disable
            D = (1 << 3),//Decimal mode
            B = (1 << 4),//Break
            U =(1 << 5),//Unused(normalmente fica fixo em 1)
            V =(1 << 6),//Overflow
            N = (1 << 7),//Negative
        };

void setFlag(FLAGS flag, bool value){
    if(value)
        Status |= flag;//ativa o bit correspondente
    else
        Status &= ~flag;//limpa o bit correspondente
}

bool getFlag(FLAGS flag){
    return (Status & flag)>0;
}
};//Registradores e Flags

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