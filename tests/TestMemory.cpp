#include <iostream>
#include "src/cpu/Memory.h"

using namespace std;

void Testes(Memory& memory);

int main() 
{
    Memory memory;  // Criar instância da memória

    cout << "\n\nIniciando os testes de memória...\n";

    Testes(memory);
    return 0;
}

void Testes(Memory& memory)
{

    // Teste 1: Escrever e ler dentro da RAM
    memory.write(0x0000, 42);  // Escreve o valor 42 no endereço 0x0000
    uint8_t value = memory.read(0x0000);  // Lê de volta o valor
  
    if (value == 42) 
    {
        cout << "Teste 1 PASSOU ✅ (Leitura e escrita OK)\n";
    } 
    else 
    {
        cout << "Teste 1 FALHOU ❌\n";
    }
  
    // Teste 2: Escrever e ler em outro endereço da RAM
    memory.write(0x07FF, 99);  // Último byte da RAM
    value = memory.read(0x07FF);
  
    if (value == 99) 
    {
        cout << "Teste 2 PASSOU ✅ (Acesso ao final da RAM OK)\n";
    } 
    else 
    {
        cout << "Teste 2 FALHOU ❌\n";
    }
  
    // Teste 3: Ler um endereço FORA da RAM (Deve ser 0)
    value = memory.read(0x0800);
  
    if (value == 0) 
    {
        cout << "Teste 3 PASSOU ✅ (Leitura fora da RAM retornou 0)\n";
    } 
    else 
    {
        cout << "Teste 3 FALHOU ❌\n";
    }

    // Teste 4: Escrever e ler um valor incorreto (erro)
    memory.write(0x0001, 123);  // Escreve o valor 123 no endereço 0x0001
    value = memory.read(0x0001);

    // Aqui estamos esperando um valor diferente de 123 para forçar um erro
    if (value == 124) 
    {
        cout << "Teste 3 PASSOU ✅ (Valor incorreto, mas esperado)\n";
    } 
    else 
    {
        cout << "Teste 3 FALHOU ❌ (Esperado 124, mas leu " << (int)value << ")\n";
    }
}