package ppu

import (
    "fmt"

    "os"
    "nes-emulator/memory"
    "github.com/retroenv/retrogolib/arch/cpu/m6502"
    "github.com/veandco/go-sdl2/sdl"
)

func VisualTestCPU(rom []byte) {
    mem := &memory.NESMemory{ROM: rom}
    cpu := m6502.New(m6502.NewMemory(mem), m6502.WithTracing())

    if err := sdl.Init(sdl.INIT_VIDEO); err != nil {
        fmt.Printf("Erro SDL Init: %s\n", err)
        return
    }
    defer sdl.Quit()

    window, err := sdl.CreateWindow("Teste CPU NES", sdl.WINDOWPOS_UNDEFINED, sdl.WINDOWPOS_UNDEFINED,
        500, 320, sdl.WINDOW_SHOWN)
    if err != nil {
        fmt.Printf("Erro ao criar janela: %s\n", err)
        return
    }
    defer window.Destroy()

    surface, err := window.GetSurface()
    if err != nil {
        fmt.Printf("Erro ao obter surface: %s\n", err)
        return
    }

    running := true
    var cpuErr error
    frame := 0

    for running {
        for event := sdl.PollEvent(); event != nil; event = sdl.PollEvent() {
            if _, ok := event.(*sdl.QuitEvent); ok {
                running = false
            }
        }

        if cpuErr == nil {
            if err := cpu.Step(); err != nil {
                cpuErr = err
                fmt.Printf("Erro na CPU: %v\n", err)
            }
        }

        st := cpu.State()

        surface.FillRect(nil, 0x1A1A1A)

        drawBar(surface, 40, 40, st.A, 0x00FF00, "A")
        drawBar(surface, 40, 80, st.X, 0x0000FF, "X")
        drawBar(surface, 40, 120, st.Y, 0xFF0000, "Y")
        drawBar(surface, 40, 160, uint8(st.PC&0xFF), 0xFFFF00, "PC")
        drawBar(surface, 40, 200, st.SP, 0xFF00FF, "SP")
        drawBar(surface, 40, 240, st.Flags.C, 0x00FFFF, "Flags:C")

        if cpuErr != nil {
            surface.FillRect(&sdl.Rect{X: 0, Y: 0, W: 500, H: 30}, 0xFF2222)
            fmt.Fprintf(os.Stderr, "*** CPU PAROU: %v\n", cpuErr)
        }

        window.UpdateSurface()

        if frame%60 == 0 || cpuErr != nil {
            printState(st, cpu.GetFlags())
        }

        frame++
        sdl.Delay(16)
    }
}

func drawBar(surface *sdl.Surface, x, y int32, value uint8, color uint32, label string) {
    width := int32(value) * 2
    if width > 400 {
        width = 400
    }
    surface.FillRect(&sdl.Rect{X: x, Y: y, W: width, H: 20}, color)
    // Aqui você pode desenhar texto com SDL_ttf se quiser rotular os valores visualmente
}

func printState(st m6502.State, flags byte) {
    fmt.Println("---- CPU State ----")
    fmt.Printf("PC:    $%04X | A: $%02X | X: $%02X | Y: $%02X\n", st.PC, st.A, st.X, st.Y)
    fmt.Printf("SP:    $%02X | Flags: %08b (N V U B D I Z C)\n", st.SP, flags)
    fmt.Printf("Cycles:%d\n", st.Cycles)
    fmt.Println("-------------------")
}
