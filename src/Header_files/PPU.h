#pragma once

#include <cstdint>
#include <array>

class PPU {
public:
    PPU();
    ~PPU();

    // PPU Register Access
    void writeRegister(uint16_t addr, uint8_t data);
    uint8_t readRegister(uint16_t addr);

    // Main PPU Functions
    void tick();
    void renderScanline();
    void renderFrame();

    // Memory Access
    uint8_t readVRAM(uint16_t addr);
    void writeVRAM(uint16_t addr, uint8_t data);
    uint8_t readOAM(uint8_t addr);
    void writeOAM(uint8_t addr, uint8_t data);

    // Frame Buffer Access
    const uint32_t* getFrameBuffer() const { return frameBuffer.data(); }

private:
    // PPU Memory
    std::array<uint8_t, 16384> vram;      // 16KB VRAM
    std::array<uint8_t, 256> oam;         // Object Attribute Memory
    std::array<uint8_t, 32> paletteRAM;   // Palette RAM
    std::array<uint32_t, 256 * 240> frameBuffer;  // Frame buffer (256x240 pixels)

    // PPU Registers
    struct {
        uint8_t control;    // $2000
        uint8_t mask;       // $2001
        uint8_t status;     // $2002
        uint8_t oamAddr;    // $2003
        uint8_t scroll;     // $2005
        uint8_t addr;       // $2006
        uint8_t data;       // $2007
    } registers;

    // Internal PPU State
    int scanline;
    int dot;
    bool frameComplete;
    bool nmiOccurred;
    bool nmiOutput;

    // Helper Functions
    void evaluateSprites();
    void renderBackground();
    void renderSprites();
    uint32_t getColorFromPalette(uint8_t palette, uint8_t pixel);
};