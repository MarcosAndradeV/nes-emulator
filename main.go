package main

import (
	"fmt"
	"nes-emulator/interface"
)

func main() {
	fmt.Println("Iniciando NES Emulator...")
	
	ui.RunEmulator()
	
	fmt.Println("Emulador finalizado.")

}