package menu

import (
	"github.com/veandco/go-sdl2/sdl"
	"nes-emulator/interface/display"
	"nes-emulator/interface/game"
)

// MenuRenderer responsável pela renderização das telas
type MenuRenderer struct {
	textRenderer *display.TextRenderer
	menu         *GameMenu
}

// NewMenuRenderer cria um novo renderizador de menu
func NewMenuRenderer(renderer *sdl.Renderer, menu *GameMenu) *MenuRenderer {
	return &MenuRenderer{
		textRenderer: display.NewTextRenderer(renderer),
		menu:         menu,
	}
}

// Render renderiza a tela atual baseada no estado
func (mr *MenuRenderer) Render() {
	switch mr.menu.state {
	case int(game.StateMenu):
		mr.renderMenu()
	case int(game.StatePlaying):
		mr.renderGame()
	case int(game.StatePaused):
		mr.renderPaused()
	}
}

// renderPaused renderiza a tela de pausa
func (mr *MenuRenderer) renderPaused() {
	renderer := mr.menu.renderer

	// Limpa a tela com cor de fundo cinza escuro
	renderer.SetDrawColor(display.ColorGray.R, display.ColorGray.G, display.ColorGray.B, display.ColorGray.A)
	renderer.Clear()

	// Mensagem de pausa
	mr.textRenderer.DrawText("JOGO PAUSADO", display.TitleX, display.TitleY, display.ColorWhite)
	mr.textRenderer.DrawText("PRESSIONE ESC PARA VOLTAR AO MENU", display.TitleX, display.TitleY+50, display.ColorWhite)

	renderer.Present()
}

// renderGame renderiza a tela do jogo (placeholder)
func (mr *MenuRenderer) renderGame() {
	// Implemente a renderização do jogo aqui ou deixe como placeholder
	// Exemplo: limpar tela e mostrar mensagem
	renderer := mr.menu.renderer
	renderer.SetDrawColor(display.ColorBlack.R, display.ColorBlack.G, display.ColorBlack.B, display.ColorBlack.A)
	renderer.Clear()
	mr.textRenderer.DrawText("Jogo em execução...", display.TitleX, display.TitleY, display.ColorWhite)
	renderer.Present()
}

// renderMenu renderiza o menu principal
func (mr *MenuRenderer) renderMenu() {
	renderer := mr.menu.renderer

	// Limpa a tela com cor de fundo azul escuro
	renderer.SetDrawColor(display.ColorDarkBlue.R, display.ColorDarkBlue.G, display.ColorDarkBlue.B, display.ColorDarkBlue.A)
	renderer.Clear()

	// Título
	mr.textRenderer.DrawText("=== NES EMULATOR ===", display.TitleX, display.TitleY, display.ColorWhite)
	mr.textRenderer.DrawText("SELECIONE UM JOGO:", display.TitleX+30, display.SubtitleY, display.ColorWhite)

	// Lista de jogos
	mr.renderGameList()

	// Instruções na parte inferior
	mr.renderInstructions()

	renderer.Present()
}

// renderGameList renderiza a lista de jogos
func (mr *MenuRenderer) renderGameList() {
	for i, gameInfo := range mr.menu.games {
		y := display.GameListY + int32(i*display.GameSpacing)

		var color sdl.Color
		if i == mr.menu.selectedIdx {
			color = display.ColorYellow
			mr.textRenderer.DrawText(">", display.ArrowX, y, display.ColorRed)
		} else {
			color = display.ColorGray
		}

		displayName := display.TruncateText(gameInfo.Name, display.MaxDisplayName)
		mr.textRenderer.DrawText(displayName, display.GameNameX, y, color)
	}
}

// renderInstructions renderiza as instruções do menu
func (mr *MenuRenderer) renderInstructions() {
	instructions := []struct {
		text string
		y    int32
	}{
		{"USE SETAS PARA NAVEGAR", display.InstructionsY},
		{"ENTER PARA INICIAR JOGO", display.InstructionsY + 30},
		{"F5 PARA ATUALIZAR LISTA", display.InstructionsY + 60},
		{"ESC PARA SAIR", display.InstructionsY + 90},
	}

		for _, inst := range instructions {
			mr.textRenderer.DrawText(inst.text, 200, inst.y, display.ColorGray)
		}
	}