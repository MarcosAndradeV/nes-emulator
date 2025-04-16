#include "../../include/global.h"

Mapper::Mapper(uint8_t prgBanks, uint8_t chrBanks) 
{
	nPRGBanks = prgBanks;
	nCHRBanks = chrBanks;

	reset();
}

Mapper::~Mapper()
{
}

void Mapper::reset()
{

}

MIRROR Mapper::mirror()
{
	return MIRROR::HARDWARE;
}

bool Mapper::irqState()
{
	return false;
}

void Mapper::irqClear()
{
}

void Mapper::scanline()
{
}