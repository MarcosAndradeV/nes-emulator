package memory

import (
	"fmt"
	"os"
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
	
	// Mapper do cartucho
	mapperImpl Mapper
}

// New cria uma nova instância de memória NES
func New() *NESMemory {
	return &NESMemory{}
}

// Reset limpa a RAM (opcional, mas útil para debug)
func (m *NESMemory) Reset() {
	for i := range m.ram {
		m.ram[i] = 0
	}
}

// LoadROM carrega um arquivo ROM .nes
func (m *NESMemory) LoadROM(filename string) error {
	data, err := os.ReadFile(filename)
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

	// Determina o tipo de espelhamento
	var mirroring Mirroring
	if m.verticalMirroring {
		mirroring = MirroringVertical
	} else {
		mirroring = MirroringHorizontal
	}

	// Cria o mapper apropriado
	mapper, err := MapperFactory(m.mapper, m.prgROM, m.chrROM, mirroring)
	if err != nil {
		return fmt.Errorf("erro ao criar mapper: %v", err)
	}
	m.mapperImpl = mapper

	fmt.Printf("ROM carregada: PRG=%dKB, CHR=%dKB, Mapper=%d, Mirroring=%v\n",
		m.prgROMSize/1024, m.chrROMSize/1024, m.mapper, mirroring)

	return nil
}

// ✅ Implementa a interface Memory da retroenv

// Read implementa a interface de leitura de memória para o m6502
func (m *NESMemory) Read(address uint16) byte {
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
		// ROM do cartucho via mapper
		return m.mapperImpl.ReadPRG(address)

	default:
		// Área não mapeada
		return 0
	}
}

// Write implementa a interface de escrita de memória para o m6502
func (m *NESMemory) Write(address uint16, value byte) {
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
		fmt.Printf("[NESMemory] Write: address=$%04X value=$%02X\n", address, value)
		// Tentativa de escrita na ROM via mapper
		m.mapperImpl.WritePRG(address, value)
	}
}

// ======== PPU e I/O (placeholders) ========

// readPPU lê registradores PPU (placeholder)
func (m *NESMemory) readPPU(address uint16) byte {
	reg := 0x2000 + (address % 8)
	_ = reg
	// TODO: Implementar leitura dos registradores PPU
	return 0
}

// writePPU escreve registradores PPU (placeholder)
func (m *NESMemory) writePPU(address uint16, value byte) {
	reg := 0x2000 + (address % 8)
	_ = reg
	// TODO: Implementar escrita dos registradores PPU
}

// readIO lê registradores de I/O (placeholder)
func (m *NESMemory) readIO(address uint16) byte {
	// TODO: Implementar leitura de I/O (controles, etc.)
	return 0
}

// writeIO escreve registradores de I/O (placeholder)
func (m *NESMemory) writeIO(address uint16, value byte) {
	// TODO: Implementar escrita de I/O
}

// ======== Getters ========

func (m *NESMemory) GetPRGROM() []byte {
	return m.prgROM
}

func (m *NESMemory) GetCHRROM() []byte {
	return m.chrROM
}

func (m *NESMemory) GetMapper() int {
	return m.mapper
}

func (m *NESMemory) GetMapperImpl() Mapper {
	return m.mapperImpl
}

// Cartridge representa um cartucho NES simples (NROM)
type Cartridge struct {
	PRG []byte // PRG-ROM
	CHR []byte // CHR-ROM
	Mapper Mapper
}

// NewCartridge cria um cartucho a partir de uma NESMemory
func NewCartridge(mem *NESMemory) *Cartridge {
	return &Cartridge{
		PRG: mem.GetPRGROM(),
		CHR: mem.GetCHRROM(),
		Mapper: mem.GetMapperImpl(),
	}
}
