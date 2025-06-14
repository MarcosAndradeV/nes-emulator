package cpu

import (
	"nes-emulator/memory"
	"github.com/retroenv/retrogolib/arch/cpu/m6502"
)

type CPU struct {
	instance *m6502.CPU
}

func New(mem *memory.NESMemory) *CPU {
	adapter := m6502.NewMemory(mem)
	return &CPU{instance: m6502.New(adapter)}
}

func (c *CPU) Step() error {
	return c.instance.Step()
}

func (c *CPU) GetState() m6502.State {
	return c.instance.State()
}
