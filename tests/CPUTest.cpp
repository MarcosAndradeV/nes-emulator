#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include <functional>
#include "../src/Header_files/CPU.h"
#include "../src/Header_files/Bus.h"

// Simplified test bus that connects to the CPU
class TestBus : public Bus {
private:
    uint8_t ram[65536];  // Full addressable memory

public:
    TestBus() {
        // Clear RAM
        for (int i = 0; i < 65536; i++)
            ram[i] = 0x00;
    }

    // Interface for CPU
    uint8_t cpuRead(uint16_t addr, bool readOnly = false) override {
        return ram[addr];
    }

    void cpuWrite(uint16_t addr, uint8_t data) override {
        ram[addr] = data;
    }

    // Helpers for testing
    void loadProgram(uint16_t address, const std::vector<uint8_t>& program) {
        for (size_t i = 0; i < program.size(); i++) {
            ram[address + i] = program[i];
        }
    }

    void setReset(uint16_t resetVector) {
        ram[0xFFFC] = resetVector & 0xFF;
        ram[0xFFFD] = (resetVector >> 8) & 0xFF;
    }

    void setIRQ(uint16_t irqVector) {
        ram[0xFFFE] = irqVector & 0xFF;
        ram[0xFFFF] = (irqVector >> 8) & 0xFF;
    }

    void setNMI(uint16_t nmiVector) {
        ram[0xFFFA] = nmiVector & 0xFF;
        ram[0xFFFB] = (nmiVector >> 8) & 0xFF;
    }

    uint8_t peek(uint16_t addr) {
        return ram[addr];
    }
};

// Struct to hold expected CPU state for comparison
struct CPUState {
    uint8_t a;
    uint8_t x;
    uint8_t y;
    uint8_t stkp;
    uint16_t pc;
    uint8_t status;
    int cycles;
};

class CPU6502Tester {
private:
    TestBus bus;
    olc6502 cpu;
    bool verbose;
    int testsPassed = 0;
    int testsFailed = 0;

    std::string getStatusString(uint8_t status) {
        std::string flags = "";
        flags += (status & olc6502::N) ? "N" : "-";
        flags += (status & olc6502::V) ? "V" : "-";
        flags += (status & olc6502::U) ? "U" : "-";
        flags += (status & olc6502::B) ? "B" : "-";
        flags += (status & olc6502::D) ? "D" : "-";
        flags += (status & olc6502::I) ? "I" : "-";
        flags += (status & olc6502::Z) ? "Z" : "-";
        flags += (status & olc6502::C) ? "C" : "-";
        return flags;
    }

    void printCPUState() {
        std::cout << "A: " << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(cpu.a) 
                  << " X: " << std::setw(2) << static_cast<int>(cpu.x)
                  << " Y: " << std::setw(2) << static_cast<int>(cpu.y)
                  << " SP: " << std::setw(2) << static_cast<int>(cpu.stkp)
                  << " PC: " << std::setw(4) << cpu.pc
                  << " Status: " << getStatusString(cpu.status)
                  << " (" << std::setw(2) << static_cast<int>(cpu.status) << ")"
                  << std::dec << std::endl;
    }

