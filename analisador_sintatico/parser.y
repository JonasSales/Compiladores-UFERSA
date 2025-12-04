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

/* --- Tokens --- */
%token <std::string> CLASS_NAME RELATION_NAME INSTANCE_NAME DATATYPE_NAME
%token <std::string> STRING NUMBER
%token <std::string> CLASS_STEREOTYPE RELATION_STEREOTYPE
%token <std::string> NATIVE_DATA_TYPE
%token <std::string> CARDINALITY
%token <std::string> RELATION_OP

/* Tokens Específicos para Controle de Ordem */
%token DISJOINT COMPLETE

%token PACKAGE IMPORT GENSET
%token GENERAL SPECIFICS
%token SPECIALIZES DATATYPE ENUM RELATION
%token LBRACE "{" RBRACE "}" COLON ":" COMMA "," END_OF_FILE 0 "EOF"
%token META_ATTRIBUTE_START "meta{" META_ATTRIBUTE_END "meta}"

/* --- Tipos de Retorno --- */
%type <SinteseClasse> class_decl
%type <std::vector<SinteseAtributo>> opt_body body_content
%type <SinteseAtributo> attribute_decl inner_relation_decl
%type <std::string> type_ref opt_specializes identifier opt_nature opt_cardinality
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

        string key = Parser::makeKey(novaClasse.pacote, novaClasse.nome);
        parserData.classesEncontradas[key] = novaClasse;

        $$ = novaClasse;
    }
    ;

opt_nature:
    identifier identifier {
        /* Assume que o primeiro ID é "of" e o segundo é a natureza */
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
    "{" body_content "}" { $$ = $2; }
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

/* Cardinalidade */
opt_cardinality:
    CARDINALITY { $$ = $1; }
    | /* vazio */ { $$ = ""; }
    ;

/* opt_cardinality */
attribute_decl:
    RELATION_NAME ":" type_ref opt_cardinality {
        SinteseAtributo attr;
        attr.nome = $1;
        attr.tipo = $3;
        // attr.cardinalidade = $4; // Descomente se SinteseAtributo tiver este campo
        $$ = attr;
    }
    ;

inner_relation_decl:
    /* Com estereótipo (Ex: @mediation -- nome -- [1] Tipo) */
    RELATION_STEREOTYPE RELATION_OP RELATION_NAME RELATION_OP CARDINALITY type_ref {
        SinteseAtributo attr;
        attr.nome = $3;
        attr.tipo = $6;
        attr.metaAtributo = $1;
        $$ = attr;
    }
    /* Sem estereótipo (Ex: -- nome -- [1] Tipo) */
    | RELATION_OP RELATION_NAME RELATION_OP CARDINALITY type_ref {
        SinteseAtributo attr;
        attr.nome = $2;
        attr.tipo = $5;
        attr.metaAtributo = "";
        $$ = attr;
    }
    | RELATION_STEREOTYPE CARDINALITY RELATION_OP CARDINALITY type_ref {
            SinteseAtributo attr;
            attr.nome = ""; // Não há nome explícito nesta sintaxe
            attr.tipo = $5;
            attr.metaAtributo = $1;
            // Se quiser salvar as cardinalidades, precisaria adicionar campos na struct SinteseAtributo
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
      /* vazio */ { $$ = ""; }
    | DISJOINT { $$ = "disjoint"; }
    | COMPLETE { $$ = "complete"; }
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

        string key = Parser::makeKey(currentPkg, gs.nome);
        parserData.gensetsEncontrados[key] = gs;    }
    ;

datatype_decl:
    DATATYPE identifier "{" "}" {
        SinteseDatatype dt;
        dt.nome = $2; dt.pacote = parserData.pacoteAtual;

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

relation_decl:
      /* Caso 1: Declaração Completa com Cardinalidades e Source/Target */
      RELATION_STEREOTYPE RELATION identifier CARDINALITY RELATION_OP identifier RELATION_OP CARDINALITY identifier {
          SinteseRelacao r;
          r.estereotipo = $1;
          r.dominio = $3;              // $3 é o identificador da classe de origem (Source)
          r.cardinalidadeDominio = $4; // $4 é a cardinalidade da origem (ex: [1..*])
          r.simboloRelacao = $5;       // $5 é o operador (ex: --)
          r.nome = $6;                 // $6 é o nome da relação
          // $7 é o segundo operador (ignoramos ou assumimos simetria)
          r.cardinalidadeImagem = $8;  // $8 é a cardinalidade do destino
          r.imagem = $9;               // $9 é o identificador da classe de destino (Target)

          parserData.relacoesExternasEncontradas.push_back(r);
      }

      /* Caso 2: Declaração Simplificada (apenas nome e estereótipo) */
    | RELATION_STEREOTYPE RELATION identifier {
          SinteseRelacao r;
          r.estereotipo = $1;
          r.nome = $3;
          // Outros campos ficam vazios por padrão
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
    std::string mensagem = m;
    // 1. Substitui o nome técnico do token pelo lexema real
    std::string alvo = "unexpected ";
    size_t pos = mensagem.find(alvo);

    if (pos != std::string::npos) {
        size_t inicioToken = pos + alvo.length();
        size_t fimToken = mensagem.find(',', inicioToken);
        if (fimToken == std::string::npos) {
            fimToken = mensagem.length();
        }
        mensagem.replace(inicioToken, fimToken - inicioToken, "\"" + parserData.ultimoLexema + "\"");
    }

    std::string prefixo = "syntax error";
    std::string corpoMensagem;

    if (mensagem.find(prefixo) == 0) {
        corpoMensagem = mensagem.substr(prefixo.length());
        if (corpoMensagem.find(", ") == 0) {
            corpoMensagem = corpoMensagem.substr(2);
        }
    } else {
        corpoMensagem = mensagem;
    }

    std::cerr << "\039[1;31m"
              << "syntax error (linha " << loc.begin.line
              << ", coluna " << loc.begin.column << "): "
              << corpoMensagem
              << "\033[0m" << "\n";
    parserData.addErro(loc.begin.line, loc.begin.column, mensagem);
}