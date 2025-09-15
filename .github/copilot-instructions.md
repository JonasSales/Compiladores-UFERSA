# Copilot Instructions for untitled2

## Visão Geral
Este projeto é um analisador léxico (lexer) para uma linguagem chamada "tonto". O código principal está em `tonto_lexer.cpp`, com um cabeçalho mínimo em `tonto_lexer.h`. O projeto é construído usando CMake (veja `CMakeLists.txt`).

## Componentes Principais
- **Lexer**: Classe principal que processa o texto fonte e gera tokens. Os tipos de tokens estão definidos no enum `TokenType`.
- **Token**: Estrutura que representa um token, incluindo tipo, lexema, linha e coluna.
- **KEYWORDS_ARR**: Array de palavras-chave reconhecidas pela linguagem.
- **Funções de utilidade**: `is_keyword` verifica se um identificador é palavra-chave.

## Fluxo de Dados
- O programa lê um arquivo `.tonto` passado como argumento na linha de comando.
- O conteúdo do arquivo é processado pelo `Lexer`, que gera tokens sequencialmente.
- Cada token é impresso no formato `[linha:coluna] TIPO 'lexema'`.

## Convenções Específicas
- **Comentários**: Suporte para comentários de linha (`//`) e bloco (`/* ... */`).
- **Identificadores**: Podem conter letras, dígitos, `_` e `.`.
- **Strings**: Suportam escape para `\n` e outros caracteres.
- **Operadores e Símbolos**: Reconhecimento especial para sequências como `<o>--`, `<->--`, `--`, além de símbolos únicos.
- **Tokens desconhecidos**: Qualquer caractere não reconhecido é marcado como `TK_UNKNOWN`.

## Build e Execução
- Para compilar, use o CMake:
  ```bash
  mkdir -p build && cd build
  cmake ..
  make
  ```
- Para rodar o lexer:
  ```bash
  ./untitled2 caminho/para/arquivo.tonto
  ```

## Padrões do Projeto
- Código C++ moderno (C++20).
- O lexer é implementado como uma classe única, sem dependências externas além da STL.
- O cabeçalho `tonto_lexer.h` está vazio, pois toda a lógica está em `tonto_lexer.cpp`.
- Mensagens de erro e uso estão em português.

## Exemplos de Uso
- Veja o arquivo `teste.tonto` para exemplos de entrada.
- O output do lexer mostra tokens com tipo, posição e lexema.

## Pontos de Atenção para Agentes
- Sempre mantenha a lógica de tokens e palavras-chave consistente com o array `KEYWORDS_ARR`.
- Se adicionar novos tipos de tokens ou símbolos, atualize o fluxo de impressão no `main`.
- O projeto não possui testes automatizados nem integração contínua.
- Não há dependências externas ou integração com outros sistemas.

## Arquivos Importantes
- `tonto_lexer.cpp`: Implementação principal do lexer.
- `CMakeLists.txt`: Configuração de build.
- `teste.tonto`: Exemplo de arquivo de entrada.

---

Se alguma seção estiver incompleta ou faltando detalhes importantes, por favor, informe para que eu possa aprimorar as instruções.