    bool compareState(const CPUState& expected) {
        bool passed = true;
        if (cpu.a != expected.a) {
            std::cout << "FAIL: A register - Expected: " << std::hex << static_cast<int>(expected.a) 
                      << " Actual: " << static_cast<int>(cpu.a) << std::dec << std::endl;
            passed = false;
        }
        if (cpu.x != expected.x) {
            std::cout << "FAIL: X register - Expected: " << std::hex << static_cast<int>(expected.x) 
                      << " Actual: " << static_cast<int>(cpu.x) << std::dec << std::endl;
            passed = false;
        }
        if (cpu.y != expected.y) {
            std::cout << "FAIL: Y register - Expected: " << std::hex << static_cast<int>(expected.y) 
                      << " Actual: " << static_cast<int>(cpu.y) << std::dec << std::endl;
            passed = false;
        }
        if (cpu.stkp != expected.stkp) {
            std::cout << "FAIL: Stack Pointer - Expected: " << std::hex << static_cast<int>(expected.stkp) 
                      << " Actual: " << static_cast<int>(cpu.stkp) << std::dec << std::endl;
            passed = false;
        }
        if (cpu.pc != expected.pc) {
            std::cout << "FAIL: Program Counter - Expected: " << std::hex << expected.pc 
                      << " Actual: " << cpu.pc << std::dec << std::endl;
            passed = false;
        }
        if (cpu.status != expected.status) {
            std::cout << "FAIL: Status register - Expected: " << getStatusString(expected.status) 
                      << " Actual: " << getStatusString(cpu.status) << std::endl;
            passed = false;
        }
        return passed;
    }

public:
    CPU6502Tester(bool verboseOutput = false) : verbose(verboseOutput) {
        cpu.ConnectBus(&bus);
    }

    // Run a test with program at specified address and compare final state
    bool runTest(const std::string& testName, uint16_t programAddress, 
                 const std::vector<uint8_t>& program, const CPUState& expectedState) {
        std::cout << "Running test: " << testName << std::endl;
        
        // Setup test environment
        bus.loadProgram(programAddress, program);
        bus.setReset(programAddress);
        
        // Reset CPU to known state
        cpu.reset();
        
        // Force specific initial state if needed by the test
        // (Note: Real reset would set registers to specific values)
        
        // Run for expected number of cycles plus some margin
        int totalCycles = expectedState.cycles;
        for (int i = 0; i < totalCycles; i++) {
            if (cpu.complete()) {
                int opcode = bus.peek(cpu.pc);
                if (verbose) {
                    std::cout << "Executing opcode 0x" << std::hex << opcode 
                              << " at PC=0x" << cpu.pc << std::dec << std::endl;
                }
            }
            cpu.clock();
        }
        
        if (verbose) {
            std::cout << "Final CPU state:" << std::endl;
            printCPUState();
        }
        
        // Check results
        bool passed = compareState(expectedState);
        if (passed) {
            std::cout << "TEST PASSED: " << testName << std::endl << std::endl;
            testsPassed++;
        } else {
            std::cout << "TEST FAILED: " << testName << std::endl << std::endl;
            testsFailed++;
        }
        return passed;
    }

    // Test a specific addressing mode
    bool testAddressingMode(const std::string& modeName, uint8_t opcode,
                           const std::vector<uint8_t>& setupInstructions,
                           uint16_t expectedAddr) {
        std::cout << "Testing addressing mode: " << modeName << std::endl;
        
        // Setup memory with test code
        uint16_t programAddr = 0x1000;
        std::vector<uint8_t> program = setupInstructions;
        program.push_back(opcode); // Add the opcode for the addressing mode test
        
        bus.loadProgram(programAddr, program);
        bus.setReset(programAddr);
        
        // Reset CPU
        cpu.reset();
        
        // Execute until we reach the test opcode
        while (bus.peek(cpu.pc) != opcode) {
            if (cpu.complete()) {
                if (verbose) {
                    std::cout << "Setup: Executing " << std::hex << static_cast<int>(bus.peek(cpu.pc)) 
                              << " at PC=" << cpu.pc << std::dec << std::endl;
                }
            }
            cpu.clock();
        }
        
        // Execute one more instruction (the test opcode)
        while (!cpu.complete()) {
            cpu.clock();
        }
        
        // clock() once more to advance to the next instruction
        cpu.clock();
        
        // Check if addr_abs is what we expected
        bool addrCorrect = (cpu.addr_abs == expectedAddr);
        if (addrCorrect) {
            std::cout << "TEST PASSED: " << modeName << " addressing mode correctly calculated 0x" 
                      << std::hex << expectedAddr << std::dec << std::endl << std::endl;
            testsPassed++;
        } else {
            std::cout << "TEST FAILED: " << modeName << " addressing mode - Expected address: 0x" 
                      << std::hex << expectedAddr << " Actual address: 0x" << cpu.addr_abs << std::dec << std::endl << std::endl;
            testsFailed++;
        }
        
        return addrCorrect;
    }

