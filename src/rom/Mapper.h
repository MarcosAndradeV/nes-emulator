#ifndef MAPPER_H
#define MAPPER_H

#include <cstdint>
#include <vector>

class Mapper {
public:
    virtual uint8_t cpuRead(uint16_t address) = 0;  // Lê da ROM
    virtual void cpuWrite(uint16_t address, uint8_t data) = 0;  // Escreve na ROM

    virtual ~Mapper() {}  // Destrutor virtual
};

#endif


