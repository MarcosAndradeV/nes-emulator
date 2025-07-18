package ppu

import "fmt"

func (ppu *PPU) ReadRegister(address uint16) uint8 {
	switch address {
	case 0x2000: // PPUCTRL
		return ppu.ctrl
	case 0x2001: // PPUMASK
		return ppu.mask
	case 0x2002: // PPUSTATUS
		fmt.Printf("PPU: PPUSTATUS lido = $%02X\n", ppu.status)
		result := ppu.status
		// Limpar VBlank flag após leitura
		ppu.status &= 0x7F
		ppu.nmiOccurred = false
		ppu.addressLatch = false
		return result
	case 0x2003: // OAMADDR
		return ppu.oamAddr
	case 0x2004: // OAMDATA
		return ppu.oam[ppu.oamAddr]
	case 0x2005: // PPUSCROLL
		return 0 // Write-only
	case 0x2006: // PPUADDR
		return 0 // Write-only
	case 0x2007: // PPUDATA
		return ppu.readData()
	default:
		return 0
	}
}

func (ppu *PPU) WriteRegister(address uint16, value uint8) {
	switch address {
	case 0x2000: // PPUCTRL
		ppu.ctrl = value
		fmt.Printf("PPU: PPUCTRL = $%02X\n", value)
	case 0x2001: // PPUMASK
		ppu.mask = value
		fmt.Printf("PPU: PPUMASK = $%02X\n", value)
	case 0x2002: // PPUSTATUS
		// Read-only
	case 0x2003: // OAMADDR
		ppu.oamAddr = value
		fmt.Printf("PPU: OAMADDR = $%02X\n", value)
	case 0x2004: // OAMDATA
		ppu.oam[ppu.oamAddr] = value
		ppu.oamAddr++
		fmt.Printf("PPU: OAMDATA[$%02X] = $%02X\n", ppu.oamAddr-1, value)
	case 0x2005: // PPUSCROLL
		if !ppu.addressLatch {
			ppu.fineXScroll = value & 0x07
			ppu.tempAddr = (ppu.tempAddr & 0xFFE0) | (uint16(value) >> 3)
		} else {
			ppu.tempAddr = (ppu.tempAddr & 0x8FFF) | ((uint16(value) & 0x07) << 12)
			ppu.tempAddr = (ppu.tempAddr & 0xFC1F) | ((uint16(value) & 0xF8) << 2)
		}
		ppu.addressLatch = !ppu.addressLatch
		fmt.Printf("PPU: PPUSCROLL = $%02X (latch=%v)\n", value, ppu.addressLatch)
	case 0x2006: // PPUADDR
		if !ppu.addressLatch {
			ppu.tempAddr = (ppu.tempAddr & 0x80FF) | ((uint16(value) & 0x3F) << 8)
		} else {
			ppu.tempAddr = (ppu.tempAddr & 0xFF00) | uint16(value)
			ppu.vramAddr = ppu.tempAddr
		}
		ppu.addressLatch = !ppu.addressLatch
		fmt.Printf("PPU: PPUADDR = $%02X -> vramAddr=$%04X (latch=%v)\n", value, ppu.vramAddr, ppu.addressLatch)
	case 0x2007: // PPUDATA
		fmt.Printf("PPU: PPUDATA = $%02X (addr=$%04X)\n", value, ppu.vramAddr)
		ppu.writeData(value)
		if (ppu.ctrl & 0x04) == 0 {
			ppu.vramAddr++
		} else {
			ppu.vramAddr += 32
		}
	}
}

func (ppu *PPU) readData() uint8 {
	address := ppu.vramAddr & 0x3FFF

	if address < 0x3F00 {
		// Buffered read
		result := ppu.dataBuffer
		ppu.dataBuffer = ppu.vram[address&0x07FF]
		return result
	} else {
		// Palette - immediate read
		return ppu.palette[address&0x1F]
	}
}

func (ppu *PPU) writeData(value uint8) {
	address := ppu.vramAddr & 0x3FFF

	if address < 0x3F00 {
		ppu.vram[address&0x07FF] = value
		if address >= 0x2000 && address < 0x2400 {
			// Log para Name Table 0
			fmt.Printf("PPU: VRAM escrita em $%04X = $%02X\n", address, value)
		}
	} else {
		ppu.palette[address&0x1F] = value
		if address >= 0x3F00 && address < 0x3F20 {
			// Log para Paleta
			fmt.Printf("PPU: PALETA escrita em $%04X = $%02X\n", address, value)
		}
	}
} 