    // Run a test with a specific instruction
    bool testInstruction(const std::string& instrName, uint8_t opcode,
                        const std::vector<uint8_t>& setupInstructions,
                        const CPUState& initialState,
                        const CPUState& expectedState) {
        std::cout << "Testing instruction: " << instrName << std::endl;
        
        // Setup memory with test sequence
        uint16_t programAddr = 0x1000;
        std::vector<uint8_t> program = setupInstructions;
        program.push_back(opcode); // Add the test instruction
        
        bus.loadProgram(programAddr, program);
        bus.setReset(programAddr);
        
        // Reset CPU
        cpu.reset();
        
        // Setup initial state if needed
        cpu.a = initialState.a;
        cpu.x = initialState.x;
        cpu.y = initialState.y;
        cpu.status = initialState.status | olc6502::U; // Set unused flag
        cpu.stkp = initialState.stkp;
        
        // Execute setup instructions
        while (bus.peek(cpu.pc) != opcode) {
            if (cpu.complete()) {
                if (verbose) {
                    std::cout << "Setup: Executing " << std::hex << static_cast<int>(bus.peek(cpu.pc)) 
                              << " at PC=" << cpu.pc << std::dec << std::endl;
                }
            }
            cpu.clock();
        }
        
        if (verbose) {
            std::cout << "Before instruction execution:" << std::endl;
            printCPUState();
        }
        
        // Execute the test instruction
        int cycles = 0;
        do {
            cpu.clock();
            cycles++;
        } while (!cpu.complete());
        
        if (verbose) {
            std::cout << "After instruction execution (took " << cycles << " cycles):" << std::endl;
            printCPUState();
        }
        
        // Check results
        bool passed = compareState(expectedState);
        if (passed) {
            std::cout << "TEST PASSED: " << instrName << " instruction" << std::endl << std::endl;
            testsPassed++;
        } else {
            std::cout << "TEST FAILED: " << instrName << " instruction" << std::endl << std::endl;
            testsFailed++;
        }
        
        return passed;
    }

    // Test stack operations
    bool testStackOperations() {
        std::cout << "Testing stack operations..." << std::endl;
        
        uint16_t programAddr = 0x1000;
        // PHA, PHP, PLA, PLP test sequence
        std::vector<uint8_t> program = {
            0xA9, 0x42,     // LDA #$42
            0x48,           // PHA (push A to stack)
            0x08,           // PHP (push status to stack)
            0xA9, 0x00,     // LDA #$00 (clear A)
            0x68,           // PLA (pull A from stack)
            0x28            // PLP (pull status from stack)
        };
        
        bus.loadProgram(programAddr, program);
        bus.setReset(programAddr);
        
        // Reset CPU
        cpu.reset();
        
        // Initial stack pointer (after reset)
        uint8_t initialSP = cpu.stkp;
        
        // Run the program
        int maxCycles = 20; // More than enough for our program
        for (int i = 0; i < maxCycles && cpu.pc < programAddr + program.size(); i++) {
            if (cpu.complete()) {
                if (verbose) {
                    std::cout << "Executing at PC=0x" << std::hex << cpu.pc << std::dec << std::endl;
                    printCPUState();
                }
            }
            cpu.clock();
        }
        
        // Check if A was correctly restored from stack
        bool passed = (cpu.a == 0x42);
        
        // Check if stack pointer is back to initial value
        // We pushed twice and pulled twice, so it should be the same
        passed &= (cpu.stkp == initialSP);
        
        if (passed) {
            std::cout << "TEST PASSED: Stack operations" << std::endl << std::endl;
            testsPassed++;
        } else {
            std::cout << "TEST FAILED: Stack operations" << std::endl;
            std::cout << "  Final A: " << std::hex << static_cast<int>(cpu.a) << std::dec << std::endl;
            std::cout << "  Initial SP: " << static_cast<int>(initialSP) << ", Final SP: " << static_cast<int>(cpu.stkp) << std::endl << std::endl;
            testsFailed++;
        }
        
        return passed;
    }

