#include "../../include/global.h"

Mapper_066::Mapper_066(uint8_t prgBanks, uint8_t chrBanks) : Mapper(prgBanks, chrBanks)
{
}


Mapper_066::~Mapper_066()
{
}

bool Mapper_066::cpuMapRead(uint16_t addr, uint32_t &mapped_addr, uint8_t &data)
{
	if (addr >= 0x8000 && addr <= 0xFFFF)
	{
		mapped_addr = nPRGBankSelect * 0x8000 + (addr & 0x7FFF);
		return true;
	}
	else
		return false;
}

bool Mapper_066::cpuMapWrite(uint16_t addr, uint32_t &mapped_addr, uint8_t data)
{
	if (addr >= 0x8000 && addr <= 0xFFFF)
	{
		nCHRBankSelect = data & 0x03;
		nPRGBankSelect = (data & 0x30) >> 4;
	}
	
	// Mapper has handled write, but do not update ROMs
	return false;
}

bool Mapper_066::ppuMapRead(uint16_t addr, uint32_t &mapped_addr)
{
	if (addr < 0x2000)
	{
		mapped_addr = nCHRBankSelect * 0x2000 + addr;
		return true;
	}
	else
		return false;
}

bool Mapper_066::ppuMapWrite(uint16_t addr, uint32_t &mapped_addr)
{
	return false;
}

void Mapper_066::reset()
{
	nCHRBankSelect = 0;
	nPRGBankSelect = 0;
}