package menu

import (
	"fmt"
	"nes-emulator/emulador"
	"nes-emulator/interface/display"
	"nes-emulator/interface/game"
	"nes-emulator/rom"

	"github.com/veandco/go-sdl2/sdl"
)

type GameMenu struct {
	games       []rom.GameInfo
	selectedIdx int
	state       game.State // 0=Menu, 1=Playing, 2=Paused
	renderer    *sdl.Renderer
	window      *sdl.Window
	input       *InputHandler
	render      *MenuRenderer
	core        *emulador.Core
}

func (m *GameMenu) moveSelectionUp() {
	if m.selectedIdx > 0 {
		m.selectedIdx--
	}
}

func (m *GameMenu) moveSelectionDown() {
	if m.selectedIdx < len(m.games)-1 {
		m.selectedIdx++
	}
}

func NewGameMenu(renderer *sdl.Renderer, window *sdl.Window, games []rom.GameInfo) *GameMenu {
	m := &GameMenu{
		games:       games,
		selectedIdx: 0,
		state:       game.StateMenu,
		renderer:    renderer,
		window:      window,
	}
	m.input = NewInputHandler(m)
	m.render = NewMenuRenderer(renderer, m)
	return m
}

func (m *GameMenu) selectCurrentGame() {
	if len(m.games) == 0 {
		return
	}
	game := m.GetSelectedGame()
	fmt.Printf("[MENU] Iniciando jogo: %s (%s)\n", game.Name, game.FullPath)
	core, err := emulador.NewCore(game.FullPath, m.renderer)
	if err != nil {
		fmt.Printf("Erro ao criar core: %v\n", err)
		return
	}
	m.core = core
	m.state = 1 // Playing
}

func (m *GameMenu) refreshGameList() {
	// Aqui você pode implementar recarregar a lista de jogos do disco
	// Exemplo:
	// loader := rom.NewLoader()
	// games, err := loader.LoadGameList()
	// if err == nil {
	//     m.games = games
	//     m.selectedIdx = 0
	// }
}

func (m *GameMenu) returnToMenu() {
	if m.core != nil {
		m.core.Cleanup()
		m.core = nil
	}
	m.state = 0 // volta para o menu
}

func (m *GameMenu) pauseGame() {
	if m.state == 1 { // se estiver jogando
		m.state = 2 // pausa
	}
}

func (m *GameMenu) resumeGame() {
	if m.state == 2 { // se estiver pausado
		m.state = 1 // volta a jogar
	}
}

func (m *GameMenu) Run() error {
	fps := display.TargetFPS
	running := true
	for running {
		running = m.input.HandleInput()

		switch m.state {
		case 0: // Menu
			m.render.Render()
		case 1: // Playing
			if m.core != nil {
				m.core.RunFrame()
				// A PPU já renderiza, só precisamos apresentar
				m.renderer.Present()
			}
		case 2: // Paused
			// Renderizar tela de pausa
			m.render.Render()
		}

		sdl.Delay(uint32(1000 / fps))
	}
	return nil
}

func (m *GameMenu) GetSelectedGame() *rom.GameInfo {
	if len(m.games) == 0 {
		return nil
	}
	return &m.games[m.selectedIdx]
}

func (m *GameMenu) Cleanup() {
	if m.renderer != nil {
		m.renderer.Destroy()
	}
	if m.window != nil {
		m.window.Destroy()
	}
}

