#pragma once

#include "../../include/global.h"

class Mapper_004 : public Mapper
{
public:
	Mapper_004(uint8_t prgBanks, uint8_t chrBanks);
	~Mapper_004();

	bool cpuMapRead(uint16_t addr, uint32_t &mapped_addr, uint8_t &data) override;
	bool cpuMapWrite(uint16_t addr, uint32_t &mapped_addr, uint8_t data = 0) override;
	bool ppuMapRead(uint16_t addr, uint32_t &mapped_addr) override;
	bool ppuMapWrite(uint16_t addr, uint32_t &mapped_addr) override;
	void reset() override;

	bool irqState() override;
	void irqClear() override;

	void scanline() override;
	MIRROR mirror() override;

private:
	// Control variables
	uint8_t nTargetRegister = 0x00;
	bool bPRGBankMode = false;
	bool bCHRInversion = false;
	MIRROR mirrormode = MIRROR::HORIZONTAL;

	uint32_t pRegister[8];
	uint32_t pCHRBank[8];
	uint32_t pPRGBank[4];

	bool bIRQActive = false;
	bool bIRQEnable = false;
	bool bIRQUpdate = false;
	uint16_t nIRQCounter = 0x0000;
	uint16_t nIRQReload = 0x0000;

	std::vector<uint8_t> vRAMStatic;
};
