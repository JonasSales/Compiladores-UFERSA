# Compiladores: Analisador Léxico para a Linguagem TONTO

Este projeto é um analisador léxico e sintático para a linguagem de modelagem de ontologias "TONTO". O analisador é implementado em C++, Flex e Bison, com o build gerenciado pelo CMake. Todo o processo de compilação e geração de arquivos foi automatizado para facilitar o uso.

## Autores

- [@Jonas Sales](https://www.github.com/JonasSales)
- [@Guilherme Melo](https://github.com/glhermeMelo)

## Estrutura do projeto

```
.
├── analisador_lexico            //Pacote para analisador léxico
│   ├── entities                 //Classes utilizadas na análise léxica
│   │   ├── TokenAnalisys.cpp    
│   │   ├── TokenAnalisys.h      //Definição de como será a escrita da análise léxica           
│   │   └── Token.cpp e .h       //Definição do que é um Token e suas funções
│   ├── lexer                    //Analisador léxico
│   │   └── lexer.l             
│   └── lexer_utils              
│       ├── KeyWordGroup         //Definição de todas as palavras reservadas para a linguaguem         
│       └── lex_config.cpp e .h  //Função para tratar processar os Tokens de entrada e saída
├── analisador_sintatico         //Pacote para analisador sintático
│   ├── Parser.h                 //Definição do parser e todos os structs usandos na síntese 
│   └── parser.y                 //Definição das regras gramaticais
├── build                        //Pasta gerada automaticamente após compilação
├── CMakeLists.txt               //CMake para automatizar processo de compilação
├── main.cpp                     //Lógica de funcionamento do código
├── readme.md
├── testes                       //Pasta para se colocar os arquivos da ontologia e armazenar as saídas das análises léxica e sintática
│   ├── Hospital_Model           //Ex: Hospital
│   ├── Pizzaria_Model           //Ex: Pizzaria
|   └── TDAH                     //Ex: TDAH
└── util                         //Funções para gerar Json com as análises 

```

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
**Assumindo que as ferramentas Bison, Flex, build-essential, git , make etc estão instaladas**

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
mkdir -p build && cd build && rm -rf * && cmake .. && make -j$(nproc)
```

Rodar teste

```bash
  ./tonto_lexer
```
## Após rodar o teste

O terminal exibe um breve resumo do que foi encontrado no arquivo .tonto assim como erros encontrados e sugestões de tratamentos, a análise mais aprofundada estará nos arquivos .json
```
Iniciando analise lexica com Flex para alergiaAlimentar.tonto
Analise lexica salva em "../testes/FoodAllergyExample/alergiaAlimentarLexerAnalysis.json"
Iniciando analise sintatica com Bison para alergiaAlimentar.tonto
syntax error (linha 19, coluna 36): unexpected "specializes", expecting CLASS_NAME or RELATION_NAME or INSTANCE_NAME
syntax error (linha 19, coluna 48): unexpected "Alergia", expecting EOF
Analise sintatica salva em "../testes/FoodAllergyExample/alergiaAlimentarSyntaxAnalysis.json"

========================================
      RESUMO DA ANÁLISE SINTÁTICA       
========================================
Pacotes encontrados:   1
Classes/Stereotypes:   8
Gensets (Disjunções):  0
Datatypes Custom:      0
Enums:                 0
Relações Externas:     0
Erros Sintáticos:      2
========================================
```

Após a compilação, os arquivos .json com as análises do código estarão na pasta testes com os seguintes nomes:
```
Ex:
...LexerAnalysis.json     //Arquivo .json com a análise léxica dos tokens 
...SyntaxAnalysis.json    //Arquivo .json com a anĺise sintática do arquivo
ConsultaMedica.tonto      //Arquivo original .tonto
```  
```
 .
├── Hospital_Model                           //Pasta para o teste da ontoglogia do Hospital
│   ├── ConsultaMedicaLexerAnalysis.json     //Arquivo .json com a análise léxica do arquivo ConsultaMedica.json
│   ├── ConsultaMedicaSyntaxAnalysis.json    //Arquivo .json com a análise sintática do arquivo ConsultaMedica.tonto
│   ├── ConsultaMedica.tonto                 //Arquivo .tonto da Consulta Médica
.   .
.   .
.   .
```

**Estrutura do arquivo LexerAnalysis.json** 

``` 
[
  {                  
    "id": 1,                   // Identificador único e sequencial do Token
    "linha": 12,               // Número da linha onde o Token foi encontrado no arquivo fonte
    "coluna": 5,               // Número da coluna inicial do Token
    "tipo": "CLASS_NAME",      // Categoria do Token (Ex: CLASS_NAME, RESERVED_WORD, SYMBOL)
    "lexema": "Paciente"       // O texto exato capturado do arquivo fonte
  },
  // ... demais tokens
]
```

**Estrutura do arquivo SyntaxAnalysis.json** 

```
{
  "diagrama_info": [                     // Lista de relações externas identificadas para fins de diagramação
      {
        "tipo": "relacao",               // Tipo do elemento gráfico
        "descricao_visual": "Paciente \"1..*\" -- \"1..*\" Medico : consultado_Por", // Representação textual da relação
        "detalhes": {
          "source": "Paciente",          // Classe de origem da relação
          "target": "Medico",            // Classe de destino da relação
          "estereotipo": "@material"     // Estereótipo da relação (se houver)
        }
      }
  ],
  "Tabela de Sintese": {                 // Estrutura extraída do código
    "pacotes": [
       "ConsultaMedica"                  // Lista de nomes dos pacotes encontrados
    ],               
    "classes": [                         // Lista de todas as classes, roles, kinds, etc. encontradas
      {
        "pacote": "ConsultaMedica",      // Pacote ao qual a classe pertence
        "estereotipo": "relator",        // Estereótipo da classe (ex: kind, role, relator)
        "nome": "Consulta_Medica",       // Nome do identificador da classe
        "herdaDe": "",                   // Nome da classe pai (se houver herança via 'specializes')
        "atributos": [                   // Lista de atributos ou relações internas da classe
           { 
             "nome": "tipo_De_Consulta", 
             "tipo": "Tipo_De_Consulta", 
             "meta": ""                  // Meta-atributo (se houver)
           }
        ]
      }
    ],               
    "generalizationSets": [             // Conjuntos de generalização (gensets)
      {
        "nome": "Tipo_Do_Item",         // Nome do genset
        "general": "Item_Da_Pizzaria",  // Classe geral (pai)
        "disjoint": true,               // Se é disjunto (boolean)
        "complete": true,               // Se é completo (boolean)
        "specifics": [                  // Lista das classes específicas (filhas)
           "Pizza", "Bebida"
        ]
      }
    ]
  },
  "Relatorio de Erros": [               // Lista de erros sintáticos encontrados durante o parsing
    {
      "linha": 10,                      // Linha onde o erro ocorreu
      "coluna": 5,                      // Coluna estimada do erro
      "mensagem": ""                    // Mensagem descritiva do erro
    }
  ]
}
```

