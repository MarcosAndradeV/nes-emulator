#include "src/Header_files/PPU.h"

PPU::PPU() :
    scanline(0),
    dot(0),
    frameComplete(false),
    nmiOccurred(false),
    nmiOutput(false) {
    // Initialize all memory to 0
    vram.fill(0);
    oam.fill(0);
    paletteRAM.fill(0);
    frameBuffer.fill(0);

    // Initialize registers
    registers.control = 0;
    registers.mask = 0;
    registers.status = 0;
    registers.oamAddr = 0;
    registers.scroll = 0;
    registers.addr = 0;
    registers.data = 0;
}

PPU::~PPU() {

}

void PPU::tick() {
    // PPU operates at 3 times the CPU speed
    // Each scanline is 341 dots long
    // There are 262 scanlines per frame

    if (scanline >= -1 && scanline < 240) {
        if (scanline == -1 && dot == 1) {
            registers.status &= ~0x80;  // Clear VBlank flag
            frameComplete = false;
        }

        if ((dot >= 1 && dot < 257) || (dot >= 321 && dot < 337)) {
            renderBackground();
        }

        if (dot >= 257 && dot < 321) {
            evaluateSprites();
        }
    }

    if (scanline == 241 && dot == 1) {
        registers.status |= 0x80;  // Set VBlank flag
        frameComplete = true;

        // Generate NMI if enabled
        if (registers.control & 0x80) {
            nmiOccurred = true;
            nmiOutput = true;
        }
    }

    // Update dot and scanline counters
    dot++;
    if (dot >= 341) {
        dot = 0;
        scanline++;
        if (scanline >= 262) {
            scanline = -1;
        }
    }
}

void PPU::renderScanline() {
    if (scanline >= 0 && scanline < 240) {
        renderBackground();
        renderSprites();
    }
}

void PPU::renderFrame() {
    while (!frameComplete) {
        tick();
    }
}

void PPU::writeRegister(uint16_t addr, uint8_t data) {
    switch (addr & 0x7) {
        case 0: // Control
            registers.control = data;
            break;
        case 1: // Mask
            registers.mask = data;
            break;
        case 3: // OAM Address
            registers.oamAddr = data;
            break;
        case 4: // OAM Data
            writeOAM(registers.oamAddr++, data);
            break;
        case 5: // Scroll
            registers.scroll = data;
            break;
        case 6: // Address
            registers.addr = data;
            break;
        case 7: // Data
            writeVRAM(registers.addr, data);
            registers.addr += (registers.control & 0x04) ? 32 : 1;
            break;
    }
}

uint8_t PPU::readRegister(uint16_t addr) {
    uint8_t data = 0;
    switch (addr & 0x7) {
        case 2: // Status
            data = registers.status;
            registers.status &= ~0x80;  // Clear VBlank flag
            break;
        case 4: // OAM Data
            data = readOAM(registers.oamAddr++);
            break;
        case 7: // Data
            data = readVRAM(registers.addr);
            registers.addr += (registers.control & 0x04) ? 32 : 1;
            break;
    }
    return data;
}

uint8_t PPU::readVRAM(uint16_t addr) {
    return vram[addr & 0x3FFF];
}

void PPU::writeVRAM(uint16_t addr, uint8_t data) {
    vram[addr & 0x3FFF] = data;
}

uint8_t PPU::readOAM(uint8_t addr) {
    return oam[addr];
}

void PPU::writeOAM(uint8_t addr, uint8_t data) {
    oam[addr] = data;
}

void PPU::evaluateSprites() {
    // TODO: Implement sprite evaluation
}

void PPU::renderBackground() {
    if (!(registers.mask & 0x08)) return;

    // Get the current tile coordinates
    uint16_t tileX = dot / 8;
    uint16_t tileY = scanline / 8;

    // Calculate the nametable address
    uint16_t nametableAddr = 0x2000 | (registers.control & 0x03) << 10;
    nametableAddr += tileY * 32 + tileX;

    // Get the tile index from the nametable
    uint8_t tileIndex = readVRAM(nametableAddr);

    // Get the pattern table address
    uint16_t patternTableAddr = ((registers.control & 0x10) ? 0x1000 : 0) + tileIndex * 16;

    // Get the attribute table address
    uint16_t attributeAddr = 0x23C0 | (nametableAddr & 0x0C00) | ((nametableAddr >> 4) & 0x38) | ((nametableAddr >> 2) & 0x07);
    uint8_t attribute = readVRAM(attributeAddr);

    // Calculate fine Y position
    uint8_t fineY = scanline % 8;

    // Get the pattern data for this row
    uint8_t patternLow = readVRAM(patternTableAddr + fineY);
    uint8_t patternHigh = readVRAM(patternTableAddr + fineY + 8);

    // Get the palette index
    uint8_t paletteIndex = (patternHigh & 0x01) << 1 | (patternLow & 0x01);
    uint8_t palette = (attribute >> ((tileX % 4) / 2 * 2 + (tileY % 4) / 2 * 4)) & 0x03;

    // Calculate final color and write to framebuffer
    uint32_t color = getColorFromPalette(palette, paletteIndex);
    frameBuffer[scanline * 256 + dot] = color;
}

void PPU::renderSprites() {
    // TODO: Implement sprite rendering
}

uint32_t PPU::getColorFromPalette(uint8_t palette, uint8_t pixel) {
    (void)palette;
    (void)pixel;
    // TODO: Implement color palette lookup
    return 0;
}

void PPU::cpuWrite(uint16_t addr, uint8_t data) //
{
    writeRegister(addr, data);
}

uint8_t PPU::cpuRead(uint16_t addr, bool readOnly) {
    (void)readOnly;
    return readRegister(addr);
}

void PPU::ConnectCartridge(const std::shared_ptr<Cartridge>& cartridge) //conecta o cartucho
{
    this->cart = cartridge;
}

void PPU::reset()
{
    scanline = 0;
    dot = 0;
    frameComplete = false;
    nmiOccurred = false;
    nmiOutput = false;

    // Reset registers
    registers.control = 0;
    registers.mask = 0;
    registers.status = 0;
    registers.oamAddr = 0;
    registers.scroll = 0;
    registers.addr = 0;
    registers.data = 0;
}

void PPU::clock() {
    tick();
}
