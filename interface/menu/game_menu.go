package menu

import (
	"nes-emulator/interface/display"
	"nes-emulator/interface/game"

	"github.com/veandco/go-sdl2/sdl"
)

// GameMenu representa o menu principal de seleção de jogos
type GameMenu struct {
	games      []game.GameInfo
	selectedIdx int
	state      game.State
	renderer   *sdl.Renderer
	window     *sdl.Window
	input      *InputHandler
	render     *MenuRenderer
}

// NewGameMenu cria um novo menu de jogos
func NewGameMenu(renderer *sdl.Renderer, window *sdl.Window, games []game.GameInfo) *GameMenu {
	menu := &GameMenu{
		games:      games,
		selectedIdx: 0,
		state:      game.StateMenu,
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
		m.state = game.StatePlaying
	}
}

// returnToMenu retorna ao menu principal
func (m *GameMenu) returnToMenu() {
	m.state = game.StateMenu
}

// pauseGame pausa o jogo
func (m *GameMenu) pauseGame() {
	m.state = game.StatePaused
}

// resumeGame retoma o jogo pausado
func (m *GameMenu) resumeGame() {
	m.state = game.StatePlaying
}

// refreshGameList pode ser implementado para recarregar a lista de jogos
func (m *GameMenu) refreshGameList() {
	// Exemplo: recarregar lista de jogos do disco
	// loader := game.NewLoader()
	// games, _ := loader.LoadGameList()
	// m.games = games
	// m.selectedIdx = 0
}

// GetSelectedGame retorna o GameInfo do jogo selecionado
func (m *GameMenu) GetSelectedGame() *game.GameInfo {
	if len(m.games) == 0 {
		return nil
	}
	return &m.games[m.selectedIdx]
}