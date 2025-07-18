package memory

import "fmt"

// Mapper interface para diferentes tipos de mappers NES
type Mapper interface {
	ReadPRG(addr uint16) uint8
	WritePRG(addr uint16, value uint8)
	ReadCHR(addr uint16) uint8
	WriteCHR(addr uint16, value uint8)
	GetMirroring() Mirroring
}

// Mirroring tipos de espelhamento de VRAM
type Mirroring int

const (
	MirroringHorizontal Mirroring = iota
	MirroringVertical
	MirroringSingle0
	MirroringSingle1
	MirroringFourScreen
)

// Remover a struct UxROM, o construtor NewUxROM e os métodos ReadPRG, WritePRG, ReadCHR, WriteCHR, GetMirroring do UxROM
// Remover a struct MMC1, o construtor NewMMC1 e os métodos ReadPRG, WritePRG, ReadCHR, WriteCHR, GetMirroring do MMC1

// MapperFactory cria o mapper apropriado baseado no número do mapper
func MapperFactory(mapperNum int, prgROM, chrROM []uint8, mirroring Mirroring) (Mapper, error) {
	switch mapperNum {
	case 0:
		return NewNROM(prgROM, chrROM, mirroring), nil
	case 1:
		return NewMMC1(prgROM, chrROM, mirroring), nil
	case 2:
		return NewUxROM(prgROM, chrROM, mirroring), nil
	case 4:
		return NewMMC3(prgROM, chrROM, mirroring), nil
	default:
		return nil, fmt.Errorf("mapper %d não suportado", mapperNum)
	}
} 