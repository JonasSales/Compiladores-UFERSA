    Projeto Ufersa Patrício Compiladores

# 1. Clone o repositório do GitHub para a sua máquina local.
#    Este comando baixa todos os arquivos do projeto.
git clone [https://github.com/JonasSales/Compiladores-UFERSA.git](https://github.com/JonasSales/Compiladores-UFERSA.git)

# 2. Navegue para o diretório do projeto que você acabou de clonar.
cd Compiladores-UFERSA

# 3. Este comando utiliza o CMake para configurar o projeto e preparar
# o ambiente de compilação na pasta 'cmake-build-debug'.
cmake -S . -B cmake-build-debug 

# 4. Este comando compila o código-fonte, gerando o executável.
# O CMake cuidará de chamar o Flex para gerar o analisador léxico
# e, em seguida, compilará todos os arquivos C++.
cmake --build cmake-build-debug

# 5. Execute o programa passando como argumento o caminho para um arquivo de código.
# O exemplo abaixo utiliza o arquivo 'teste.tonto'.
# A saída será um arquivo JSON com os tokens encontrados.
./bin/Debug/tonto_lexer testes/teste.tonto