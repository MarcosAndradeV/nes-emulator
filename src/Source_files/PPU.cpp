#include "src/Header_files/PPU.h"
#include <vector>

PPU::PPU()
    : scanline(0), dot(0), frameComplete(false), nmiOccurred(false),
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

PPU::~PPU() {}

void PPU::tick() {
  // PPU operates at 3 times the CPU speed
  // Each scanline is 341 dots long
  // There are 262 scanlines per frame

  if (scanline >= -1 && scanline < 240) {
    if (scanline == -1 && dot == 1) {
      registers.status &= ~0x80; // Clear VBlank flag
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
    registers.status |= 0x80; // Set VBlank flag
    frameComplete = true;

    // Generate NMI if enabled
    if (registers.control & 0x80) {
      nmiOccurred = true;
      nmiOutput = true;
    }
  }

  // Update dot and scanline counters
  dot++;
  // scanline++;
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
    registers.status &= ~0x80; // Clear VBlank flag
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

uint8_t PPU::readVRAM(uint16_t addr) { return vram[addr & 0x3FFF]; }

void PPU::writeVRAM(uint16_t addr, uint8_t data) { vram[addr & 0x3FFF] = data; }

uint8_t PPU::readOAM(uint8_t addr) { return oam[addr]; }

void PPU::writeOAM(uint8_t addr, uint8_t data) { oam[addr] = data; }

void PPU::evaluateSprites() {
  int spritesFound = 0;
  std::fill(std::begin(sOAM), std::end(sOAM), 0xFF); // Clear secondary OAM

  for (int i = 0; i < 64; i++) {
    uint8_t spriteY = oam[i * 4];             // Sprite Y position
    int spriteHeight = (spriteSize) ? 16 : 8; // 8x8 or 8x16 sprite mode

    if (scanline >= spriteY && scanline < spriteY + spriteHeight) {
      if (spritesFound < 8) {
        // Copy 4 bytes (Y, tile index, attributes, X) to secondary OAM
        for (int j = 0; j < 4; j++) {
          sOAM[spritesFound * 4 + j] = oam[i * 4 + j];
        }
        spritesFound++;
      } else {
        // Set sprite overflow flag (bit 5 of PPU status)
        registers.status |= 0x20;
        break;
      }
    }
  }
}

void PPU::renderBackground() {
  if (!(registers.mask & 0x08))
    return;

  uint16_t coarseX = dot / 8;
  uint16_t coarseY = scanline / 8;

  // Calculate nametable base
  uint16_t nametableAddr = 0x2000 | ((registers.control & 0x03) << 10);
  nametableAddr += coarseY * 32 + coarseX;

  // Get tile index
  uint8_t tileIndex = readVRAM(nametableAddr);

  // Pattern table base
  uint16_t patternTableAddr = (registers.control & 0x10) ? 0x1000 : 0;
  patternTableAddr += tileIndex * 16;

  // Fine Y
  uint8_t fineY = scanline % 8;
  uint8_t patternLow = readVRAM(patternTableAddr + fineY);
  uint8_t patternHigh = readVRAM(patternTableAddr + fineY + 8);

  // Attribute table logic
  uint16_t attributeAddr =
      0x23C0 | (nametableAddr & 0x0C00) | ((coarseY / 4) << 3) | (coarseX / 4);
  uint8_t attribute = readVRAM(attributeAddr);

  uint8_t shift = ((coarseY % 4) / 2) * 4 + ((coarseX % 4) / 2) * 2;
  uint8_t palette = (attribute >> shift) & 0x03;

  // Pixel within tile (bitplane decoding)
  uint8_t bit = 7 - (dot % 8);
  uint8_t paletteIndex =
      ((patternHigh >> bit) & 0x01) << 1 | ((patternLow >> bit) & 0x01);

  // Skip transparent background pixels
  if (paletteIndex == 0)
    return;

  uint8_t finalPaletteIndex = (palette << 2) | paletteIndex;
  uint8_t color = readVRAM(0x3F00 + finalPaletteIndex);

  uint32_t pixelColor = getColorFromPalette(0, color);
  frameBuffer[scanline * 256 + dot] = pixelColor;
}

void PPU::renderSprites() {
  for (int i = 0; i < 64; i++) {
    uint8_t spriteY = oam[i * 4];
    uint8_t spriteTileIndex = oam[i * 4 + 1];
    uint8_t spriteAttributes = oam[i * 4 + 2];
    uint8_t spriteX = oam[i * 4 + 3];

    if (scanline >= spriteY && scanline < spriteY + 8) {
      uint8_t rowOffset = scanline - spriteY;
      if (spriteAttributes & 0x80) {
        rowOffset = 7 - rowOffset; // Vertical flip
      }

      uint16_t patternAddr = (registers.control & 0x08) ? 0x1000 : 0x0000;
      patternAddr += spriteTileIndex * 16 + rowOffset;

      uint8_t patternLow = readVRAM(patternAddr);
      uint8_t patternHigh = readVRAM(patternAddr + 8);

      for (int tilePixel = 0; tilePixel < 8; tilePixel++) {
        int pixelIndex = (spriteAttributes & 0x40)
                             ? tilePixel
                             : (7 - tilePixel); // Horizontal flip
        uint8_t paletteIndex = (((patternHigh >> pixelIndex) & 0x01) << 1) |
                               ((patternLow >> pixelIndex) & 0x01);
        if (paletteIndex == 0)
          continue; // Transparent

        int pixelX = spriteX + tilePixel;
        if (pixelX >= 0 && pixelX < 256) {
          uint8_t paletteNumber = spriteAttributes & 0x03;
          uint16_t paletteAddr =
              0x3F10 + (paletteNumber << 2) + (paletteIndex & 0x03);
          uint8_t color = readVRAM(paletteAddr);

          uint32_t pixelColor = getColorFromPalette(0, color);
          frameBuffer[scanline * 256 + pixelX] = pixelColor;
        }
      }
    }
  }
}

uint32_t PPU::getColorFromPalette(uint8_t palette, uint8_t pixel) {
  (void)
      palette; // Currently unused, but could be used for different palette sets

  static const std::vector<uint32_t> colorPalette = {
      0x545454, 0x001E74, 0x081090, 0x300088, 0x440064, 0x5C0030, 0x540400,
      0x3C1800, 0x202A00, 0x083A00, 0x004000, 0x003C00, 0x00323C, 0x000000,
      0x000000, 0x000000, 0x989698, 0x084CC4, 0x3032EC, 0x5C1EE4, 0x8814B0,
      0xA01464, 0x982220, 0x783C00, 0x545A00, 0x287200, 0x087C00, 0x007628,
      0x006678, 0x000000, 0x000000, 0x000000, 0xECECEC, 0x4C9AEC, 0x787CEC,
      0xB062EC, 0xE454EC, 0xEC58B4, 0xEC6A64, 0xD48820, 0xA0AA00, 0x74C400,
      0x4CD020, 0x38CC6C, 0x38B4CC, 0x3C3C3C, 0x000000, 0x000000, 0xECECEC,
      0xA8CCEC, 0xBCBCEC, 0xD4B2EC, 0xECA8EC, 0xECA8D4, 0xECB4B0, 0xE4C490,
      0xCCD278, 0xB4DE78, 0xA8E290, 0x98E2B0, 0xA0D6E4, 0xA0A2A0, 0x000000,
      0x000000};

  if (pixel >= colorPalette.size()) {
    return 0x000000; // Fallback to black if out-of-bounds
  }

  return colorPalette[pixel];
}

void PPU::cpuWrite(uint16_t addr, uint8_t data) //
{
  writeRegister(addr, data);
}

uint8_t PPU::cpuRead(uint16_t addr, bool readOnly) {
  (void)readOnly;
  return readRegister(addr);
}

void PPU::ConnectCartridge(
    const std::shared_ptr<Cartridge> &cartridge) // conecta o cartucho
{
  this->cart = cartridge;
}

void PPU::reset() {
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

void PPU::clock() { renderFrame(); }
