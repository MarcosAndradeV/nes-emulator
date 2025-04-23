#include "src/Mappers/Mapper005.h"

Mapper_005::Mapper_005(uint8_t prgBanks, uint8_t chrBanks) : Mapper(prgBanks, chrBanks)
{
    reset();
}

Mapper_005::~Mapper_005()
{
}

void Mapper_005::reset()
{
    prgMode = 0;
    for (int i = 0; i < 4; i++) prgBank[i] = 0;

    chrMode = 0;
    for (int i = 0; i < 12; i++) chrBank[i] = 0;

    ntMode = 0;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 0x400; j++)
            ntRam[i][j] = 0;

    irqEnable = false;
    irqPending = false;
    irqCounter = 0;
    irqLatch = 0;

    for (int i = 0; i < 4; i++) exSound[i] = 0;

    prgRam.fill(0);
    chrRam.fill(0);

    mul1 = 0;
    mul2 = 0;
    mulResult = 0;
}

bool Mapper_005::cpuMapRead(uint16_t addr, uint32_t &mapped_addr, uint8_t &data)
{
    if (addr >= 0x4020 && addr <= 0x5FFF)
    {
        // Expansion Audio and other registers
        switch (addr)
        {
            case 0x5204: // IRQ Status
                data = (irqPending ? 0x80 : 0x00) | (irqEnable ? 0x40 : 0x00);
                irqPending = false;
                return false;
            case 0x5205: // Multiplication Result (low)
                data = mulResult & 0xFF;
                return false;
            case 0x5206: // Multiplication Result (high)
                data = mulResult >> 8;
                return false;
        }
        return false;
    }

    if (addr >= 0x6000 && addr <= 0x7FFF)
    {
        // PRG RAM
        mapped_addr = addr & 0x1FFF;
        data = prgRam[mapped_addr];
        return true;
    }

    if (addr >= 0x8000 && addr <= 0xFFFF)
    {
        // PRG ROM
        uint8_t bank = 0;
        if (prgMode == 0)
        {
            // 32KB mode
            bank = prgBank[0] >> 1;
            mapped_addr = (bank * 0x8000) + (addr & 0x7FFF);
        }
        else
        {
            // 16KB mode
            if (addr < 0xC000)
                bank = prgBank[0];
            else
                bank = prgBank[1];
            mapped_addr = (bank * 0x4000) + (addr & 0x3FFF);
        }
        return true;
    }

    return false;
}

bool Mapper_005::cpuMapWrite(uint16_t addr, uint32_t &mapped_addr, uint8_t data)
{
    if (addr >= 0x5000 && addr <= 0x5015)
    {
        // Expansion Audio
        exSound[addr - 0x5000] = data;
        return false;
    }

    if (addr >= 0x5100 && addr <= 0x5103)
    {
        // PRG Mode
        prgMode = data & 0x03;
        return false;
    }

    if (addr >= 0x5113 && addr <= 0x5117)
    {
        // PRG Banking
        prgBank[addr - 0x5113] = data;
        return false;
    }

    if (addr >= 0x5120 && addr <= 0x512B)
    {
        // CHR Banking
        chrBank[addr - 0x5120] = data;
        return false;
    }

    if (addr >= 0x5130 && addr <= 0x5133)
    {
        // CHR Mode
        chrMode = data & 0x03;
        return false;
    }

    if (addr >= 0x5200 && addr <= 0x5206)
    {
        // IRQ and Multiplication
        switch (addr)
        {
            case 0x5200: // IRQ Counter (low)
                irqCounter = (irqCounter & 0xFF00) | data;
                break;
            case 0x5201: // IRQ Counter (high)
                irqCounter = (irqCounter & 0x00FF) | (data << 8);
                break;
            case 0x5202: // IRQ Enable
                irqEnable = (data & 0x80) != 0;
                break;
            case 0x5203: // IRQ Status
                irqPending = false;
                break;
            case 0x5205: // Multiplicand
                mul1 = data;
                mulResult = mul1 * mul2;
                break;
            case 0x5206: // Multiplier
                mul2 = data;
                mulResult = mul1 * mul2;
                break;
        }
        return false;
    }

    if (addr >= 0x6000 && addr <= 0x7FFF)
    {
        // PRG RAM
        mapped_addr = addr & 0x1FFF;
        prgRam[mapped_addr] = data;
        return true;
    }

    return false;
}

bool Mapper_005::ppuMapRead(uint16_t addr, uint32_t &mapped_addr)
{
    if (addr >= 0x0000 && addr <= 0x1FFF)
    {
        // CHR
        if (nCHRBanks == 0)
        {
            // CHR RAM
            mapped_addr = addr;
            return true;
        }
        else
        {
            // CHR ROM
            uint8_t bank = 0;
            if (chrMode == 0)
            {
                // 8KB mode
                bank = chrBank[0] >> 1;
                mapped_addr = (bank * 0x2000) + (addr & 0x1FFF);
            }
            else
            {
                // 4KB mode
                bank = chrBank[addr >> 12];
                mapped_addr = (bank * 0x1000) + (addr & 0x0FFF);
            }
            return true;
        }
    }

    if (addr >= 0x2000 && addr <= 0x3EFF)
    {
        // Nametable
        uint8_t nt = (addr >> 10) & 0x03;
        mapped_addr = ntRam[nt][addr & 0x03FF];
        return true;
    }

    return false;
}

bool Mapper_005::ppuMapWrite(uint16_t addr, uint32_t &mapped_addr)
{
    if (addr >= 0x0000 && addr <= 0x1FFF)
    {
        // CHR
        if (nCHRBanks == 0)
        {
            // CHR RAM
            mapped_addr = addr;
            return true;
        }
        return false;
    }

    if (addr >= 0x2000 && addr <= 0x3EFF)
    {
        // Nametable
        uint8_t nt = (addr >> 10) & 0x03;
        mapped_addr = ntRam[nt][addr & 0x03FF];
        return true;
    }

    return false;
}

MIRROR Mapper_005::mirror()
{
    switch (ntMode)
    {
        case 0: return HORIZONTAL;
        case 1: return VERTICAL;
        case 2: return ONESCREEN_LO;
        case 3: return ONESCREEN_HI;
        default: return HARDWARE;
    }
}

bool Mapper_005::irqState()
{
    return irqPending;
}

void Mapper_005::irqClear()
{
    irqPending = false;
}

void Mapper_005::scanline()
{
    if (irqEnable)
    {
        if (irqCounter == 0)
        {
            irqCounter = irqLatch;
            irqPending = true;
        }
        else
        {
            irqCounter--;
        }
    }
}
