#include "src/Header_files/APU.h"

APU2A03::APU2A03()
{
}

APU2A03::~APU2A03()
{
}

void APU2A03::cpuWrite(uint16_t addr, uint8_t data){
    switch(addr){
        {
            case 0x4000:
                switch((data & 0xC0) >> 6)
                {
                    case 0x00: pulse01_seq.sequence = 0b00000001; break;
                    case 0x01: pulse01_seq.sequence = 0b00000011; break;
                    case 0x02: pulse01_seq.sequence = 0b00001111; break;
                    case 0x03: pulse01_seq.sequence = 0b11111100; break;
                }
                break;
            case 0x4001:
                break;
            case 0x4002:
                pulse01_seq.reload = (pulse01_seq.reload & 0xFF00) | data;
                break;
            case 0x4003:
                pulse01_seq.reload = (uint16_t)((data & 0x07)) << 8 | (pulse01_seq.reload & 0x00FF);
                pulse01_seq.timer = pulse01_seq.reload;
                break;
            case 0x4004:
                break;
            case 0x4005:
                break;
            case 0x4006:
                break;
            case 0x4007:
                break;
            case 0x4008:
                break;
            case 0x400C:
                break;
            case 0x400E:
                break;
            case 0x4015:
                pulse01_enabled = data & 0x01;
                break;
            case 0x400F:
                break;
        }
    }
}

uint8_t APU2A03::cpuRead(uint16_t addr)
{
    (void)addr;
    return 0x00;
}

void APU2A03::clock()
{
    bool bQuarterFrameClock = false;
    bool bHalfFrameClock = false;

    dGlobalTime += (0.3333333333 / 1789773.0);

    if(clock_counter % 6 == 0)
    {
        frame_clock_counter++;

        if (frame_clock_counter == 3729)
		{
			bQuarterFrameClock = true;
		}

		if (frame_clock_counter == 7457)
		{
			bQuarterFrameClock = true;
			bHalfFrameClock = true;
		}

		if (frame_clock_counter == 11186)
		{
			bQuarterFrameClock = true;
		}

		if (frame_clock_counter == 14916)
		{
			bQuarterFrameClock = true;
			bHalfFrameClock = true;
			frame_clock_counter = 0;
		}

        if (bQuarterFrameClock)
        {

        }

        if (bHalfFrameClock)
        {

        }

        pulse01_seq.clock(pulse01_enabled, [](uint32_t &s){
            s = ((s & 0x0001 << 7) | (s & 0x00FE) >> 1);
        });

        pulse01_osc.frequency = 1789773.0 / (16.0*(double)(pulse01_seq.reload+1));
        pulse01_sample = pulse01_osc.sample(dGlobalTime);

        clock_counter++;
    }
}

void APU2A03::reset()
{
}

double APU2A03::GetOutputSample()
{
    return pulse01_sample;
}
