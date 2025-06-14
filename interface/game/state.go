package game

// State representa os diferentes estados do jogo
type State int

const (
	// StateMenu estado do menu principal
	StateMenu State = iota
	// StatePlaying estado jogando
	StatePlaying
	// StatePaused estado pausado
	StatePaused
)

// String retorna a representação em string do estado
func (s State) String() string {
	switch s {
	case StateMenu:
		return "Menu"
	case StatePlaying:
		return "Playing"
	case StatePaused:
		return "Paused"
	default:
		return "Unknown"
	}
}

// IsPlayingState retorna true se o estado é de jogo (playing ou paused)
func (s State) IsPlayingState() bool {
	return s == StatePlaying || s == StatePaused
}

// CanPause retorna true se o estado pode ser pausado
func (s State) CanPause() bool {
	return s == StatePlaying
}

// CanResume retorna true se o estado pode ser retomado
func (s State) CanResume() bool {
	return s == StatePaused
}