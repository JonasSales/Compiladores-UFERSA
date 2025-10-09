    Projeto Ufersa Patrício Compiladores

# 1. Clone o repositório do GitHub para a sua máquina local.
    git clone [https://github.com/JonasSales/Compiladores-UFERSA.git](https://github.com/JonasSales/Compiladores-UFERSA.git)

# 2. Navegue para o diretório do projeto que você acabou de clonar.
    cd Compiladores-UFERSA

# 3. Este comando utiliza o CMake para configurar o projeto e preparar o ambiente de compilação na pasta 'cmake-build-debug'.
    cmake -S . -B cmake-build-debug 

# 4. Este comando compila o código-fonte, gerando o executável (cria pastas bin e debug)
    cmake --build cmake-build-debug

# 5. Execute o programa passando como argumento o caminho para um arquivo de código.
    ./bin/Debug/tonto_lexer testes/teste.tonto