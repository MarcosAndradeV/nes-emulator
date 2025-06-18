package menu

import (
	"fmt"
	"github.com/veandco/go-sdl2/sdl"
)

// InputHandler responsável pela manipulação de entrada no menu e no emulador
type InputHandler struct {
	menu *GameMenu
}

func NewInputHandler(menu *GameMenu) *InputHandler {
	return &InputHandler{menu: menu}
}

func (ih *InputHandler) HandleInput() bool {
	for event := sdl.PollEvent(); event != nil; event = sdl.PollEvent() {
		switch e := event.(type) {
		case *sdl.QuitEvent:
			return false
		case *sdl.KeyboardEvent:
			if e.Type == sdl.KEYDOWN {
				switch ih.menu.state {
				case 0: // StateMenu
					return ih.handleMenuInput(e.Keysym.Sym)
				case 1: // StatePlaying
					return ih.handleGameInput(e.Keysym.Sym)
				case 2: // StatePaused
					return ih.handlePauseInput(e.Keysym.Sym)
				}
			}
		}
	}
	return true
}

func (ih *InputHandler) handleMenuInput(key sdl.Keycode) bool {
	switch key {
	case sdl.K_ESCAPE:
		return false // sai do programa
	case sdl.K_UP:
		ih.menu.moveSelectionUp()
	case sdl.K_DOWN:
		ih.menu.moveSelectionDown()
	case sdl.K_RETURN, sdl.K_SPACE:
		ih.menu.selectCurrentGame() // inicia o emulador carregando o ROM selecionado
	case sdl.K_F5:
		ih.menu.refreshGameList()
	}
	return true
}

func (ih *InputHandler) handleGameInput(key sdl.Keycode) bool {
	switch key {
	case sdl.K_ESCAPE:
		ih.menu.returnToMenu() // para o emulador e volta ao menu
		fmt.Println("Voltando ao menu...")
	case sdl.K_p:
		ih.menu.pauseGame()
		fmt.Println("Jogo pausado")
	// Aqui você pode mapear os botões do NES para enviar para o emulador
	case sdl.K_z: // Botão A
		// ih.menu.emulator.PressButton("A") — você implementa esse método no emulador
	case sdl.K_x: // Botão B
		// ih.menu.emulator.PressButton("B")
	case sdl.K_LEFT:
		// ih.menu.emulator.PressButton("LEFT")
	case sdl.K_RIGHT:
		// ih.menu.emulator.PressButton("RIGHT")
	case sdl.K_UP:
		// ih.menu.emulator.PressButton("UP")
	case sdl.K_DOWN:
		// ih.menu.emulator.PressButton("DOWN")
	}
	return true
}

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
