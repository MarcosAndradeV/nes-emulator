package ppu

import "github.com/veandco/go-sdl2/sdl"

func (ppu *PPU) initPalette() {
	// Paleta NES básica (cores aproximadas)
	colors := []sdl.Color{
		{84, 84, 84, 255},   // 0x00 - Cinza escuro
		{0, 30, 116, 255},   // 0x01 - Azul escuro
		{8, 16, 144, 255},   // 0x02 - Azul
		{48, 0, 136, 255},   // 0x03 - Roxo escuro
		{68, 0, 100, 255},   // 0x04 - Roxo
		{92, 0, 48, 255},    // 0x05 - Vermelho escuro
		{84, 4, 0, 255},     // 0x06 - Vermelho
		{60, 24, 0, 255},    // 0x07 - Laranja escuro
		{32, 42, 0, 255},    // 0x08 - Verde escuro
		{8, 58, 0, 255},     // 0x09 - Verde
		{0, 64, 0, 255},     // 0x0A - Verde claro
		{0, 60, 0, 255},     // 0x0B - Verde amarelado
		{0, 50, 60, 255},    // 0x0C - Verde azulado
		{0, 0, 0, 255},      // 0x0D - Transparente
		{0, 0, 0, 255},      // 0x0E - Transparente
		{0, 0, 0, 255},      // 0x0F - Transparente
		{152, 150, 152, 255}, // 0x10 - Cinza claro
		{8, 76, 196, 255},   // 0x11 - Azul claro
		{48, 50, 236, 255},  // 0x12 - Azul brilhante
		{92, 30, 228, 255},  // 0x13 - Roxo claro
		{136, 20, 176, 255}, // 0x14 - Roxo médio
		{160, 20, 100, 255}, // 0x15 - Rosa escuro
		{152, 34, 32, 255},  // 0x16 - Vermelho médio
		{120, 60, 0, 255},   // 0x17 - Laranja
		{84, 90, 0, 255},    // 0x18 - Verde amarelado
		{40, 114, 0, 255},   // 0x19 - Verde brilhante
		{8, 124, 0, 255},    // 0x1A - Verde claro
		{0, 118, 40, 255},   // 0x1B - Verde azulado claro
		{0, 102, 120, 255},  // 0x1C - Verde azulado
		{0, 0, 0, 255},      // 0x1D - Transparente
		{0, 0, 0, 255},      // 0x1E - Transparente
		{0, 0, 0, 255},      // 0x1F - Transparente
		{236, 238, 236, 255}, // 0x20 - Branco
		{76, 154, 236, 255}, // 0x21 - Azul muito claro
		{120, 124, 236, 255}, // 0x22 - Azul claro
		{176, 98, 236, 255}, // 0x23 - Roxo claro
		{228, 84, 228, 255}, // 0x24 - Rosa
		{236, 88, 136, 255}, // 0x25 - Rosa claro
		{236, 106, 64, 255}, // 0x26 - Laranja claro
		{228, 160, 32, 255}, // 0x27 - Amarelo escuro
		{160, 170, 0, 255},  // 0x28 - Verde amarelado claro
		{108, 184, 0, 255},  // 0x29 - Verde amarelado brilhante
		{56, 204, 108, 255}, // 0x2A - Verde azulado brilhante
		{56, 180, 204, 255}, // 0x2B - Verde azulado claro
		{60, 60, 60, 255},   // 0x2C - Cinza
		{0, 0, 0, 255},      // 0x2D - Transparente
		{0, 0, 0, 255},      // 0x2E - Transparente
		{0, 0, 0, 255},      // 0x2F - Transparente
		{236, 238, 236, 255}, // 0x30 - Branco
		{168, 204, 236, 255}, // 0x31 - Azul muito claro
		{188, 188, 236, 255}, // 0x32 - Azul claro
		{212, 178, 236, 255}, // 0x33 - Roxo claro
		{236, 174, 236, 255}, // 0x34 - Rosa claro
		{236, 174, 212, 255}, // 0x35 - Rosa muito claro
		{236, 180, 176, 255}, // 0x36 - Rosa amarelado
		{228, 196, 144, 255}, // 0x37 - Amarelo claro
		{204, 210, 120, 255}, // 0x38 - Verde amarelado claro
		{180, 222, 120, 255}, // 0x39 - Verde amarelado brilhante
		{168, 226, 144, 255}, // 0x3A - Verde azulado brilhante
		{152, 226, 180, 255}, // 0x3B - Verde azulado claro
		{160, 214, 228, 255}, // 0x3C - Verde azulado muito claro
		{160, 162, 160, 255}, // 0x3D - Cinza claro
		{0, 0, 0, 255},      // 0x3E - Transparente
		{0, 0, 0, 255},      // 0x3F - Transparente
	}

	copy(ppu.nesPalette[:], colors)

	// Inicializar paleta PPU (ppu.palette) com preto nos índices de fundo
	ppu.palette[0x00] = 0x00
	ppu.palette[0x10] = 0x00
} 