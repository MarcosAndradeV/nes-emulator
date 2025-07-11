package cpu

import (
	"github.com/retroenv/retrogolib/arch/cpu/m6502"
)

type CPU struct {
	instance *m6502.CPU
}

// Memory é a interface que a CPU espera para acessar memória
// (bus ou memória direta devem implementar)
type Memory interface {
	Read(addr uint16) uint8
	Write(addr uint16, value uint8)
}

func New(mem Memory) *CPU {
	adapter := m6502.NewMemory(mem)
	return &CPU{instance: m6502.New(adapter)}
}

func (c *CPU) Step() error {
	// Aqui você chamaria a execução real da CPU
	return nil // placeholder
}

func (c *CPU) GetState() m6502.State {
	return c.instance.State()
}
