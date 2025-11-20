%language "C++"
%skeleton "lalr1.cc"
%require "3.2"
%defines
%define api.value.type variant
%define api.token.constructor
%define parse.error verbose

%code requires {
    #include <string>
    #include <vector>
    #include <iostream>
    #include "analisador_sintatico/Parser.h"
}

%code {
    #include "analisador_lexico/lexer_utils/lex_config.h"

    yy::parser::symbol_type yylex();

    extern Parser parserData;
}

%token <std::string> CLASS_NAME RELATION_NAME INSTANCE_NAME DATATYPE_NAME
%token <std::string> STRING NUMBER
%token <std::string> CLASS_STEREOTYPE RELATION_STEREOTYPE
%token <std::string> NATIVE_DATA_TYPE

%token PACKAGE IMPORT GENSET DISJOINT COMPLETE GENERAL SPECIFICS
%token SPECIALIZES DATATYPE ENUM RELATION
%token LBRACE "{" RBRACE "}" COLON ":" COMMA "," END_OF_FILE 0 "EOF"
%token META_ATTRIBUTE_START "meta{" META_ATTRIBUTE_END "meta}"

%type <SinteseClasse> class_decl
%type <std::vector<SinteseAtributo>> opt_body body_content
%type <SinteseAtributo> attribute_decl
%type <std::string> type_ref opt_specializes identifier

%type <std::string> opt_disjoint opt_complete

%%


program:
    declarations_list
    ;

declarations_list:
    declarations_list declaration
    | /* vazio */
    ;

declaration:
      package_decl
    | import_decl
    | class_decl {
        // Ao terminar uma classe, salva no vetor global
        parserData.classesEncontradas.push_back($1);
      }
    | genset_decl
    | datatype_decl
    | enum_decl
    | relation_decl
    ;

// --- Regras Auxiliares ---
identifier:
    CLASS_NAME { $$ = $1; }
    | INSTANCE_NAME { $$ = $1; }
    ;

// --- Package & Import ---
package_decl:
    PACKAGE identifier {
        parserData.pacoteAtual = $2;
        parserData.pacotesEncontrados.push_back($2);
        std::cout << "BISON: Pacote " << $2 << std::endl;
    }
    ;

import_decl:
    IMPORT identifier { std::cout << "BISON: Import " << $2 << std::endl; }
    ;

// --- Classe ---
class_decl:
    CLASS_STEREOTYPE identifier opt_specializes opt_body {
        SinteseClasse novaClasse;
        novaClasse.pacote = parserData.pacoteAtual;
        novaClasse.estereotipo = $1;
        novaClasse.nome = $2;
        novaClasse.herdaDe = $3;
        novaClasse.atributos = $4;
        $$ = novaClasse; // Retorna a struct preenchida
        std::cout << "BISON: Classe " << $2 << " (" << $1 << ")" << std::endl;
    }
    ;

opt_specializes:
    SPECIALIZES identifier { $$ = $2; }
    | /* vazio */ { $$ = ""; }
    ;

opt_body:
    "{" body_content "}" { $$ = $2; }
    | /* vazio */ { $$ = std::vector<SinteseAtributo>(); }
    ;

body_content:
    body_content attribute_decl {
        $1.push_back($2);
        $$ = $1;
    }
    | /* vazio */ { $$ = std::vector<SinteseAtributo>(); }
    ;

attribute_decl:
    RELATION_NAME ":" type_ref {
        SinteseAtributo attr;
        attr.nome = $1;
        attr.tipo = $3;
        // attr.metaAtributo = ... (Implementar parse de meta-atributos se necessário)
        $$ = attr;
    }
    ;

type_ref:
    identifier { $$ = $1; }
    | NATIVE_DATA_TYPE { $$ = $1; }
    | DATATYPE_NAME { $$ = $1; }
    ;

// --- Genset ---
genset_decl:
    opt_disjoint opt_complete GENSET identifier "{" GENERAL identifier SPECIFICS class_list "}" {
        SinteseGenset gs;
        gs.nome = $4;
        gs.classeGeral = $7;
        // (Para ser completo, precisaria capturar 'class_list' num vector<string>,
        // mas por enquanto vamos focar em compilar)
        gs.isDisjoint = ($1 == "disjoint");
        gs.isComplete = ($2 == "complete");
        parserData.gensetsEncontrados.push_back(gs);
        std::cout << "BISON: Genset " << $4 << std::endl;
    }
    ;

// Regras que agora retornam std::string explicitamente
opt_disjoint:
      DISJOINT { $$ = std::string("disjoint"); }
    | /* vazio */ { $$ = std::string(""); }
    ;

opt_complete:
      COMPLETE { $$ = std::string("complete"); }
    | /* vazio */ { $$ = std::string(""); }
    ;

class_list:
    class_list "," identifier
    | identifier
    ;

// --- Outros ---
datatype_decl: DATATYPE identifier "{" "}" {
    SinteseDatatype dt; dt.nome = $2; dt.pacote = parserData.pacoteAtual;
    parserData.datatypesEncontrados.push_back(dt);
};

enum_decl: ENUM identifier "{" class_list "}" {
    SinteseEnum en; en.nome = $2; en.pacote = parserData.pacoteAtual;
    parserData.enumsEncontrados.push_back(en);
};

relation_decl:
    RELATION_STEREOTYPE RELATION identifier { /* Lógica para relação nomeada */ }
    | RELATION_STEREOTYPE identifier { /* Lógica para relação sem palavra 'relation' */ }
    ;

%%

void yy::parser::error(const std::string& m) {
    std::cerr << "[ERRO SINTATICO BISON]: " << m << std::endl;
    // Adiciona o erro na estrutura para sair no JSON final
    parserData.addErro(0, 0, m);
}