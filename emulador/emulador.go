package emulador

import (
	"fmt"
)

type Emulator struct {
	romPath string
	running bool
}

func NewEmulator() *Emulator {
	return &Emulator{}
}

func (e *Emulator) LoadROM(path string) error {
	e.romPath = path
	fmt.Println("ROM carregada:", path)
	return nil
}

func (e *Emulator) Run() {
	e.running = true
	fmt.Println("Emulador iniciado")
}

func (e *Emulator) Pause() {
	e.running = false
	fmt.Println("Emulador pausado")
}

func (e *Emulator) Resume() {
	e.running = true
	fmt.Println("Emulador retomado")
}

func (e *Emulator) Step() {
	if e.running {
		fmt.Println("Executando emulador com ROM:", e.romPath)
	}
}

func (e *Emulator) Stop() {
	e.running = false
	fmt.Println("Emulador parado")
}
