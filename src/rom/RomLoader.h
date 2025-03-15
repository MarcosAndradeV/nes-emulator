#ifndef ROMLOADER_H
#define ROMLOADER_H

#include <vector>
#include <string>
#include <memory>
#include "Mapper.h"

class RomLoader {
public:
    RomLoader();  // Construtor

    bool load(const std::string& filename);  // Carrega uma ROM do disco

    std::vector<uint8_t> prg_rom;  // Armazena os dados de PRG-ROM
    std::vector<uint8_t> chr_rom;  // Armazena os dados de CHR-ROM
    int mapperID;  // ID do mapeador

private:
    bool validateHeader(const std::vector<uint8_t>& data);  // Verifica se o arquivo é válido
};

#endif
