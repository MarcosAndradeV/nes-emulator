package memory

type MMC3 struct {
    prgROM    []byte
    chrROM    []byte
    mirror    Mirroring // Corrigido para tipo Mirroring
    prgBanks  int
    chrBanks  int
    bankRegs  [8]uint8
    bankSel   uint8
    prgMode   bool
    chrMode   bool

    // IRQ (básico)
    irqReload   uint8
    irqCounter  uint8
    irqEnable   bool
}

func NewMMC3(prgROM, chrROM []byte, mirror Mirroring) *MMC3 { // Corrigido tipo do argumento
    prgBanks := len(prgROM) / 0x2000
    chrBanks := len(chrROM) / 0x0400

    return &MMC3{
        prgROM:   prgROM,
        chrROM:   chrROM,
        mirror:   mirror, // Corrigido
        prgBanks: prgBanks,
        chrBanks: chrBanks,
        // inicializando IRQ
        irqReload:  0,
        irqCounter: 0,
        irqEnable:  false,
    }
}

func (m *MMC3) ReadPRG(addr uint16) byte {
    switch {
    case addr >= 0x8000 && addr <= 0x9FFF:
        bank := 0
        if m.prgMode {
            bank = int(m.prgBanks - 2) // banco fixo no início
        } else {
            bank = int(m.bankRegs[6] % uint8(m.prgBanks))
        }
        offset := int(addr-0x8000) + bank*0x2000
        return m.prgROM[offset%len(m.prgROM)]

    case addr >= 0xA000 && addr <= 0xBFFF:
        bank := int(m.bankRegs[7] % uint8(m.prgBanks))
        offset := int(addr-0xA000) + bank*0x2000
        return m.prgROM[offset%len(m.prgROM)]

    case addr >= 0xC000 && addr <= 0xDFFF:
        bank := 0
        if m.prgMode {
            bank = int(m.bankRegs[6] % uint8(m.prgBanks))
        } else {
            bank = int(m.prgBanks - 2)
        }
        offset := int(addr-0xC000) + bank*0x2000
        return m.prgROM[offset%len(m.prgROM)]

    case addr >= 0xE000 && addr <= 0xFFFF:
        bank := int(m.prgBanks - 1) // último banco fixo
        offset := int(addr-0xE000) + bank*0x2000
        return m.prgROM[offset%len(m.prgROM)]
    }
    return 0
}

func (m *MMC3) ReadCHR(addr uint16) byte {
    bank := 0
    if addr < 0x2000 {
        // 8 bancos de 1KB controlados pelos regs
        index := 0
        switch {
        case addr < 0x400:
            index = 0
        case addr < 0x800:
            index = 1
        case addr < 0xC00:
            index = 2
        case addr < 0x1000:
            index = 3
        case addr < 0x1400:
            index = 4
        case addr < 0x1800:
            index = 5
        case addr < 0x1C00:
            index = 6
        default:
            index = 7
        }
        bank = int(m.bankRegs[index] % uint8(m.chrBanks))
        offset := int(addr%0x0400) + bank*0x0400
        return m.chrROM[offset%len(m.chrROM)]
    }
    return 0
}

func (m *MMC3) Write(addr uint16, value byte) {
    switch {
    case addr >= 0x8000 && addr <= 0x9FFF:
        if addr%2 == 0 {
            m.bankSel = value
            m.prgMode = (value>>6)&1 == 1
            m.chrMode = (value>>7)&1 == 1
        } else {
            reg := m.bankSel & 0x07
            m.bankRegs[reg] = value
        }

    case addr >= 0xC000 && addr <= 0xDFFF:
        if addr%2 == 0 {
            m.irqReload = value
        } else {
            m.irqCounter = 0
        }

    case addr >= 0xE000 && addr <= 0xFFFF:
        if addr%2 == 0 {
            m.irqEnable = false
        } else {
            m.irqEnable = true
        }
    }
}

func (m *MMC3) WriteCHR(addr uint16, value uint8) {
	// MMC3 normalmente não suporta escrita em CHR-ROM (apenas CHR-RAM)
	// Stub para interface
}

func (m *MMC3) WritePRG(addr uint16, value uint8) {
	m.Write(addr, value)
}

// --- IRQ básico ---
// Deve ser chamado a cada scanline pela PPU.
func (m *MMC3) ClockScanline(triggerIRQ func()) {
    if m.irqCounter == 0 {
        m.irqCounter = m.irqReload
    } else {
        m.irqCounter--
        if m.irqCounter == 0 && m.irqEnable {
            triggerIRQ()
        }
    }
}

func (m *MMC3) GetMirroring() Mirroring {
	return m.mirror
}
