package emulador

import (
	"fmt"
	"nes-emulator/bus"
	"nes-emulator/cpu"
	"nes-emulator/memory"
	"nes-emulator/ppu"

	"github.com/veandco/go-sdl2/sdl"
)

type Core struct {
	mem     *memory.NESMemory
	bus     *bus.Bus
	cpu     *cpu.CPU
	ppu     *ppu.PPU
	renderer *sdl.Renderer
	running bool
	paused  bool
}

func NewCore(romPath string, renderer *sdl.Renderer) (*Core, error) {
	mem := memory.New()
	if err := mem.LoadROM(romPath); err != nil {
		return nil, err
	}
	
	// Criar PPU
	ppu := ppu.New(renderer)
	
	// Conectar mapper à PPU
	ppu.SetMapper(mem.GetMapperImpl())
	
	cart := memory.NewCartridge(mem)
	b := bus.New(cart, ppu)
	c := cpu.New(b)

	// Log do estado inicial da CPU
	fmt.Printf("CPU inicial: PC=$%04X, A=$%02X, X=$%02X, Y=$%02X, SP=$%02X\n", 
		c.Core.PC, c.Core.A, c.Core.X, c.Core.Y, c.Core.SP)

	// Log do vetor de reset e primeiros bytes da PRG-ROM
	resetLow := b.Read(0xFFFC)
	resetHigh := b.Read(0xFFFD)
	resetVector := uint16(resetHigh)<<8 | uint16(resetLow)
	fmt.Printf("[ROM] Reset Vector: $%04X (Low=$%02X High=$%02X)\n", resetVector, resetLow, resetHigh)
	fmt.Printf("[ROM] Primeiros 16 bytes da PRG-ROM em $8000: ")
	for i := 0; i < 16; i++ {
		fmt.Printf("%02X ", b.Read(0x8000+uint16(i)))
	}
	fmt.Println()
	
	return &Core{
		mem:      mem,
		bus:      b,
		cpu:      c,
		ppu:      ppu,
		renderer: renderer,
		running:  false,
		paused:   false,
	}, nil
}

// RunGame executa o ciclo principal do NES para uma ROM
func RunGame(romPath string, renderer *sdl.Renderer) error {
	fmt.Println("Executando ROM:", romPath)
	
	// Criar core completo com ROM
	core, err := NewCore(romPath, renderer)
	if err != nil {
		return fmt.Errorf("erro ao criar core: %v", err)
	}
	defer core.Cleanup()
	
	// Loop principal do emulador
	frameCount := 0
	maxFrames := 1800 // 30 segundos a 60 FPS
	
	fmt.Println("Iniciando emulação...")
	
	for frameCount < maxFrames {
		// Executar um frame completo
		core.RunFrame()
		frameCount++
		
		// Delay para 60 FPS
		sdl.Delay(16)
		
		// Verificar se o usuário quer sair
		for event := sdl.PollEvent(); event != nil; event = sdl.PollEvent() {
			switch e := event.(type) {
			case *sdl.QuitEvent:
				fmt.Printf("Emulação interrompida: %d frames executados\n", frameCount)
				return nil
			case *sdl.KeyboardEvent:
				if e.Type == sdl.KEYDOWN && e.Keysym.Sym == sdl.K_ESCAPE {
					fmt.Printf("Emulação interrompida pelo usuário: %d frames executados\n", frameCount)
					return nil
				}
			}
		}
	}
	
	fmt.Printf("Emulação concluída: %d frames executados\n", frameCount)
	return nil
}

// RunFrame executa um frame completo (89342 ciclos de PPU)
func (c *Core) RunFrame() {
	ppuCycles := 0
	for ppuCycles < 89342 {
		before := c.cpu.GetCycles()
		if err := c.cpu.Step(); err != nil {
			fmt.Printf("CPU ERROR: %v (PC=$%04X)\n", err, c.cpu.GetPC())
			if err.Error()[:18] == "unsupported opcode" {
				continue
			}
			return
		}
		after := c.cpu.GetCycles()
		cpuCycles := int(after - before)
		if cpuCycles <= 0 {
			cpuCycles = 2 // fallback: 6502 nunca consome menos de 2 ciclos
		}
		for j := 0; j < cpuCycles*3; j++ {
			c.ppu.Step()
			if c.ppu.IsNMIReady() {
				fmt.Println("CPU: NMI disparado!")
				c.cpu.TriggerNMI()
			}
			ppuCycles++
		}
	}
	c.ppu.Render()
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
		// Executar CPU
		if err := c.cpu.Step(); err != nil {
			fmt.Printf("Erro ao executar CPU: %v\n", err)
			return
		}
		
		// Executar PPU (3 ciclos)
		for i := 0; i < 3; i++ {
			c.ppu.Step()
		}
		
		// Verificar NMI
		if c.ppu.IsNMIReady() {
			c.cpu.TriggerNMI()
		}
		
		// Renderizar se necessário
		if c.ppu.IsVBlank() {
			c.ppu.Render()
		}
	}
}

// GetCPUState retorna informações do estado da CPU
func (c *Core) GetCPUState() string {
	return fmt.Sprintf("PC: $%04X, A: $%02X, X: $%02X, Y: $%02X, SP: $%02X, Status: $%02X",
		c.cpu.GetPC(), c.cpu.GetA(), c.cpu.GetX(), c.cpu.GetY(), c.cpu.GetSP(), c.cpu.GetStatus())
}

// GetPPUState retorna informações do estado da PPU
func (c *Core) GetPPUState() string {
	return fmt.Sprintf("Frame: %d, Scanline: %d, Cycle: %d, VBlank: %v",
		c.ppu.GetFrame(), c.ppu.GetScanline(), c.ppu.GetCycle(), c.ppu.IsVBlank())
}

// Cleanup limpa recursos do Core
func (c *Core) Cleanup() {
	if c.ppu != nil {
		c.ppu.Cleanup()
	}
}
