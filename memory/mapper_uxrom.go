package memory

type UxROM struct {
	prgROM   []uint8
	chrROM   []uint8
	bank     uint8
	mirroring Mirroring
}

func NewUxROM(prgROM, chrROM []uint8, mirroring Mirroring) *UxROM {
	return &UxROM{
		prgROM:   prgROM,
		chrROM:   chrROM,
		bank:     0,
		mirroring: mirroring,
	}
}

func (m *UxROM) ReadPRG(addr uint16) uint8 {
	if addr < 0x8000 {
		return 0
	}
	if addr < 0xC000 {
		bankOffset := int(m.bank) * 0x4000
		return m.prgROM[bankOffset+int(addr-0x8000)]
	}
	return m.prgROM[len(m.prgROM)-0x4000+int(addr-0xC000)]
}

func (m *UxROM) WritePRG(addr uint16, value uint8) {
	if addr >= 0x8000 && addr < 0xC000 {
		m.bank = value & 0x0F
	}
}

func (m *UxROM) ReadCHR(addr uint16) uint8 {
	if int(addr) < len(m.chrROM) {
		return m.chrROM[addr]
	}
	return 0
}

func (m *UxROM) WriteCHR(addr uint16, value uint8) {
	// UxROM normalmente usa CHR-ROM (não RAM)
}

func (m *UxROM) GetMirroring() Mirroring {
	return m.mirroring
} 