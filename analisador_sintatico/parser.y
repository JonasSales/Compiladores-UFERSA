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

/* --- Tokens com Valor (Strings) --- */
%token <std::string> CLASS_NAME RELATION_NAME INSTANCE_NAME DATATYPE_NAME
%token <std::string> STRING NUMBER
%token <std::string> CLASS_STEREOTYPE RELATION_STEREOTYPE
%token <std::string> NATIVE_DATA_TYPE
%token <std::string> CARDINALITY
%token <std::string> RELATION_OP  /* Token para operadores como "--", ".." */

/* --- Tokens Vazios (Keywords e Símbolos) --- */
/* RELATION volta a ser keyword aqui para casar com main.cpp */
%token DISJOINT COMPLETE
%token PACKAGE IMPORT GENSET
%token GENERAL SPECIFICS
%token SPECIALIZES DATATYPE ENUM RELATION
%token LBRACE "{" RBRACE "}" COLON ":" COMMA "," END_OF_FILE 0 "EOF"
%token META_ATTRIBUTE_START "meta{" META_ATTRIBUTE_END "meta}"

/* --- Tipos de Retorno (Não alterados) --- */
%type <SinteseClasse> class_decl
%type <std::vector<SinteseAtributo>> opt_body body_content
%type <SinteseAtributo> attribute_decl inner_relation_decl
%type <std::string> type_ref
%type <std::string> opt_specializes identifier opt_nature opt_cardinality
%type <std::string> opt_disjoint_complete specialization_list
%type <std::vector<std::string>> class_list

%%

/* ---------------- Programa Principal ---------------- */
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
    | class_decl
    | genset_decl
    | datatype_decl
    | enum_decl
    | relation_decl
    | error
    ;

/* --- Identificadores --- */
identifier:
    CLASS_NAME { $$ = $1; }
    | INSTANCE_NAME { $$ = $1; }
    | RELATION_NAME { $$ = $1; }
    ;

/* --- Package & Import --- */
package_decl:
    PACKAGE identifier {
        parserData.definirPacote($2);
    }
    ;

import_decl:
    IMPORT identifier {
        parserData.addImport($2);
    }
    ;

/* --- Classe / Relator --- */
class_decl:
    CLASS_STEREOTYPE identifier opt_nature opt_specializes opt_body {
        SinteseClasse novaClasse;
        novaClasse.pacote = parserData.pacoteAtual;
        novaClasse.estereotipo = $1;
        novaClasse.nome = $2;
        novaClasse.herdaDe = $4;
        novaClasse.atributos = $5;

        novaClasse.linha = @2.begin.line;
        novaClasse.coluna = @2.begin.column;
        novaClasse.arquivo = parserData.arquivoAtual;

        string key = Parser::makeKey(novaClasse.pacote, novaClasse.nome);
        parserData.classesEncontradas[key] = novaClasse;

        $$ = novaClasse;
    }
    ;

opt_nature:
    identifier identifier {
        $$ = $1 + " " + $2;
    }
    | /* vazio */ { $$ = ""; }
    ;

specialization_list:
      identifier { $$ = $1; }
    | specialization_list "," identifier {
          $$ = $1 + ", " + $3;
    }
    ;

opt_specializes:
    SPECIALIZES specialization_list { $$ = $2; }
    | /* vazio */ { $$ = ""; }
    ;

/* --- Corpo da Classe --- */
opt_body:
    "{" body_content "}" { $$ = $2;
    }
    | /* vazio */ { $$ = std::vector<SinteseAtributo>(); }
    ;

