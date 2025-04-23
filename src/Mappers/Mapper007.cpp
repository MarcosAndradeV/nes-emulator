#include "../../include/global.h"

Mapper_007::Mapper_007(uint8_t prgBanks, uint8_t chrBanks) : Mapper(prgBanks, chrBanks)
{
    reset();
}

Mapper_007::~Mapper_007()
{
}

void Mapper_007::reset()
{
    nPRGBankSelect = 0;
    mirrorMode = ONESCREEN_LO;
}

bool Mapper_007::cpuMapRead(uint16_t addr, uint32_t &mapped_addr, uint8_t &data)
{
    if (addr >= 0x8000 && addr <= 0xFFFF)
    {
        // Map the entire 32KB PRG ROM bank
        mapped_addr = (nPRGBankSelect * 0x8000) + (addr & 0x7FFF);
        return true;
    }

    return false;
}

bool Mapper_007::cpuMapWrite(uint16_t addr, uint32_t &mapped_addr, uint8_t data)
{
    if (addr >= 0x8000 && addr <= 0xFFFF)
    {
        // Bank select and mirroring control
        nPRGBankSelect = data & 0x07;
        mirrorMode = (data & 0x10) ? ONESCREEN_HI : ONESCREEN_LO;
        return false; // No write to PRG ROM
    }

    return false;
}

bool Mapper_007::ppuMapRead(uint16_t addr, uint32_t &mapped_addr)
{
    if (addr >= 0x0000 && addr <= 0x1FFF)
    {
        mapped_addr = addr;
        return true;
    }

    return false;
}

bool Mapper_007::ppuMapWrite(uint16_t addr, uint32_t &mapped_addr)
{
    if (addr >= 0x0000 && addr <= 0x1FFF)
    {
        if (nCHRBanks == 0)
        {
            // Treat as RAM
            mapped_addr = addr;
            return true;
        }
    }

    return false;
}

MIRROR Mapper_007::mirror()
{
    return mirrorMode;
} 