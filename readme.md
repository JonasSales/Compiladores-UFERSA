
# Compiladores
Projeto do analisador léxico para a linguagem Tonto


# Autores

- [@Jonas Sales](https://www.github.com/JonasSales)
- [@Guilherme Melo](https://github.com/glhermeMelo)


# Estrutura do projeto
Separados em pastas com os nomes

## analisador_lexico
Regra de negócio do analisador léxico

### entities
Classe Token (.cpp e .h) para a criação dos tokens

### lexer
Lexer está localizado nesta pastas

### lexer_utils

- keyword_group (pasta para armazenar as classes para cada tipo de token)
- lex_config (enum com os tipos de token, mapa para armazenar tokens e função tokenToString para coverter o enum em uma string)

### util
Funções auxiliares escrever os arquivos .json

## testes
Pasta para armazenar os arquivos .tonto para os testes e .json com os resultados dos testes

# Preparar ambiente

Abra o terminal

Instale dependências

```bash
sudo apt update
  sudo apt upgrade 
  sudo apt install git
  sudo apt install build-essential
  sudo apt install g++ gdb
  sudo apt install make cmake
  sudo apt install flex
```

Clone o projeto

```bash
  cd ~
  git clone https://github.com/JonasSales/Compiladores-UFERSA.git
```

Vá para o diretório do projeto

```bash
  cd Compiladores-UFERSA
```


# Rodar localmente

- Para rodar o projeto, abra a pasta teste na raiz do projeto
```bash
cd testes/
```

- Abra o arquivo teste.tonto na pasta testes e cole o texto de teste

```bash
  cd ..
```

Configure o CMake para criar as pastas necessárias para o projeto

Compilar o código fonte

```bash
  mkdir cmake-build-debug && cd cmake-build-debug && cmake .. && cmake --build . && cd ..
```

Rodar teste

```bash
  ./cmake-build-debug/tonto_lexer testes/teste.tonto
```

Após a compilação, os arquivos .json com a análise do código estarão na par

## lexerTokenAnalisys.json
Mapeamento um a um dos tokens gerados com os atributos
- id: Ordem na qual os tokens foram gerados
- linha: Linha onde o token foi encontrado
- coluna: Coluna do token na linha
- tipo: Tipo do Token
- lexema: Lexema

## lexerTokenCount.json
Arquivo para a contagem dos tokens separados por tipo
- Tipo do token
- Contagem de tokens desse tipo
- Lista com todos os tokens encontrados na ordem (com repetição)