    // Test interrupt handling
    bool testInterrupts() {
        std::cout << "Testing interrupt handling..." << std::endl;
        
        uint16_t programAddr = 0x1000;
        uint16_t irqHandlerAddr = 0x2000;
        uint16_t nmiHandlerAddr = 0x3000;
        
        // Main program
        std::vector<uint8_t> mainProgram = {
            0x78,           // SEI (set interrupt disable)
            0xA9, 0x42,     // LDA #$42
            0x58,           // CLI (clear interrupt disable)
            0xEA, 0xEA      // NOP, NOP (padding)
        };
        
        // IRQ handler
        std::vector<uint8_t> irqHandler = {
            0xA9, 0x69,     // LDA #$69
            0x40            // RTI (return from interrupt)
        };
        
        // NMI handler
        std::vector<uint8_t> nmiHandler = {
            0xA9, 0x55,     // LDA #$55
            0x40            // RTI (return from interrupt)
        };
        
        // Load programs to memory
        bus.loadProgram(programAddr, mainProgram);
        bus.loadProgram(irqHandlerAddr, irqHandler);
        bus.loadProgram(nmiHandlerAddr, nmiHandler);
        
        // Setup vectors
        bus.setReset(programAddr);
        bus.setIRQ(irqHandlerAddr);
        bus.setNMI(nmiHandlerAddr);
        
        bool irqPassed = false;
        bool nmiPassed = false;

        // Test IRQ
        {
            // Reset CPU
            cpu.reset();
            
            // Run until CLI
            while (bus.peek(cpu.pc) != 0x58) {
                cpu.clock();
            }
            // Execute CLI
            do { cpu.clock(); } while (!cpu.complete());
            
            // Trigger IRQ
            cpu.irq();
            
            // Run until RTI
            while (bus.peek(cpu.pc) != 0x40) {
                cpu.clock();
            }
            // Execute RTI
            do { cpu.clock(); } while (!cpu.complete());
            
            // Check if A was set by IRQ handler
            irqPassed = (cpu.a == 0x69);
            
            if (irqPassed) {
                std::cout << "TEST PASSED: IRQ handling" << std::endl;
                testsPassed++;
            } else {
                std::cout << "TEST FAILED: IRQ handling - A = " << std::hex << static_cast<int>(cpu.a) << std::dec << std::endl;
                testsFailed++;
            }
        }
        
        // Test NMI
        {
            // Reset CPU
            cpu.reset();
            
            // Run a few instructions
            for (int i = 0; i < 3; i++) {
                do { cpu.clock(); } while (!cpu.complete());
            }
            
            // Trigger NMI
            cpu.nmi();
            
            // Run until RTI
            while (bus.peek(cpu.pc) != 0x40) {
                cpu.clock();
            }
            // Execute RTI
            do { cpu.clock(); } while (!cpu.complete());
            
            // Check if A was set by NMI handler
            nmiPassed = (cpu.a == 0x55);
            
            if (nmiPassed) {
                std::cout << "TEST PASSED: NMI handling" << std::endl << std::endl;
                testsPassed++;
            } else {
                std::cout << "TEST FAILED: NMI handling - A = " << std::hex << static_cast<int>(cpu.a) << std::dec << std::endl << std::endl;
                testsFailed++;
            }
        }

        return irqPassed && nmiPassed;
    }

    // Run a full test suite
    void runTestSuite() {
        std::cout << "====== 6502 CPU Test Suite ======" << std::endl;

        // Test basic instructions
        bool allTestsPassed = true;
        allTestsPassed &= testLDAImmediate();
        allTestsPassed &= testADCImmediate();
        testBranchInstructions();
        allTestsPassed &= testStackOperations();
        allTestsPassed &= testInterrupts();
        testAddressingModes();

        // Report test results
        std::cout << "==== Test Results Summary ====" << std::endl;
        std::cout << "Tests passed: " << testsPassed << std::endl;
        std::cout << "Tests failed: " << testsFailed << std::endl;
        if (!allTestsPassed) {
            std::cout << "Some tests failed. Check the logs above for details." << std::endl;
        }
        std::cout << "============================" << std::endl;
    }

