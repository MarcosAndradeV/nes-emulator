#pragma once

#include "../../include/global.h"

using namespace std;

class Mapper_001 : public Mapper
{
	public:
		Mapper_001(uint8_t prgBanks, uint8_t chrBanks);
		~Mapper_001();

		bool cpuMapRead(uint16_t addr, uint32_t &mapped_addr, uint8_t &data) override;
		bool cpuMapWrite(uint16_t addr, uint32_t &mapped_addr, uint8_t data = 0) override;
		bool ppuMapRead(uint16_t addr, uint32_t &mapped_addr) override;
		bool ppuMapWrite(uint16_t addr, uint32_t &mapped_addr) override;
		void reset() override;
		MIRROR mirror();

	private:
		uint8_t nCHRBankSelect4Lo = 0x00;
		uint8_t nCHRBankSelect4Hi = 0x00;
		uint8_t nCHRBankSelect8 = 0x00;

		uint8_t nPRGBankSelect16Lo = 0x00;
		uint8_t nPRGBankSelect16Hi = 0x00;
		uint8_t nPRGBankSelect32 = 0x00;

		uint8_t nLoadRegister = 0x00;
		uint8_t nLoadRegisterCount = 0x00;
		uint8_t nControlRegister = 0x00;

		MIRROR mirrormode = MIRROR::HORIZONTAL;

		vector<uint8_t> vRAMStatic;
};
