package ppu

import (
	"fmt"
	"nes-emulator/interface/game"
	"nes-emulator/interface/menu"

	"github.com/veandco/go-sdl2/sdl"
)

// PPU representa a Picture Processing Unit do NES
type PPU struct {
	menu *menu.GameMenu
}

// New cria uma nova instância da PPU
func New() (*PPU, error) {
	// Inicializa SDL
	if err := sdl.Init(sdl.INIT_VIDEO); err != nil {
		return nil, fmt.Errorf("erro ao inicializar SDL: %w", err)
	}

	window, err := sdl.CreateWindow("NES Emulator", sdl.WINDOWPOS_UNDEFINED, sdl.WINDOWPOS_UNDEFINED, 800, 600, sdl.WINDOW_SHOWN)
	if err != nil {
		return nil, fmt.Errorf("erro ao criar janela: %w", err)
	}

	renderer, err := sdl.CreateRenderer(window, -1, sdl.RENDERER_ACCELERATED)
	if err != nil {
		window.Destroy()
		return nil, fmt.Errorf("erro ao criar renderer: %w", err)
	}

	// Carrega lista de jogos
	loader := game.NewLoader()
	games, _ := loader.LoadGameList()

	gameMenu := menu.NewGameMenu(renderer, window, games)
	return &PPU{
		menu: gameMenu,
	}, nil
}

// Run inicia a execução da PPU
func (p *PPU) Run() error {
	return p.menu.Run()
}

// Cleanup limpa os recursos da PPU
func (p *PPU) Cleanup() {
	if p.menu != nil {
		p.menu.Cleanup()
	}
}

// RunEmulator é a função principal para iniciar o emulador
func RunEmulator() error {
	ppu, err := New()
	if err != nil {
		return fmt.Errorf("erro ao inicializar PPU: %w", err)
	}
	defer ppu.Cleanup()

	return ppu.Run()
}