    // Example instruction tests
    bool testLDAImmediate() {
        // Test LDA with immediate addressing
        // LDA #$42 should load 0x42 into A and set appropriate flags
        
        // Setup
        CPUState initialState = {0x00, 0x00, 0x00, 0xFD, 0x1000, 0x00, 0};
        
        // Expected state after LDA #$42
        CPUState expectedState = {0x42, 0x00, 0x00, 0xFD, 0x1002, 0x00 | olc6502::U, 2};
        
        // Run test
        return testInstruction("LDA Immediate", 0xA9, {0x42}, initialState, expectedState);
    }

    bool testADCImmediate() {
        // Test ADC with immediate addressing
        // ADC #$40 should add 0x40 to A and set appropriate flags
        
        // Setup - A starts with 0x02
        CPUState initialState = {0x02, 0x00, 0x00, 0xFD, 0x1000, 0x00, 0};
        
        // Expected state after ADC #$40 - A becomes 0x42, no carry or other flags set
        CPUState expectedState = {0x42, 0x00, 0x00, 0xFD, 0x1002, 0x00 | olc6502::U, 2};
        
        // Run test
        return testInstruction("ADC Immediate", 0x69, {0x40}, initialState, expectedState);
    }

    void testBranchInstructions() {
        // Test the branch instructions (BEQ, BNE, etc.)
        
        // Test BEQ when Z=1 (should branch)
        {
            CPUState initialState = {0x00, 0x00, 0x00, 0xFD, 0x1000, 0x00 | olc6502::Z, 0};
            // Branch to PC+4 (0x1004)
            CPUState expectedState = {0x00, 0x00, 0x00, 0xFD, 0x1004, 0x00 | olc6502::Z | olc6502::U, 3};
            testInstruction("BEQ with Z=1", 0xF0, {0x02}, initialState, expectedState);
        }
        
        // Test BEQ when Z=0 (should not branch)
        {
            CPUState initialState = {0x01, 0x00, 0x00, 0xFD, 0x1000, 0x00, 0};
            // Should not branch, PC just advances to next instruction
            CPUState expectedState = {0x01, 0x00, 0x00, 0xFD, 0x1002, 0x00 | olc6502::U, 2};
            testInstruction("BEQ with Z=0", 0xF0, {0x02}, initialState, expectedState);
        }
    }

    void testAddressingModes() {
        // Test ZeroPage
        testAddressingMode("ZeroPage", 0xA5, {}, 0x00FF);
        
        // Test ZeroPageX
        {
            // Set X to 0x10 first
            CPUState initialState = {0x00, 0x10, 0x00, 0xFD, 0x1000, 0x00, 0};
            // ZP address 0x42 + X(0x10) = 0x52
            CPUState expectedState = {0x00, 0x10, 0x00, 0xFD, 0x1002, 0x00 | olc6502::U, 0};
            testInstruction("ZeroPageX", 0xB5, {0x42}, initialState, expectedState);
        }
        
        // Test Absolute
        testAddressingMode("Absolute", 0xAD, {0x34, 0x12}, 0x1234);
        
        // Test AbsoluteX
        {
            CPUState initialState = {0x00, 0x10, 0x00, 0xFD, 0x1000, 0x00, 0};
            // ABS address 0x1234 + X(0x10) = 0x1244
            CPUState expectedState = {0x00, 0x10, 0x00, 0xFD, 0x1003, 0x00 | olc6502::U, 0};
            testInstruction("AbsoluteX", 0xBD, {0x34, 0x12}, initialState, expectedState);
        }
        
        // Can add more addressing mode tests as needed
    }
};

int main(int argc, char* argv[]) {
    bool verbose = (argc > 1 && std::string(argv[1]) == "-v");
    
    CPU6502Tester tester(verbose);
    tester.runTestSuite();
    
    return 0;
}