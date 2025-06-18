package main

import (
	"fmt"
	"nes-emulator/interface"
)

func main() {
	fmt.Println("Iniciando NES Emulator...")
	
	// Inicia o emulador com o menu
	ui.RunEmulator()
	
	fmt.Println("Emulador finalizado.")
}