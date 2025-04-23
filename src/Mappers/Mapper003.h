#pragma once

#include "src/Mappers/Mapper.h"
using namespace std;

class Mapper_003 : public Mapper
{
public:
	Mapper_003(uint8_t prgBanks, uint8_t chrBanks);
	~Mapper_003();

	bool cpuMapRead(uint16_t addr, uint32_t &mapped_addr, uint8_t &data) override;
	bool cpuMapWrite(uint16_t addr, uint32_t &mapped_addr, uint8_t data = 0) override;
	bool ppuMapRead(uint16_t addr, uint32_t &mapped_addr) override;
	bool ppuMapWrite(uint16_t addr, uint32_t &mapped_addr) override;
	void reset() override;

private:
	uint8_t nCHRBankSelect = 0x00;
};
