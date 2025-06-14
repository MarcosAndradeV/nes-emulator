package display

import "github.com/veandco/go-sdl2/sdl"

// Constantes de tamanho de janela
const (
	WindowWidth  = 800
	WindowHeight = 600
	GameWidth    = 256
	GameHeight   = 240
)

// Constantes de texto
const (
	FontScale      = 2
	CharWidth      = 8 * FontScale
	CharSpacing    = 2
	MaxDisplayName = 35
)

// Constantes de timing
const (
	TargetFPS     = 60
	FrameDuration = 16 // milliseconds (1000/60)
)

// Cores predefinidas
var (
	ColorWhite      = sdl.Color{R: 255, G: 255, B: 255, A: 255}
	ColorYellow     = sdl.Color{R: 255, G: 255, B: 0, A: 255}
	ColorRed        = sdl.Color{R: 255, G: 0, B: 0, A: 255}
	ColorGray       = sdl.Color{R: 200, G: 200, B: 200, A: 255}
	ColorDarkGray   = sdl.Color{R: 150, G: 150, B: 150, A: 255}
	ColorLightGray  = sdl.Color{R: 100, G: 100, B: 100, A: 255}
	ColorBlack      = sdl.Color{R: 0, G: 0, B: 0, A: 255}
	ColorDarkBlue   = sdl.Color{R: 25, G: 25, B: 112, A: 255}
	ColorDarkRed    = sdl.Color{R: 51, G: 0, B: 0, A: 255}
	ColorGreen      = sdl.Color{R: 0, G: 255, B: 0, A: 255}
)

// Posições do menu
const (
	TitleX        = 200
	TitleY        = 50
	SubtitleY     = 100
	GameListY     = 150
	GameSpacing   = 35
	ArrowX        = 180
	GameNameX     = 210
	InstructionsY = 480
)