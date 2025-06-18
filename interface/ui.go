package ui

import (
	"fmt"
	"nes-emulator/interface/menu"
	"nes-emulator/rom"
	"github.com/veandco/go-sdl2/sdl"
)

type Interface struct {
	menu *menu.GameMenu
}

func New() (*Interface, error) {
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

	loader := rom.NewLoader()
	games, err := loader.LoadGameList()
	if err != nil {
		renderer.Destroy()
		window.Destroy()
		sdl.Quit()
		return nil, fmt.Errorf("erro ao carregar lista de jogos: %w", err)
	}

	gameMenu := menu.NewGameMenu(renderer, window, games)

	return &Interface{
		menu: gameMenu,
	}, nil
}

func (ui *Interface) Run() error {
	return ui.menu.Run()
}

func (ui *Interface) Cleanup() {
	if ui.menu != nil {
		ui.menu.Cleanup()
	}
	sdl.Quit()
}

func RunEmulator() error {
	ui, err := New()
	if err != nil {
		return fmt.Errorf("erro ao inicializar interface: %w", err)
	}
	defer ui.Cleanup()

	return ui.Run()
}
