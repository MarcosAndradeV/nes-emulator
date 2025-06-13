package main

import (
	"fmt"
	"log"
	"nes-emulator/memory"
	"os"

	"github.com/retroenv/retrogolib/arch/cpu/m6502"
)

func LoadROM(path string) ([]byte, error) {
	data, err := os.ReadFile(path)
	if err != nil {
		return nil, err
	}

	if len(data) < 16 {
		return nil, fmt.Errorf("arquivo ROM muito pequeno")
	}

	header := data[:16]
	prgSize := int(header[4]) * 16 * 1024

	if len(data) < 16+prgSize {
		return nil, fmt.Errorf("dados PRG-ROM insuficientes")
	}

	prgROM := data[16 : 16+prgSize]
	return prgROM, nil
}

func main() {
	if len(os.Args) < 2 {
		log.Fatal("Uso: nes_emulador <rom.nes>")
	}

	rom, err := LoadROM(os.Args[1])
	if err != nil {
		log.Fatal(err)
	}

	mem := &memory.NESMemory{
		ROM: rom,
	}

	cpu := m6502.New(m6502.NewMemory(mem), m6502.WithTracing())

	for {
		if err := cpu.Step(); err != nil {
			fmt.Printf("err: %v\n", err)
			return
		}
	}
}
