package memory

import "fmt"

type NESMemory struct {
	RAM [1<<16]byte
	ROM []byte
}

func (m *NESMemory) Read(addr uint16) byte {
	switch {
	case addr < 0x2000:
		return m.RAM[addr%0x0800]
	case addr >= 0x8000:
		offset := int(addr - 0x8000)
		if offset < len(m.ROM) {
			return m.ROM[offset]
		}
		return 0
	case addr == 0xFFFC || addr == 0xFFFD:
		return m.ROM[len(m.ROM)-2+int(addr-0xFFFC)]
	default:
		return 0
	}
}

func (m *NESMemory) Write(addr uint16, val byte) {

    if addr >= 0x6000 && addr <= 0x600F {
        fmt.Printf("%c", val)
    }

    if addr < 0x2000 {
        m.RAM[addr%0x0800] = val
    }
}
