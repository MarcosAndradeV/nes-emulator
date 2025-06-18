package display

import (
	"strings"
	"github.com/veandco/go-sdl2/sdl"
)

// TextRenderer responsável pela renderização de texto
type TextRenderer struct {
	renderer *sdl.Renderer
}

// NewTextRenderer cria um novo renderizador de texto
func NewTextRenderer(renderer *sdl.Renderer) *TextRenderer {
	return &TextRenderer{
		renderer: renderer,
	}
}

// DrawChar desenha um caractere na posição especificada
func (tr *TextRenderer) DrawChar(char rune, x, y int32, color sdl.Color, scale int32) {
	charData, exists := Font8x8[char]
	if !exists {
		charData = Font8x8['?']
	}

	tr.renderer.SetDrawColor(color.R, color.G, color.B, color.A)

	for row := range 8 {
		for col := range 8 {
			if charData[row]&(1<<(7-col)) != 0 {
				for sy := range scale {
					for sx := range scale {
						pixelX := x + int32(col)*scale + sx
						pixelY := y + int32(row)*scale + sy
						tr.renderer.DrawPoint(pixelX, pixelY)
					}
				}
			}
		}
	}
}

// DrawText desenha uma string na posição especificada
func (tr *TextRenderer) DrawText(text string, x, y int32, color sdl.Color) {
	currentX := x
	for _, char := range strings.ToUpper(text) {
		tr.DrawChar(char, currentX, y, color, FontScale)
		currentX += CharWidth + CharSpacing
	}
}

// DrawTextCentered desenha texto centralizado
func (tr *TextRenderer) DrawTextCentered(text string, y int32, color sdl.Color, windowWidth int32) {
	textWidth := int32(len(text)) * (CharWidth + CharSpacing)
	x := (windowWidth - textWidth) / 2
	tr.DrawText(text, x, y, color)
}

// TruncateText trunca o texto se for muito longo
func TruncateText(text string, maxLen int) string {
	if len(text) > maxLen {
		return text[:maxLen-3] + "..."
	}
	return text
}
