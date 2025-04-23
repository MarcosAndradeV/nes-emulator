#pragma once

#include "src/Mappers/Mapper.h"
#include <array>
class Mapper_005 : public Mapper
{
public:
    Mapper_005(uint8_t prgBanks, uint8_t chrBanks);
    ~Mapper_005();

    bool cpuMapRead(uint16_t addr, uint32_t &mapped_addr, uint8_t &data) override;
    bool cpuMapWrite(uint16_t addr, uint32_t &mapped_addr, uint8_t data) override;
    bool ppuMapRead(uint16_t addr, uint32_t &mapped_addr) override;
    bool ppuMapWrite(uint16_t addr, uint32_t &mapped_addr) override;
    void reset() override;
    MIRROR mirror() override;
    bool irqState() override;
    void irqClear() override;
    void scanline() override;

private:
    // PRG ROM Banking
    uint8_t prgMode = 0;
    uint8_t prgBank[4] = {0};

    // CHR Banking
    uint8_t chrMode = 0;
    uint8_t chrBank[12] = {0};

    // Nametable Mirroring
    uint8_t ntMode = 0;
    uint8_t ntRam[4][0x400] = {};

    // IRQ
    bool irqEnable = false;
    bool irqPending = false;
    uint16_t irqCounter = 0;
    uint16_t irqLatch = 0;

    // Expansion Audio
    uint8_t exSound[4] = {0};

    // RAM
    std::array<uint8_t, 0x2000> prgRam;
    std::array<uint8_t, 0x2000> chrRam;

    // Internal Registers
    uint8_t mul1 = 0;
    uint8_t mul2 = 0;
    uint16_t mulResult = 0;
};
