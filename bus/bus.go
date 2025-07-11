package bus

import (
	"nes-emulator/cpu"
	"nes-emulator/memory"
)

// Bus conecta CPU, memória e (futuramente) PPU/APU
// e faz o roteamento de leitura/escrita

type Bus struct {
	CPU    *cpu.CPU
	Memory *memory.NESMemory
	// Futuro: PPU, APU, mappers, etc
}

func NewBus(mem *memory.NESMemory) *Bus {
	return &Bus{
		Memory: mem,
	}
}

// AttachCPU conecta a CPU ao bus
func (b *Bus) AttachCPU(cpu *cpu.CPU) {
	b.CPU = cpu
}

// Read delega leitura para a memória
func (b *Bus) Read(addr uint16) uint8 {
	return b.Memory.Read(addr)
}

// Write delega escrita para a memória
func (b *Bus) Write(addr uint16, value uint8) {
	b.Memory.Write(addr, value)
}
