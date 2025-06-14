package memory

import (
	"fmt"
	"io/ioutil"
)

// NESMemory representa o sistema de memória do NES
type NESMemory struct {
	// RAM interna do NES (2KB, espelhada para 8KB)
	ram [0x2000]byte
	
	// ROM do cartucho
	prgROM []byte
	chrROM []byte
	
	// Informações do header
	prgROMSize int
	chrROMSize int
	mapper     int
	
	// Flags do cartucho
	verticalMirroring bool
	hasBatteryRAM     bool
}

// New cria uma nova instância de memória NES
func New() *NESMemory {
	return &NESMemory{}
}

// LoadROM carrega um arquivo ROM .nes
func (m *NESMemory) LoadROM(filename string) error {
	data, err := ioutil.ReadFile(filename)
	if err != nil {
		return fmt.Errorf("erro ao ler arquivo ROM: %v", err)
	}
	
	// Verifica se é um arquivo NES válido
	if len(data) < 16 || string(data[0:4]) != "NES\x1a" {
		return fmt.Errorf("arquivo não é uma ROM NES válida")
	}
	
	// Lê o header iNES
	header := data[0:16]
	m.prgROMSize = int(header[4]) * 16384 // 16KB por unidade
	m.chrROMSize = int(header[5]) * 8192  // 8KB por unidade
	
	flags6 := header[6]
	m.verticalMirroring = (flags6 & 0x01) != 0
	m.hasBatteryRAM = (flags6 & 0x02) != 0
	m.mapper = int(header[6]>>4) | int(header[7]&0xF0)
	
	// Carrega PRG ROM
	prgStart := 16
	if m.prgROMSize > 0 {
		m.prgROM = make([]byte, m.prgROMSize)
		copy(m.prgROM, data[prgStart:prgStart+m.prgROMSize])
	}
	
	// Carrega CHR ROM
	chrStart := prgStart + m.prgROMSize
	if m.chrROMSize > 0 {
		m.chrROM = make([]byte, m.chrROMSize)
		copy(m.chrROM, data[chrStart:chrStart+m.chrROMSize])
	}
	
	fmt.Printf("ROM carregada: PRG=%dKB, CHR=%dKB, Mapper=%d\n", 
		m.prgROMSize/1024, m.chrROMSize/1024, m.mapper)
	
	return nil
}

// Read implementa a interface de leitura de memória para o m6502
func (m *NESMemory) Read(address uint16) uint8 {
	switch {
	case address < 0x2000:
		// RAM interna (2KB espelhada)
		return m.ram[address%0x800]
	
	case address >= 0x2000 && address < 0x4000:
		// Registradores PPU (espelhados a cada 8 bytes)
		return m.readPPU(address)
	
	case address >= 0x4000 && address < 0x4020:
		// Registradores APU e I/O
		return m.readIO(address)
	
	case address >= 0x8000:
		// ROM do cartucho
		return m.readROM(address)
	
	default:
		// Área não mapeada
		return 0
	}
}

// Write implementa a interface de escrita de memória para o m6502
func (m *NESMemory) Write(address uint16, value uint8) {
	switch {
	case address < 0x2000:
		// RAM interna (2KB espelhada)
		m.ram[address%0x800] = value
	
	case address >= 0x2000 && address < 0x4000:
		// Registradores PPU
		m.writePPU(address, value)
	
	case address >= 0x4000 && address < 0x4020:
		// Registradores APU e I/O
		m.writeIO(address, value)
	
	case address >= 0x8000:
		// Tentativa de escrita na ROM (alguns mappers permitem)
		m.writeROM(address, value)
	}
}

// readPPU lê registradores PPU (placeholder)
func (m *NESMemory) readPPU(address uint16) uint8 {
	// TODO: Implementar leitura dos registradores PPU
	return 0
}

// writePPU escreve registradores PPU (placeholder)
func (m *NESMemory) writePPU(address uint16, value uint8) {
	// TODO: Implementar escrita dos registradores PPU
}

// readIO lê registradores de I/O (placeholder)
func (m *NESMemory) readIO(address uint16) uint8 {
	// TODO: Implementar leitura de I/O (controles, etc.)
	return 0
}

// writeIO escreve registradores de I/O (placeholder)
func (m *NESMemory) writeIO(address uint16, value uint8) {
	// TODO: Implementar escrita de I/O
}

// readROM lê da ROM do cartucho
func (m *NESMemory) readROM(address uint16) uint8 {
	if len(m.prgROM) == 0 {
		return 0
	}
	
	// Mapeia endereços da ROM
	if len(m.prgROM) == 16384 {
		// ROM de 16KB: espelha em $8000-$BFFF e $C000-$FFFF
		return m.prgROM[(address-0x8000)%0x4000]
	} else {
		// ROM de 32KB: mapeia diretamente
		return m.prgROM[address-0x8000]
	}
}

// writeROM escreve na ROM (para mappers que suportam)
func (m *NESMemory) writeROM(address uint16, value uint8) {
	// TODO: Implementar escrita para mappers específicos
}

// GetPRGROM retorna a ROM do programa
func (m *NESMemory) GetPRGROM() []byte {
	return m.prgROM
}

// GetCHRROM retorna a ROM dos caracteres
func (m *NESMemory) GetCHRROM() []byte {
	return m.chrROM
}

// GetMapper retorna o número do mapper
func (m *NESMemory) GetMapper() int {
	return m.mapper
}