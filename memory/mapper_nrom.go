package memory

type NROM struct {
	prgROM []uint8
	chrROM []uint8
	mirroring Mirroring
}

func NewNROM(prgROM, chrROM []uint8, mirroring Mirroring) *NROM {
	return &NROM{
		prgROM:    prgROM,
		chrROM:    chrROM,
		mirroring: mirroring,
	}
}

func (m *NROM) ReadPRG(addr uint16) uint8 {
	if addr < 0x8000 {
		return 0
	}
	if len(m.prgROM) == 16384 {
		return m.prgROM[(addr-0x8000)%0x4000]
	} else {
		return m.prgROM[addr-0x8000]
	}
}

func (m *NROM) WritePRG(addr uint16, value uint8) {
	// NROM não suporta escrita na PRG-ROM
}

func (m *NROM) ReadCHR(addr uint16) uint8 {
	if addr >= 0x2000 {
		return 0
	}
	if len(m.chrROM) > 0 {
		return m.chrROM[addr]
	}
	return 0
}

func (m *NROM) WriteCHR(addr uint16, value uint8) {
	// NROM com CHR-ROM não suporta escrita
}

func (m *NROM) GetMirroring() Mirroring {
	return m.mirroring
} 