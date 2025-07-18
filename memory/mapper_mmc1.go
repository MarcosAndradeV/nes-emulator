package memory

type MMC1 struct {
	prgROM    []uint8
	chrROM    []uint8
	mirroring Mirroring

	shiftReg  uint8
	shiftCount uint8
	control   uint8
	prgBank   uint8
	chrBank0  uint8
	chrBank1  uint8
}

func NewMMC1(prgROM, chrROM []uint8, mirroring Mirroring) *MMC1 {
	return &MMC1{
		prgROM:    prgROM,
		chrROM:    chrROM,
		mirroring: mirroring,
		shiftReg:  0x10,
		control:   0x0C,
	}
}

func (m *MMC1) ReadPRG(addr uint16) uint8 {
	if addr < 0x8000 {
		return 0
	}
	prgSize := len(m.prgROM)
	bank := int(m.prgBank)
	mode := (m.control >> 2) & 0x03
	if prgSize <= 0x4000 {
		return m.prgROM[int(addr-0x8000)%prgSize]
	}
	if mode == 0 || mode == 1 {
		bank &= ^1
		return m.prgROM[(bank*0x4000)+int(addr-0x8000)]
	} else if mode == 2 {
		if addr < 0xC000 {
			return m.prgROM[int(addr-0x8000)]
		}
		return m.prgROM[(bank*0x4000)+int(addr-0xC000)]
	} else {
		if addr < 0xC000 {
			return m.prgROM[(bank*0x4000)+int(addr-0x8000)]
		}
		return m.prgROM[prgSize-0x4000+int(addr-0xC000)]
	}
}

func (m *MMC1) WritePRG(addr uint16, value uint8) {
	if addr < 0x8000 {
		return
	}
	if value&0x80 != 0 {
		m.shiftReg = 0x10
		m.control |= 0x0C
		return
	}
	m.shiftReg = (m.shiftReg >> 1) | ((value & 1) << 4)
	m.shiftCount++
	if m.shiftCount == 5 {
		reg := (addr >> 13) & 0x03
		if reg == 0 {
			m.control = m.shiftReg & 0x1F
		} else if reg == 1 {
			m.chrBank0 = m.shiftReg & 0x1F
		} else if reg == 2 {
			m.chrBank1 = m.shiftReg & 0x1F
		} else if reg == 3 {
			m.prgBank = m.shiftReg & 0x0F
		}
		m.shiftReg = 0x10
		m.shiftCount = 0
	}
}

func (m *MMC1) ReadCHR(addr uint16) uint8 {
	if int(addr) < len(m.chrROM) {
		return m.chrROM[addr]
	}
	return 0
}

func (m *MMC1) WriteCHR(addr uint16, value uint8) {
	// MMC1 pode usar CHR-RAM, mas aqui só leitura
}

func (m *MMC1) GetMirroring() Mirroring {
	return m.mirroring
} 