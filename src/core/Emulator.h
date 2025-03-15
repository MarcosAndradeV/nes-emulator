#ifndef EMULATOR_H
#define EMULATOR_H

/// Responsavel por inicialização, deinicialização e event loop.
class Emulator {
public:
    Emulator();
    ~Emulator();
    void run();
};

#endif // EMULATOR_H