body_content:
      body_content attribute_decl {
          $1.push_back($2);
          $$ = $1;
      }
    | body_content inner_relation_decl {
          $1.push_back($2);
          $$ = $1;
      }
    | body_content error {
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
    | error {
          $$ = std::vector<SinteseAtributo>();
    }
    | /* vazio */ {
          $$ = std::vector<SinteseAtributo>();
    }
    ;

/* Cardinalidade */
opt_cardinality:
    CARDINALITY { $$ = $1;
    }
    | /* vazio */ { $$ = ""; }
    ;

/* Declaração de Atributo Simples */
attribute_decl:
    RELATION_NAME ":" type_ref opt_cardinality {
        SinteseAtributo attr;
        attr.nome = $1;
        attr.tipo = $3;
        $$ = attr;
    }
    ;

/* Declaração de Relação Interna (dentro da classe) */
inner_relation_decl:
    /* Caso 1: @stereo -- nome -- [card] Tipo */
      RELATION_STEREOTYPE RELATION_OP RELATION_NAME RELATION_OP CARDINALITY type_ref {
        SinteseAtributo attr;
        attr.nome = $3;
        attr.tipo = $6;
        attr.metaAtributo = $1;
        $$ = attr;
    }
    /* Caso 2: -- nome -- [card] Tipo (sem estereótipo) */
    | RELATION_OP RELATION_NAME RELATION_OP CARDINALITY type_ref {
        SinteseAtributo attr;
        attr.nome = $2;
        attr.tipo = $5;
        attr.metaAtributo = "";
        $$ = attr;
    }
    /* Caso 3 (NOVO): @stereo [card] -- [card] Tipo (sem nome explícito) */
    | RELATION_STEREOTYPE CARDINALITY RELATION_OP CARDINALITY type_ref {
        SinteseAtributo attr;
        attr.nome = "";
        attr.tipo = $5;
        attr.metaAtributo = $1;
        $$ = attr;
    }
    ;

type_ref:
    identifier { $$ = $1; }
    | NATIVE_DATA_TYPE { $$ = $1; }
    | DATATYPE_NAME { $$ = $1; }
    ;

/* --- Genset (Generalization Set) --- */
opt_disjoint_complete:
      /* vazio */ { $$ = "";
    }
    | DISJOINT { $$ = "disjoint"; }
    | COMPLETE { $$ = "complete";
    }
    | DISJOINT COMPLETE { $$ = "disjoint complete"; }
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

        string currentPkg = parserData.pacoteAtual;

        std::string flags = $1;
        gs.isDisjoint = (flags.find("disjoint") != std::string::npos);
        gs.isComplete = (flags.find("complete") != std::string::npos);

        gs.linha = @3.begin.line;
        gs.coluna = @3.begin.column;
        gs.arquivo = parserData.arquivoAtual;

        string key = Parser::makeKey(currentPkg, gs.nome);
        parserData.gensetsEncontrados[key] = gs;
    }
    ;

datatype_decl:
    DATATYPE identifier "{" "}" {
        SinteseDatatype dt;
        dt.nome = $2; dt.pacote = parserData.pacoteAtual;

        dt.linha = @2.begin.line;
        dt.coluna = @2.begin.column;
        dt.arquivo = parserData.arquivoAtual;

        string key = Parser::makeKey(dt.pacote, dt.nome);
        parserData.datatypesEncontrados[key] = dt;
    }
    ;

enum_decl:
    ENUM identifier "{" class_list "}" {
        SinteseEnum en;
        en.nome = $2;
        en.pacote = parserData.pacoteAtual;
        en.instancias = $4;

        string key = Parser::makeKey(en.pacote, en.nome);
        parserData.enumsEncontrados[key] = en;
    }
    ;

/* Declaração de Relação Externa (fora da classe) */
relation_decl:
      /* Caso 1: Completa */
      RELATION_STEREOTYPE RELATION identifier CARDINALITY RELATION_OP identifier RELATION_OP CARDINALITY identifier {
          SinteseRelacao r;
          r.estereotipo = $1;
          r.dominio = $3;
          r.cardinalidadeDominio = $4;
          r.simboloRelacao = $5;
          r.nome = $6;
          r.cardinalidadeImagem = $8;
          r.imagem = $9;

          r.linha = @6.begin.line;
          r.coluna = @6.begin.column;
          r.arquivo = parserData.arquivoAtual;

          parserData.relacoesExternasEncontradas.push_back(r);
      }
      /* Caso 2: Simplificada */
    | RELATION_STEREOTYPE RELATION identifier {
          SinteseRelacao r;
          r.estereotipo = $1;
          r.nome = $3;

          r.linha = @3.begin.line;
          r.coluna = @3.begin.column;
          r.arquivo = parserData.arquivoAtual;

          parserData.relacoesExternasEncontradas.push_back(r);
      }
    | RELATION_STEREOTYPE identifier {
          SinteseRelacao r;
          r.estereotipo = $1;
          r.nome = $2;

          r.linha = @2.begin.line;
          r.coluna = @2.begin.column;
          r.arquivo = parserData.arquivoAtual;

          parserData.relacoesExternasEncontradas.push_back(r);
      }
    ;

%%

void yy::parser::error(const yy::location& loc, const std::string& m) {
    std::string mensagem = m;
    std::string alvo = "unexpected ";
    size_t pos = mensagem.find(alvo);
    if (pos != std::string::npos) {
        size_t inicioToken = pos + alvo.length();
        size_t fimToken = mensagem.find(',', inicioToken);
        if (fimToken == std::string::npos) fimToken = mensagem.length();
        mensagem.replace(inicioToken, fimToken - inicioToken, "\"" + parserData.ultimoLexema + "\"");
    }

    std::cerr << "\033[1;31m" << "syntax error (linha " << loc.begin.line
              << ", coluna " << loc.begin.column << "): " << mensagem << "\033[0m" << "\n";
    parserData.addErro(loc.begin.line, loc.begin.column, mensagem);
}