package bus

import (
	"fmt"
	"nes-emulator/memory"
	"nes-emulator/ppu"

	"github.com/retroenv/retrogolib/arch/cpu/m6502"
)

// Garante que Bus implementa a interface BasicMemory do m6502
var _ m6502.BasicMemory = (*Bus)(nil)

type Bus struct {
	ram  [0x0800]byte  
	cart *memory.Cartridge
	ppu  *ppu.PPU
}

// New cria um novo barramento com o cartucho conectado
func New(cart *memory.Cartridge, ppu *ppu.PPU) *Bus {
	return &Bus{
		cart: cart,
		ppu:  ppu,
	}
}

// Read lê um byte do endereço fornecido
func (b *Bus) Read(addr uint16) uint8 {
	var value uint8
	switch {
	case addr < 0x2000: // RAM interna (espelhada)
		value = b.ram[addr%0x0800]
		if addr == 0x0000 || addr == 0x0001 || addr == 0x0002 {
			fmt.Printf("CPU RAM READ: $%04X = $%02X\n", addr, value)
		}
	case addr >= 0x2000 && addr < 0x4000: // Registradores PPU (espelhados)
		reg := 0x2000 + (addr % 8)
		value = b.ppu.ReadRegister(reg)
		if reg == 0x2002 {
			// fmt.Printf("CPU READ: PPUSTATUS ($2002) = $%02X\n", value) // Removido para não poluir o terminal
		}
	case addr == 0x4014: // OAM DMA
		value = 0
	case addr == 0x4015: // APU Status
		value = 0
	case addr == 0x4016: // Controller 1
		fmt.Println("CPU READ: Controller 1 ($4016)")
		value = 0x40 // Nenhum botão pressionado
	case addr == 0x4017: // Controller 2
		value = 0x40
	case addr >= 0x4020 && addr < 0x6000: // Expansion ROM
		value = 0
	case addr >= 0x6000 && addr < 0x8000: // SRAM
		value = 0
	case addr >= 0x8000: // PRG-ROM via mapper
		value = b.cart.Mapper.ReadPRG(addr)
		if addr >= 0xFFFC && addr <= 0xFFFF {
			fmt.Printf("CPU ROM READ (Reset Vector): $%04X = $%02X\n", addr, value)
		}
	}
	if addr >= 0x8000 && addr < 0x8100 {
		fmt.Printf("CPU READ: $%04X = $%02X\n", addr, value)
	}
	return value
}

// Write escreve um byte no endereço fornecido
func (b *Bus) Write(addr uint16, value uint8) {
	// Log para endereços interessantes
	if addr >= 0x8000 {
		fmt.Printf("[BUS] Write: addr=$%04X value=$%02X\n", addr, value)
	}
	
	switch {
	case addr < 0x2000: // RAM interna
		b.ram[addr%0x0800] = value
		// Log para acessos à RAM interna (apenas endereços específicos)
		if addr == 0x0000 || addr == 0x0001 || addr == 0x0002 {
			fmt.Printf("CPU RAM WRITE: $%04X = $%02X\n", addr, value)
		}
	case addr >= 0x2000 && addr < 0x4000: // Registradores PPU (espelhados)
		b.ppu.WriteRegister(0x2000+(addr%8), value)
	case addr == 0x4014: // OAM DMA
		b.ppu.OAMDMA(value, b.Read)
	case addr >= 0x4000 && addr <= 0x4013: // APU
		// TODO: Implementar APU
	case addr == 0x4015: // APU Status
		// TODO: Implementar APU
	case addr == 0x4016: // Controller strobe
		// TODO: Implementar controles
	case addr == 0x4017: // APU Frame Counter
		// TODO: Implementar APU
	case addr >= 0x6000 && addr < 0x8000: // SRAM
		// TODO: Implementar SRAM
	case addr >= 0x8000: // PRG-ROM via mapper
		b.cart.Mapper.WritePRG(addr, value)
	}
}
