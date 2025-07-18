package ppu

func (ppu *PPU) IsVBlank() bool {
	return ppu.vblank
}

func (ppu *PPU) GetFrame() int {
	return ppu.frame
}

func (ppu *PPU) GetScanline() int {
	return ppu.scanline
}

func (ppu *PPU) GetCycle() int {
	return ppu.cycle
} 