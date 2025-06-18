package menu

import (
	"nes-emulator/interface/display"
	"nes-emulator/rom"
	"github.com/veandco/go-sdl2/sdl"
)

// GameMenu representa o menu principal de seleção de jogos
type GameMenu struct {
	games      []rom.GameInfo
	selectedIdx int
	state      int // ou use um tipo de estado apropriado
	renderer   *sdl.Renderer
	window     *sdl.Window
	input      *InputHandler
	render     *MenuRenderer
}

// NewGameMenu cria um novo menu de jogos
func NewGameMenu(renderer *sdl.Renderer, window *sdl.Window, games []rom.GameInfo) *GameMenu {
	menu := &GameMenu{
		games:      games,
		selectedIdx: 0,
		state:      0, // ajuste conforme seu enum de estado
		renderer:   renderer,
		window:     window,
	}
	menu.input = NewInputHandler(menu)
	menu.render = NewMenuRenderer(renderer, menu)
	return menu
}

// Run executa o loop principal do menu
func (m *GameMenu) Run() error {
	defaultFPS := display.TargetFPS
	running := true
	for running {
		running = m.input.HandleInput()
		m.render.Render()
		sdl.Delay(uint32(1000 / defaultFPS))
	}
	return nil
}

// Cleanup libera recursos do menu
func (m *GameMenu) Cleanup() {
	if m.window != nil {
		m.window.Destroy()
	}
}

// moveSelectionUp move a seleção para cima
func (m *GameMenu) moveSelectionUp() {
	if m.selectedIdx > 0 {
		m.selectedIdx--
	}
}

// moveSelectionDown move a seleção para baixo
func (m *GameMenu) moveSelectionDown() {
	if m.selectedIdx < len(m.games)-1 {
		m.selectedIdx++
	}
}

// selectCurrentGame inicia o jogo selecionado
func (m *GameMenu) selectCurrentGame() {
	if len(m.games) > 0 {
		m.state = 1 // StatePlaying
	}
}

// returnToMenu retorna ao menu principal
func (m *GameMenu) returnToMenu() {
	m.state = 0 // StateMenu
}

// pauseGame pausa o jogo
func (m *GameMenu) pauseGame() {
	m.state = 2 // StatePaused
}

// resumeGame retoma o jogo pausado
func (m *GameMenu) resumeGame() {
	m.state = 1 // StatePlaying
}

// refreshGameList pode ser implementado para recarregar a lista de jogos
func (m *GameMenu) refreshGameList() {
	// Exemplo: recarregar lista de jogos do disco
	// loader := rom.NewLoader()
	// games, _ := loader.LoadGameList()
	// m.games = games
	// m.selectedIdx = 0
}

// GetSelectedGame retorna o GameInfo do jogo selecionado
func (m *GameMenu) GetSelectedGame() *rom.GameInfo {
	if len(m.games) == 0 {
		return nil
	}
	return &m.games[m.selectedIdx]
}