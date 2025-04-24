#pragma once

#include <string>
#include <memory>

class UI;

class Emulator {
public:
    Emulator();
    ~Emulator();
    
    void run();
    bool loadGame(const std::string& path);
    
private:
    std::unique_ptr<UI> ui;
};