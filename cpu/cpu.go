package cpu

import (
	"fmt"

	"github.com/retroenv/retrogolib/arch/cpu/m6502"
)

// CPU é o wrapper do núcleo 6502 para o NES
type CPU struct {
	Core *m6502.CPU
	Mem  *m6502.Memory
}

// New cria uma nova CPU 6502 para o NES
func New(mem m6502.BasicMemory) *CPU {
	memory := m6502.NewMemory(mem)
	cpu := m6502.New(memory)
	
	// Inicializar CPU corretamente - ler o vetor de reset
	resetLow := mem.Read(0xFFFC)
	resetHigh := mem.Read(0xFFFD)
	resetVector := uint16(resetHigh)<<8 | uint16(resetLow)
	
	// Definir PC para o vetor de reset
	cpu.PC = resetVector
	
	// Inicializar registradores (valores típicos do NES)
	cpu.A = 0
	cpu.X = 0
	cpu.Y = 0
	cpu.SP = 0xFD
	
	// Log do estado inicial da CPU
	fmt.Printf("CPU: Estado inicial - PC=$%04X (reset vector), A=$%02X, X=$%02X, Y=$%02X, SP=$%02X\n", 
		cpu.PC, cpu.A, cpu.X, cpu.Y, cpu.SP)
	
	return &CPU{
		Core: cpu,
		Mem:  memory,
	}
}

// Step executa um ciclo de instrução da CPU
func (c *CPU) Step() error {
	// Log do PC e opcode atual para debug
	opcode := c.Mem.Read(c.Core.PC)
	fmt.Printf("[CPU] PC=$%04X OPCODE=$%02X A=$%02X X=$%02X Y=$%02X SP=$%02X\n", c.Core.PC, opcode, c.Core.A, c.Core.X, c.Core.Y, c.Core.SP)
	return c.Core.Step()
}

// Run executa múltiplos ciclos
func (c *CPU) Run(cycles int) error {
	for i := 0; i < cycles; i++ {
		if err := c.Step(); err != nil {
			return err
		}
	}
	return nil
}

// GetPC retorna o contador de programa atual
func (c *CPU) GetPC() uint16 {
	return c.Core.PC
}

// GetA retorna o registrador A
func (c *CPU) GetA() byte {
	return c.Core.A
}

// GetX retorna o registrador X
func (c *CPU) GetX() byte {
	return c.Core.X
}

// GetY retorna o registrador Y
func (c *CPU) GetY() byte {
	return c.Core.Y
}

// GetSP retorna o ponteiro da pilha
func (c *CPU) GetSP() byte {
	return c.Core.SP
}

// GetStatus retorna o registrador de status
func (c *CPU) GetStatus() byte {
	return c.Core.GetFlags()
}

// TriggerNMI dispara uma interrupção NMI
func (c *CPU) TriggerNMI() {
	fmt.Println("CPU: NMI disparado!")
	c.Core.TriggerNMI()
}

// TriggerIRQ dispara uma interrupção IRQ
func (c *CPU) TriggerIRQ() {
	c.Core.TriggerIrq()
}

// GetCycles retorna o número de ciclos executados
func (c *CPU) GetCycles() uint64 {
	return c.Core.Cycles()
}
