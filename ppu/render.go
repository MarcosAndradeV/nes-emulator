package ppu

import (
	"unsafe"

	"github.com/veandco/go-sdl2/sdl"
)

func (ppu *PPU) renderPixel() {
	x := ppu.cycle - 1
	y := ppu.scanline

	if x >= 256 || y >= 240 || x < 0 || y < 0 {
		return
	}

	// 1. Calcular tileX, tileY, pixelX, pixelY
	tileX := x / 8
	tileY := y / 8
	pixelX := x % 8
	pixelY := y % 8

	// 2. Endereço base do Name Table 0 ($2000)
	nameTableAddr := 0x2000
	tileIndexAddr := nameTableAddr + tileY*32 + tileX
	tileIndex := ppu.vram[tileIndexAddr-0x2000]

	// 3. Endereço do Attribute Table
	attrTableAddr := nameTableAddr + 0x3C0 + (tileY/4)*8 + (tileX/4)
	attrByte := ppu.vram[attrTableAddr-0x2000]

	// 4. Selecionar bits da paleta para o bloco 2x2 tiles
	shift := uint(((tileY%4)/2)*2 + ((tileX%4)/2)*0)
	paletteBits := (attrByte >> shift) & 0x03

	// 5. Endereço do tile na CHR-ROM (pattern table 0)
	tileAddr := uint16(tileIndex) * 16
	plane0 := uint8(0)
	plane1 := uint8(0)
	if ppu.mapper != nil {
		plane0 = ppu.mapper.ReadCHR(tileAddr + uint16(pixelY))
		plane1 = ppu.mapper.ReadCHR(tileAddr + uint16(pixelY) + 8)
	}
	bit0 := (plane0 >> (7 - pixelX)) & 1
	bit1 := (plane1 >> (7 - pixelX)) & 1
	colorIndex := (paletteBits << 2) | (bit1 << 1) | bit0

	// 6. Buscar cor na paleta PPU ($3F00+)
	ppuPaletteIndex := ppu.palette[colorIndex&0x1F] & 0x3F
	color := ppu.nesPalette[ppuPaletteIndex]

	// Fallback: se cor de fundo (índice 0), força preto
	if colorIndex&0x1F == 0 || (color.R == 0 && color.G == 0 && color.B == 0) {
		color = sdl.Color{R: 0, G: 0, B: 0, A: 255}
	}

	// [DEBUG] Se tileIndex == 0, pintar de magenta para destacar tiles vazios
	if tileIndex == 0 {
		color = sdl.Color{R: 255, G: 0, B: 255, A: 255}
	}

	ppu.setPixel(x, y, color)
}

func (ppu *PPU) setPixel(x, y int, color sdl.Color) {
	idx := (y*256 + x) * 4
	if idx >= 0 && idx+3 < len(ppu.pixels) {
		ppu.pixels[idx] = color.R     // R
		ppu.pixels[idx+1] = color.G   // G
		ppu.pixels[idx+2] = color.B   // B
		ppu.pixels[idx+3] = color.A   // A
	}
}

func (ppu *PPU) getTileColor(x, y int) sdl.Color {
	tileX := x / 8
	tileY := y / 8
	pixelX := x % 8
	pixelY := y % 8
	tileAddr := uint16(tileY*32 + tileX) * 16
	if ppu.mapper != nil {
		plane0 := ppu.mapper.ReadCHR(tileAddr + uint16(pixelY))
		plane1 := ppu.mapper.ReadCHR(tileAddr + uint16(pixelY) + 8)
		bit0 := (plane0 >> (7 - pixelX)) & 1
		bit1 := (plane1 >> (7 - pixelX)) & 1
		colorIndex := (bit1 << 1) | bit0
		switch colorIndex {
		case 0:
			return sdl.Color{R: 0, G: 0, B: 0, A: 255}
		case 1:
			return sdl.Color{R: 128, G: 128, B: 128, A: 255}
		case 2:
			return sdl.Color{R: 200, G: 200, B: 200, A: 255}
		case 3:
			return sdl.Color{R: 255, G: 255, B: 255, A: 255}
		}
	}
	return sdl.Color{R: uint8((x + y) % 256), G: uint8(x % 256), B: uint8(y % 256), A: 255}
}

func (ppu *PPU) getBackgroundColor(x, y int) sdl.Color {
	r := uint8((x * 255) / 256)
	g := uint8((y * 255) / 240)
	b := uint8(((x + y) * 255) / (256 + 240))
	if ((x/16 + y/16) % 2) == 0 {
		r = r / 2
		g = g / 2
		b = b / 2
	}
	return sdl.Color{R: r, G: g, B: b, A: 255}
}

func (ppu *PPU) Render() {
	if ppu.texture == nil {
		return
	}
	ppu.texture.Update(nil, unsafe.Pointer(&ppu.pixels[0]), 256*4)
	dest := &sdl.Rect{X: 0, Y: 0, W: 512, H: 480}
	ppu.renderer.Copy(ppu.texture, nil, dest)
} 