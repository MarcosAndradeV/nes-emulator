#include "src/Header_files/CPU.h"
#include "src/Header_files/Bus.h"
#include <iostream>

using namespace std;

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
    uint8_t cycles = 0;
    switch(opcode){
        case 0xA9:
            {A = bus->cpuRead(PC++, false);
            setFlag(Z,A == 0);
            setFlag(N,A & 0x80);
            break;}
        case 0xA5: // LDA Zero Page
           {A = bus->cpuRead(bus->cpuRead(PC++, false), false);
            setFlag(Z, A == 0);
            setFlag(N, A & 0x80);
            break;}

        case 0xA6: // LDX Zero Page
            {X = bus->cpuRead(bus->cpuRead(PC++, false), false);
            setFlag(Z, X == 0);
            setFlag(N, X & 0x80);
            break;}

        case 0xA2: // LDX Immediate
            {X = bus->cpuRead(PC++, false);
            setFlag(Z, X == 0);
            setFlag(N, X & 0x80);
            break;}

        case 0xA0: // LDY Immediate
            {Y = bus->cpuRead(PC++, false);
            setFlag(Z, Y == 0);
            setFlag(N, Y & 0x80);
            break;}

        case 0x4C: // JMP Absolute
            {PC = bus->cpuRead(PC++, false) | (bus->cpuRead(PC++, false) << 8);
            break;}

        case 0x6C: // JMP Indirect
            {
                uint16_t addr = bus->cpuRead(PC++, false) | (bus->cpuRead(PC++, false) << 8);
                PC = bus->cpuRead(addr, false) | (bus->cpuRead(addr + 1, false) << 8);
            }
            break;

        case 0x20: // JSR
            {
                uint16_t addr = bus->cpuRead(PC++, false) | (bus->cpuRead(PC++, false) << 8);
                bus->cpuWrite(0x0100 + SP--, (PC >> 8) & 0xFF);
                bus->cpuWrite(0x0100 + SP--, PC & 0xFF);
                PC = addr;
            }
            break;

        // ADC (Add with Carry)
        case 0x69: // ADC Immediate
            {
                uint8_t operand = bus->cpuRead(PC++, false);
                uint16_t result = A + operand + getFlag(C);
                setFlag(C, result > 0xFF);
                setFlag(Z, (result & 0xFF) == 0);
                setFlag(N, result & 0x80);
                setFlag(V, ((A ^ operand) & (A ^ (result & 0xFF)) & 0x80) != 0);
                A = result & 0xFF;
            }
            break;

        case 0x65: // ADC Zero Page
            {
                uint8_t addr = bus->cpuRead(PC++, false);
                uint8_t operand = bus->cpuRead(addr, false);
                uint16_t result = A + operand + getFlag(C);
                setFlag(C, result > 0xFF);
                setFlag(Z, (result & 0xFF) == 0);
                setFlag(N, result & 0x80);
                setFlag(V, ((A ^ operand) & (A ^ (result & 0xFF)) & 0x80) != 0);
                A = result & 0xFF;
            }
            break;

        // AND (Logical AND)
        case 0x29: // AND Immediate
            {A &= bus->cpuRead(PC++, false);
            setFlag(Z, A == 0);
            setFlag(N, A & 0x80);
            break;}

        case 0x25: // AND Zero Page
            {A &= bus->cpuRead(bus->cpuRead(PC++, false), false);
            setFlag(Z, A == 0);
            setFlag(N, A & 0x80);
            break;}

        // ASL (Arithmetic Shift Left)
        case 0x0A: // ASL Accumulator
            {setFlag(C, A & 0x80);
            A <<= 1;
            setFlag(Z, A == 0);
            setFlag(N, A & 0x80);
            break;}

        case 0x06: // ASL Zero Page
            {
                uint8_t addr = bus->cpuRead(PC++, false);
                uint8_t operand = bus->cpuRead(addr, false);
                setFlag(C, operand & 0x80);
                operand <<= 1;
                bus->cpuWrite(addr, operand);
                setFlag(Z, operand == 0);
                setFlag(N, operand & 0x80);
            }
            break;

        // BCC (Branch if Carry Clear)
        case 0x90: // BCC
            {if (!getFlag(C)) {
                int8_t offset = bus->cpuRead(PC++, false);
                PC += offset;
            } else {
                PC++;
            }
            break;}

        // BCS (Branch if Carry Set)
        case 0xB0: // BCS
            {if (getFlag(C)) {
                int8_t offset = bus->cpuRead(PC++, false);
                PC += offset;
            } else {
                PC++;
            }
            break;}

        // BEQ (Branch if Equal)
        case 0xF0: // BEQ
            {if (getFlag(Z)) {
                int8_t offset = bus->cpuRead(PC++, false);
                PC += offset;
            } else {
                PC++;
            }
            break;}

        // BMI (Branch if Minus)
        case 0x30: // BMI
            {if (getFlag(N)) {
                int8_t offset = bus->cpuRead(PC++, false);
                PC += offset;
            } else {
                PC++;
            }
            break;}

        // BNE (Branch if Not Equal)
        case 0xD0: // BNE
            {if (!getFlag(Z)) {
                int8_t offset = bus->cpuRead(PC++, false);
                PC += offset;
            } else {
                PC++;
            }
            break;}

        // BPL (Branch if Positive)
        case 0x10: // BPL
            {if (!getFlag(N)) {
                int8_t offset = bus->cpuRead(PC++, false);
                PC += offset;
            } else {
                PC++;
            }
            break;}

        // BRK (Break)
        case 0x00: // BRK
            {bus->cpuWrite(0x0100 + SP--, (PC >> 8) & 0xFF); // Salva PC alto
            bus->cpuWrite(0x0100 + SP--, PC & 0xFF);        // Salva PC baixo
            bus->cpuWrite(0x0100 + SP--, Status);           // Salva Status
            setFlag(B, true);
            PC = bus->cpuRead(0xFFFE, false) | (bus->cpuRead(0xFFFF, false) << 8); // Vetor de interrupção
            break;}

        // CLC (Clear Carry Flag)
        case 0x18: // CLC
            {setFlag(C, false);
            break;}

        // CLD (Clear Decimal Mode)
        case 0xD8: // CLD
            {setFlag(D, false);
            break;}

        // CLI (Clear Interrupt Disable)
        case 0x58: // CLI
            {setFlag(I, false);
            break;}

        // CLV (Clear Overflow Flag)
        case 0xB8: // CLV
            {setFlag(V, false);
            break;}

        // CMP (Compare Accumulator)
        case 0xC9: // CMP Immediate
            {
                uint8_t operand = bus->cpuRead(PC++, false);
                uint8_t result = A - operand;
                setFlag(C, A >= operand);
                setFlag(Z, result == 0);
                setFlag(N, result & 0x80);
            }
            break;

        // CPX (Compare X Register)
        case 0xE0: // CPX Immediate
            {
                uint8_t operand = bus->cpuRead(PC++, false);
                uint8_t result = X - operand;
                setFlag(C, X >= operand);
                setFlag(Z, result == 0);
                setFlag(N, result & 0x80);
            }
            break;

        // CPY (Compare Y Register)
        case 0xC0: // CPY Immediate
            {
                uint8_t operand = bus->cpuRead(PC++, false);
                uint8_t result = Y - operand;
                setFlag(C, Y >= operand);
                setFlag(Z, result == 0);
                setFlag(N, result & 0x80);
            }
            break;

        // DEC (Decrement Memory)
        case 0xC6: // DEC Zero Page
            {
                uint8_t addr = bus->cpuRead(PC++, false);
                uint8_t operand = bus->cpuRead(addr, false);
                operand--;
                bus->cpuWrite(addr, operand);
                setFlag(Z, operand == 0);
                setFlag(N, operand & 0x80);
            }
            break;

        // DEX (Decrement X Register)
        case 0xCA: // DEX
            {X--;
            setFlag(Z, X == 0);
            setFlag(N, X & 0x80);
            break;}

        // DEY (Decrement Y Register)
        case 0x88: // DEY
            {Y--;
            setFlag(Z, Y == 0);
            setFlag(N, Y & 0x80);
            break;}

        // EOR (Exclusive OR)
        case 0x49: // EOR Immediate
            {A ^= bus->cpuRead(PC++, false);
            setFlag(Z, A == 0);
            setFlag(N, A & 0x80);
            break;}

        // INC (Increment Memory)
        case 0xE6: // INC Zero Page
            {
                uint8_t addr = bus->cpuRead(PC++, false);
                uint8_t operand = bus->cpuRead(addr, false);
                operand++;
                bus->cpuWrite(addr, operand);
                setFlag(Z, operand == 0);
                setFlag(N, operand & 0x80);
            }
            break;

        // INX (Increment X Register)
        case 0xE8: // INX
            {X++;
            setFlag(Z, X == 0);
            setFlag(N, X & 0x80);
            break;}

        // INY (Increment Y Register)
        case 0xC8: // INY
            {Y++;
            setFlag(Z, Y == 0);
            setFlag(N, Y & 0x80);
            break;}

        // LSR (Logical Shift Right)
        case 0x4A: // LSR Accumulator
            {setFlag(C, A & 0x01);
            A >>= 1;
            setFlag(Z, A == 0);
            setFlag(N, false);
            break;}

        // LSR Zero Page
        case 0x46: // LSR Zero Page
            {
                uint8_t addr = bus->cpuRead(PC++, false);
                uint8_t operand = bus->cpuRead(addr, false);
                setFlag(C, operand & 0x01);
                operand >>= 1;
                bus->cpuWrite(addr, operand);
                setFlag(Z, operand == 0);
                setFlag(N, false);
            }
            break;

        // NOP (No Operation)
        case 0xEA: // NOP
            break;

        // ORA (Logical OR)
        case 0x09: // ORA Immediate
            {A |= bus->cpuRead(PC++, false);
            setFlag(Z, A == 0);
            setFlag(N, A & 0x80);
            break;}

        case 0x05: // ORA Zero Page
            {A |= bus->cpuRead(bus->cpuRead(PC++, false), false);
            setFlag(Z, A == 0);
            setFlag(N, A & 0x80);
            break;}

        // PHA (Push Accumulator)
        case 0x48: // PHA
            {bus->cpuWrite(0x0100 + SP--, A);
            break;}

        // PHP (Push Processor Status)
        case 0x08: // PHP
            {bus->cpuWrite(0x0100 + SP--, Status | 0x10); // Define o bit B
            break;}

        // PLA (Pull Accumulator)
        case 0x68: // PLA
            {A = bus->cpuRead(0x0100 + ++SP, false);
            setFlag(Z, A == 0);
            setFlag(N, A & 0x80);
            break;}

        // PLP (Pull Processor Status)
        case 0x28: // PLP
            {Status = bus->cpuRead(0x0100 + ++SP, false);
            break;}

        // ROL (Rotate Left)
        case 0x2A: // ROL Accumulator
            {
                bool carry = getFlag(C);
                setFlag(C, A & 0x80);
                A <<= 1;
                A |= carry;
                setFlag(Z, A == 0);
                setFlag(N, A & 0x80);
            }
            break;

        // ROR (Rotate Right)
        case 0x6A: // ROR Accumulator
            {
                bool carry = getFlag(C);
                setFlag(C, A & 0x01);
                A >>= 1;
                A |= (carry << 7);
                setFlag(Z, A == 0);
                setFlag(N, A & 0x80);
            }
        break;

        // RTI (Return from Interrupt)
        case 0x40: // RTI
            {
            Status = bus->cpuRead(0x0100 + ++SP, false);
            uint16_t pcLow = bus->cpuRead(0x0100 + ++SP, false);
            uint16_t pcHigh = bus->cpuRead(0x0100 + ++SP, false);
            PC = (pcHigh << 8) | pcLow;
            }
        break;
        
        // RTS (Return from Subroutine)
        case 0x60: // RTS
            {
            uint16_t pcLow2 = bus->cpuRead(0x0100 + ++SP, false);
            uint16_t pcHigh2 = bus->cpuRead(0x0100 + ++SP, false);
            PC = (pcHigh2 << 8) | pcLow2;
            PC++; // Retorna ao próximo endereço após o RTS
            }
        break;

        // SBC (Subtract with Carry)
        case 0xE9: // SBC Immediate
            {
                uint8_t operand = bus->cpuRead(PC++, false);
                uint16_t result = A - operand - (1 - getFlag(C));
                setFlag(C, result <= 0xFF);
                setFlag(Z, (result & 0xFF) == 0);
                setFlag(N, result & 0x80);
                setFlag(V, ((A ^ operand) & (A ^ (result & 0xFF)) & 0x80) != 0);
                A = result & 0xFF;
            }
        break;

        // SEC (Set Carry Flag)
        case 0x38: // SEC
            {
            setFlag(C, true);
            break;
            }
        // SED (Set Decimal Mode)
        case 0xF8: // SED
            {
            setFlag(D, true);
            break;
            }
        // SEI (Set Interrupt Disable)
        case 0x78: // SEI
            {
            setFlag(I, true);
            break;
            }
        // STA (Store Accumulator)
        case 0x85: // STA Zero Page
            {
            bus->cpuWrite(bus->cpuRead(PC++, false), A);
            break;
            }
        case 0x8D: // STA Absolute
            {
            bus->cpuWrite(bus->cpuRead(PC++, false) | (bus->cpuRead(PC++, false) << 8), A);
            break;
            }
        // STX (Store X Register)
        case 0x86: // STX Zero Page
            {bus->cpuWrite(bus->cpuRead(PC++, false), X);
            break;}

        case 0x8E: // STX Absolute
            {bus->cpuWrite(bus->cpuRead(PC++, false) | (bus->cpuRead(PC++, false) << 8), X);
            break;}

        // STY (Store Y Register)
        case 0x84: // STY Zero Page
            {bus->cpuWrite(bus->cpuRead(PC++, false), Y);
            break;}

        case 0x8C: // STY Absolute
            {bus->cpuWrite(bus->cpuRead(PC++, false) | (bus->cpuRead(PC++, false) << 8), Y);
            break;}

        // TAX (Transfer Accumulator to X)
        case 0xAA: // TAX
            {X = A;
            setFlag(Z, X == 0);
            setFlag(N, X & 0x80);
            break;}

        // TAY (Transfer Accumulator to Y)
        case 0xA8: // TAY
            {Y = A;
            setFlag(Z, Y == 0);
            setFlag(N, Y & 0x80);
            break;}

        // TSX (Transfer Stack Pointer to X)
        case 0xBA: // TSX
            {X = SP;
            setFlag(Z, X == 0);
            setFlag(N, X & 0x80);
            break;}

        // TXA (Transfer X to Accumulator)
        case 0x8A: // TXA
            {A = X;
            setFlag(Z, A == 0);
            setFlag(N, A & 0x80);
            break;}

        // TXS (Transfer X to Stack Pointer)
        case 0x9A: // TXS
            {SP = X;
            break;}

        // TYA (Transfer Y to Accumulator)
        case 0x98: // TYA
            {A = Y;
            setFlag(Z, A == 0);
            setFlag(N, A & 0x80);
            break;}

        case 0xDF:
        {
            uint16_t addr = bus->cpuRead(PC++, false);
            addr |= (bus->cpuRead(PC++, false) << 8);

            uint16_t effAddr = addr + X;

            if((addr & 0xFF00) != (effAddr & 0xFF00)){
                cycles++;
            }

            uint8_t value = bus->cpuRead(effAddr, false);

            value--;
            bus->cpuWrite(effAddr, value);
            
            uint8_t result = A - value;

            if(A>=value){
                Status |= C;
            }else{
                Status &= ~C;
            }

            if (A == value) {
                Status |= Z; 
            } else {
                Status &= ~Z; 
            }
    
            if (result & 0x80) {
                Status |= N;
            } else {
                Status &= ~N;
            }

            cycles += 7;
            break;
        }

        default:
            {cerr << "Instrução desconhecida: 0x" << std::hex << (int)opcode << "\n";
            break;}
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
