#pragma once

#include <cstdint>
#include <functional>
class APU2A03{
    public:
        APU2A03();
        ~APU2A03();
        void clock();
        void cpuWrite(uint16_t addr, uint8_t data);
        uint8_t cpuRead(uint16_t addr);
        void reset();
        double GetOutputSample();


    private:

        uint32_t frame_clock_counter = 0;
        uint32_t clock_counter = 0;
        struct sequencer
        {
            uint32_t sequence = 0x000000000;
            uint16_t timer = 0x0000;
            uint16_t reload = 0x0000;
            uint8_t output = 0x00;
            uint8_t clock(bool bEnable, std::function<void(uint32_t &s)> funcManipulate)
            {
                if(bEnable)
                {
                    timer--;
                    if(timer == 0xFFFF)
                    {
                        timer = reload+1;
                        funcManipulate(sequence);
                        output = sequence & 0x00000001;
                    }
                }
                return output;
            }
        };

    struct oscpulse
	{
		double frequency = 0;
		double dutycycle = 0;
		double amplitude = 1;
		double pi = 3.14159;
		double harmonics = 20;

		double sample(double t)
		{
			double a = 0;
			double b = 0;
			double p = dutycycle * 2.0 * pi;

            auto approxsin = [](double t)
			{
				double j = t * 0.15915;
				j = j - (int)j;
				return 20.785 * j * (j - 0.5) * (j - 1.0);
			};

			for (double n = 1; n < harmonics; n++)
			{
				double c = n * frequency * 2.0 * pi * t;
				a += -approxsin(c) / n;
				b += -approxsin(c - p * n) / n;
			}

			return (2.0 * amplitude / pi) * (a - b);
		}
	};

        sequencer pulse01_seq;
        oscpulse pulse01_osc;
        bool pulse01_enabled = false;
        double pulse01_sample = 0.0;
        double dGlobalTime = 0.0;
};
