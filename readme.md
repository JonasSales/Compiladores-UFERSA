# Compiladores: Analisador Léxico para a Linguagem TONTO

Este projeto é um analisador léxico (lexer) para a linguagem de modelagem de ontologias "TONTO". O analisador é implementado em C++ e Flex, com o build gerenciado pelo CMake.

## Autores

- [@Jonas Sales](https://www.github.com/JonasSales)
- [@Guilherme Melo](https://github.com/glhermeMelo)

## Visão Geral do Projeto

O objetivo deste projeto é processar arquivos-fonte escritos na linguagem `.tonto` e convertê-los em uma sequência de tokens. A lógica é dividida nos seguintes componentes:

* **Lexer (Flex)**: O arquivo `analisador_lexico/lexer/lexer.l` define as expressões regulares para reconhecer os blocos básicos da linguagem (palavras-chave, identificadores, números, strings, símbolos, etc.).
* **Gerenciador de Palavras-Chave**: Um sistema (em `analisador_lexico/lexer_utils/KeyWordGroup`) é usado para classificar identificadores. Por exemplo, o lexer pode identificar `kind` como um `RELATION_NAME` genérico, e este gerenciador o reclassifica corretamente como `CLASS_STEREOTYPE`.
* **Entidades**: A classe `Token` (em `analisador_lexico/entities/`) armazena as informações de cada token, como linha, coluna, tipo e lexema.
* **Saída JSON**: O programa gera dois arquivos JSON como saída, localizados na pasta `testes/`:
    1.  `lexerTokenAnalisys.json`: Um log detalhado de *todos* os tokens encontrados, um a um, com suas posições.
    2.  `lexerTokenCount.json`: Uma **tabela de síntese** que agrupa os tokens por tipo, mostrando a contagem total e uma lista de lexemas **únicos** encontrados para cada tipo.

## Estrutura do projeto

O projeto é organizado nas seguintes pastas:

* **/analisador_lexico**: Contém toda a regra de negócio do analisador.
    * **/entities**: Classes que representam as entidades do projeto, como `Token` e `TokenAnalisys`.
    * **/lexer**: Arquivo `lexer.l` com a definição das regras do Flex.
    * **/lexer_utils**: Arquivos de configuração e o gerenciador de palavras-chave.
    * **/util**: Funções auxiliares para escrever os arquivos de saída `.json`.
* **/testes**: Pasta para armazenar os arquivos `.tonto` de entrada e os `.json` de saída.
* **/cmake-build-debug**: Pasta gerada pelo CMake que contém os executáveis (ignorada pelo `.gitignore`).

## Como Preparar o Ambiente

Abra um terminal e instale as dependências necessárias para o build:

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

Após a compilação, os arquivos .json com a análise do código estarão na pasta testes com os seguintes nomes:

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
