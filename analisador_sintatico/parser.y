%language "C++"
%skeleton "lalr1.cc"
%require "3.2"
%defines
%define api.value.type variant
%define api.token.constructor
%define parse.error verbose
%locations

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

/* --- Tokens com tipos semânticos --- */
/* Identificadores e literais */
%token <std::string> CLASS_NAME RELATION_NAME INSTANCE_NAME DATATYPE_NAME
%token <std::string> STRING NUMBER
%token <std::string> CLASS_STEREOTYPE RELATION_STEREOTYPE
%token <std::string> NATIVE_DATA_TYPE

/* Novos tokens para suportar relações complexas */
%token <std::string> CARDINALITY    /* Ex: [1], [0..*] */
%token <std::string> RELATION_OP    /* Ex: --, <>, .. */

/* Palavras reservadas e símbolos */
%token PACKAGE IMPORT GENSET
%token <std::string> DISJOINT_COMPLETE
%token GENERAL SPECIFICS
%token SPECIALIZES DATATYPE ENUM RELATION
%token LBRACE "{" RBRACE "}" COLON ":" COMMA "," END_OF_FILE 0 "EOF"
%token META_ATTRIBUTE_START "meta{" META_ATTRIBUTE_END "meta}"

/* --- Tipos de retorno das regras (semânticos) --- */
%type <SinteseClasse> class_decl
%type <std::vector<SinteseAtributo>> opt_body body_content
%type <SinteseAtributo> attribute_decl inner_relation_decl
%type <std::string> type_ref opt_specializes identifier
%type <std::string> opt_disjoint_complete
%type <std::vector<std::string>> class_list

%%

/* ---------------- Programa principal ---------------- */
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
        parserData.classesEncontradas.push_back($1);
    }
    | genset_decl
    | datatype_decl
    | enum_decl
    | relation_decl
    ;

/* --- Auxiliares --- */
identifier:
    CLASS_NAME { $$ = $1; }
    | INSTANCE_NAME { $$ = $1; }
    ;

/* --- Package & Import --- */
package_decl:
    PACKAGE identifier {
        parserData.pacoteAtual = $2;
        parserData.pacotesEncontrados.push_back($2);
        //std::cout << "BISON: Pacote " << $2 << std::endl;
    }
    ;

import_decl:
    IMPORT identifier { std::cout << "BISON: Import " << $2 << std::endl; }
    ;

/* --- Classe --- */
class_decl:
    CLASS_STEREOTYPE identifier opt_specializes opt_body {
        SinteseClasse novaClasse;
        novaClasse.pacote = parserData.pacoteAtual;
        novaClasse.estereotipo = $1;
        novaClasse.nome = $2;
        novaClasse.herdaDe = $3;
        novaClasse.atributos = $4;
        $$ = novaClasse;
        //std::cout << "BISON: Classe/Relator " << $2 << " (" << $1 << ")" << std::endl;
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

/* body_content: Lista recursiva que aceita tanto atributos simples (attribute_decl)
   quanto relações internas (inner_relation_decl)
*/
body_content:
      body_content attribute_decl {
          $1.push_back($2);
          $$ = $1;
      }
    | body_content inner_relation_decl {
          $1.push_back($2);
          $$ = $1;
      }
    | attribute_decl {
          std::vector<SinteseAtributo> v;
          v.push_back($1);
          $$ = v;
      }
    | inner_relation_decl {
          std::vector<SinteseAtributo> v;
          v.push_back($1);
          $$ = v;
      }
    | /* vazio */ {
          $$ = std::vector<SinteseAtributo>();
      }
    ;

/* Atributo simples: nome : tipo */
attribute_decl:
    RELATION_NAME ":" type_ref {
        SinteseAtributo attr;
        attr.nome = $1;
        attr.tipo = $3;
        $$ = attr;
    }
    ;

/* Nova regra para relações internas no estilo Tonto:
   Ex: @mediation -- involvesOwner -- [1] CarAgency
*/
inner_relation_decl:
    RELATION_STEREOTYPE RELATION_OP RELATION_NAME RELATION_OP CARDINALITY type_ref {
        SinteseAtributo attr;
        attr.nome = $3;           /* Nome da relação (ex: involvesOwner) */
        attr.tipo = $6;           /* Tipo alvo (ex: CarAgency) */
        attr.metaAtributo = $1;   /* Estereótipo (ex: @mediation) salvo no campo metaAtributo */

        /* Obs: $5 é a cardinalidade e $2/$4 são as setas.
           Podem ser armazenados se a struct SinteseAtributo for expandida. */

        $$ = attr;
        //std::cout << "BISON: Relação interna " << $3 << " [" << $5 << "] -> " << $6 << std::endl;
    }
    ;

type_ref:
    identifier { $$ = $1; }
    | NATIVE_DATA_TYPE { $$ = $1; }
    | DATATYPE_NAME { $$ = $1; }
    ;

/* --- Genset --- */
opt_disjoint_complete:
      DISJOINT_COMPLETE { $$ = $1; }
    | /* vazio */ { $$ = std::string(""); }
    ;

class_list:
      class_list "," identifier {
          $1.push_back($3);
          $$ = $1;
      }
    | identifier {
          std::vector<std::string> v;
          v.push_back($1);
          $$ = v;
      }
    ;

genset_decl:
    opt_disjoint_complete GENSET identifier "{" GENERAL identifier SPECIFICS class_list "}" {
        SinteseGenset gs;
        gs.nome = $3;
        gs.classeGeral = $6;
        gs.classesEspecificas = $8;
        std::string flags = $1;
        gs.isDisjoint = (flags.find("disjoint") != std::string::npos);
        gs.isComplete = (flags.find("complete") != std::string::npos);

        parserData.gensetsEncontrados.push_back(gs);
    }
    ;

/* --- Datatype, Enum --- */
datatype_decl:
    DATATYPE identifier "{" "}" {
        SinteseDatatype dt;
        dt.nome = $2; dt.pacote = parserData.pacoteAtual;
        parserData.datatypesEncontrados.push_back(dt);
    }
    ;

enum_decl:
    ENUM identifier "{" class_list "}" {
        SinteseEnum en;
        en.nome = $2;
        en.pacote = parserData.pacoteAtual;
        en.instancias = $4;
        parserData.enumsEncontrados.push_back(en);
    }
    ;

/* --- Relation (Declaração externa) --- */
relation_decl:
      RELATION_STEREOTYPE RELATION identifier {
          SinteseRelacao r;
          r.estereotipo = $1;
          r.nome = $3;
          parserData.relacoesExternasEncontradas.push_back(r);
      }
    | RELATION_STEREOTYPE identifier {
          SinteseRelacao r;
          r.estereotipo = $1;
          r.nome = $2;
          parserData.relacoesExternasEncontradas.push_back(r);
      }
    ;

%%

void yy::parser::error(const yy::location& loc, const std::string& m) {
    std::cerr << "[ERRO SINTATICO BISON]: " << m
              << " (linha " << loc.begin.line
              << ", coluna " << loc.begin.column << ")\n";
    parserData.addErro(loc.begin.line, loc.begin.column, m);
}