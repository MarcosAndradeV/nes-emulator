#ifndef EMULATOR_H_
#define EMULATOR_H_

/// Responsavel por inicialização, deinicialização e event loop.
class Emulator {
public:
    Emulator();
    ~Emulator();
    void run();
};

#endif // EMULATOR_H_
