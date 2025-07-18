package ppu

import (
    "fmt"
    "nes-emulator/memory"

    "github.com/veandco/go-sdl2/sdl"
)

type PPU struct {
    ctrl     uint8
    mask     uint8
    status   uint8
    oamAddr  uint8
    oamData  uint8
    scroll   uint8
    addr     uint8
    data     uint8

    vram    []uint8
    oam     []uint8
    palette []uint8

    scanline    int
    cycle       int
    frame       int
    vblank      bool
    nmiOccurred bool

    addressLatch bool
    dataBuffer   uint8
    tempAddr     uint16
    vramAddr     uint16
    fineXScroll  uint8

    renderer *sdl.Renderer
    texture  *sdl.Texture
    pixels   []uint8

    nesPalette [64]sdl.Color

    mapper memory.Mapper

    CPUTriggerIRQ func()
}

func New(renderer *sdl.Renderer) *PPU {
    ppu := &PPU{
        vram:     make([]uint8, 0x800),
        oam:      make([]uint8, 0x100),
        palette:  make([]uint8, 0x20),
        pixels:   make([]uint8, 256*240*4),
        renderer: renderer,
    }

    texture, err := renderer.CreateTexture(
        sdl.PIXELFORMAT_RGBA8888,
        sdl.TEXTUREACCESS_STREAMING,
        256, 240,
    )
    if err != nil {
        fmt.Printf("Erro ao criar textura PPU: %v\n", err)
        return ppu
    }
    ppu.texture = texture

    ppu.initPalette()
    return ppu
}

func (ppu *PPU) SetMapper(mapper memory.Mapper) {
    ppu.mapper = mapper
}

func (ppu *PPU) Step() {
    if ppu.scanline == 241 && ppu.cycle == 1 {
        ppu.status |= 0x80
        ppu.vblank = true
        ppu.nmiOccurred = true
    }
    if ppu.scanline == 261 && ppu.cycle == 1 {
        ppu.status &= ^uint8(0x80)
        ppu.vblank = false
        ppu.nmiOccurred = false
    }

    if ppu.scanline < 240 && ppu.cycle < 256 {
        ppu.renderPixel()
    }

    if ppu.cycle == 260 {
        if mmc3, ok := ppu.mapper.(*memory.MMC3); ok {
            mmc3.ClockScanline(func() {
                if ppu.CPUTriggerIRQ != nil {
                    ppu.CPUTriggerIRQ()
                }
            })
        }
    }

    ppu.cycle++
    if ppu.cycle > 340 {
        ppu.cycle = 0
        ppu.scanline++
        if ppu.scanline > 261 {
            ppu.scanline = 0
            ppu.frame++
        }
    }
}

func (ppu *PPU) IsNMIReady() bool {
    if ppu.nmiOccurred && (ppu.ctrl&0x80) != 0 {
        ppu.nmiOccurred = false
        return true
    }
    return false
}

func (ppu *PPU) Cleanup() {
    if ppu.texture != nil {
        ppu.texture.Destroy()
    }
}
