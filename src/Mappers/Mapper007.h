#pragma once

#include "../../include/global.h"

class Mapper_007 : public Mapper
{
public:
    Mapper_007(uint8_t prgBanks, uint8_t chrBanks);
    ~Mapper_007();

    bool cpuMapRead(uint16_t addr, uint32_t &mapped_addr, uint8_t &data) override;
    bool cpuMapWrite(uint16_t addr, uint32_t &mapped_addr, uint8_t data) override;
    bool ppuMapRead(uint16_t addr, uint32_t &mapped_addr) override;
    bool ppuMapWrite(uint16_t addr, uint32_t &mapped_addr) override;
    void reset() override;
    MIRROR mirror() override;

private:
    uint8_t nPRGBankSelect = 0;
    MIRROR mirrorMode = ONESCREEN_LO;
}; 