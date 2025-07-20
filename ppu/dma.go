package ppu

func (ppu *PPU) OAMDMA(page uint8, busRead func(uint16) uint8) {
	address := uint16(page) << 8
	for i := 0; i < 256; i++ {
		ppu.oam[ppu.oamAddr] = busRead(address + uint16(i))
		ppu.oamAddr++
	}
} 