package cpu_test

import (
	"fmt"
	"testing"

	"github.com/retroenv/retrogolib/arch/cpu/m6502"
)

// Memória simples para teste
type TestMemory struct {
	data [65536]byte
}

func (m *TestMemory) Read(addr uint16) byte {
	return m.data[addr]
}

func (m *TestMemory) Write(addr uint16, val byte) {
	m.data[addr] = val
}

func TestLDAImmediate_PrintResult(t *testing.T) {
	mem := &TestMemory{}
	// Configura vetor de reset para 0x8000
	mem.data[0xFFFC] = 0x00
	mem.data[0xFFFD] = 0x80
	// Programa: LDA #$42
	mem.data[0x8000] = 0xA9 // LDA imediato
	mem.data[0x8001] = 0x42 // valor

	cpu := m6502.New(m6502.NewMemory(mem))

	// Inicia CPU
	cpu.Step()

	// Mostra o valor do registrador A
	fmt.Printf("Registrador A = 0x%X\n", cpu.A)

	// Verificação opcional
	if cpu.A != 0x42 {
		t.Errorf("Esperado A = 0x42, obtido A = 0x%X", cpu.A)
	}
}
