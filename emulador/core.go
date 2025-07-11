package emulador

import (
	"fmt"
	"nes-emulator/bus"
	"nes-emulator/cpu"
	"nes-emulator/memory"
)

type Core struct {
	mem     *memory.NESMemory
	bus     *bus.Bus
	cpu     *cpu.CPU
	running bool
	paused  bool
}

func NewCore(romPath string) (*Core, error) {
	mem := memory.New()
	if err := mem.LoadROM(romPath); err != nil {
		return nil, err
	}
	b := bus.NewBus(mem)
	c := cpu.New(b)
	return &Core{
		mem:     mem,
		bus:     b,
		cpu:     c,
		running: false,
		paused:  false,
	}, nil
}

// RunGame executa o ciclo principal do NES para uma ROM
func RunGame(romPath string) error {
	fmt.Println("Executando ROM:", romPath)
	mem := memory.New()
	if err := mem.LoadROM(romPath); err != nil {
		return err
	}
	b := bus.NewBus(mem)
	c := cpu.New(b)

	// Loop principal do NES (simplificado)
	running := true
	for running {
		c.Step()
		// TODO: Atualizar PPU, input, sincronização, etc.
		break // só para exemplo, remova depois
	}
	return nil
}

func (c *Core) PauseGame() {
	c.paused = true
}

func (c *Core) ResumeGame() {
	c.paused = false
}

func (c *Core) StopGame() {
	c.running = false
}

func (c *Core) StepGame() {
	if c.running && !c.paused {
		c.cpu.Step()
	}
}
