package menu

import (
	"fmt"
	"github.com/veandco/go-sdl2/sdl"
	"nes-emulator/interface/game"
)

// InputHandler responsável pela manipulação de entrada
type InputHandler struct {
	menu *GameMenu
}

// NewInputHandler cria um novo manipulador de entrada
func NewInputHandler(menu *GameMenu) *InputHandler {
	return &InputHandler{
		menu: menu,
	}
}

// HandleInput processa os eventos de entrada
func (ih *InputHandler) HandleInput() bool {
	for event := sdl.PollEvent(); event != nil; event = sdl.PollEvent() {
		switch e := event.(type) {
		case *sdl.QuitEvent:
			return false
		case *sdl.KeyboardEvent:
			if e.Type == sdl.KEYDOWN {
				switch ih.menu.state {
				case game.StateMenu:
					return ih.handleMenuInput(e.Keysym.Sym)
				case game.StatePlaying:
					return ih.handleGameInput(e.Keysym.Sym)
				case game.StatePaused:
					return ih.handlePauseInput(e.Keysym.Sym)
				}
			}
		}
	}
	return true
}

// handleMenuInput processa entrada no menu
func (ih *InputHandler) handleMenuInput(key sdl.Keycode) bool {
	switch key {
	case sdl.K_ESCAPE:
		return false
	case sdl.K_UP:
		ih.menu.moveSelectionUp()
	case sdl.K_DOWN:
		ih.menu.moveSelectionDown()
	case sdl.K_RETURN, sdl.K_SPACE:
		ih.menu.selectCurrentGame()
	case sdl.K_F5:
		ih.menu.refreshGameList()
	}
	return true
}

// handleGameInput processa entrada durante o jogo
func (ih *InputHandler) handleGameInput(key sdl.Keycode) bool {
	switch key {
	case sdl.K_ESCAPE:
		ih.menu.returnToMenu()
		fmt.Println("Voltando ao menu...")
	case sdl.K_p:
		ih.menu.pauseGame()
		fmt.Println("Jogo pausado")
	// Controles do NES (para implementação futura)
	case sdl.K_z: // Botão A
		// TODO: Implementar botão A
	case sdl.K_x: // Botão B
		// TODO: Implementar botão B
	case sdl.K_LEFT, sdl.K_RIGHT, sdl.K_UP, sdl.K_DOWN: // D-pad
		// TODO: Implementar D-pad
	}
	return true
}

// handlePauseInput processa entrada quando pausado
func (ih *InputHandler) handlePauseInput(key sdl.Keycode) bool {
	switch key {
	case sdl.K_ESCAPE:
		ih.menu.returnToMenu()
		fmt.Println("Voltando ao menu...")
	case sdl.K_p:
		ih.menu.resumeGame()
		fmt.Println("Jogo retomado")
	}
	return true
}