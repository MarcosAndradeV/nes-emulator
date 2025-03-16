:i count 1
:b shell 16
build/TestMemory
:i returncode 0
:b stdout 225


Iniciando os testes de memória...
Teste 1 PASSOU ✅ (Leitura e escrita OK)
Teste 2 PASSOU ✅ (Acesso ao final da RAM OK)
Teste 3 PASSOU ✅ (Leitura fora da RAM retornou 0)
Teste 3 FALHOU ❌ (Esperado 124, mas leu 123)

:b stderr 0

