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
}

%code {
    #include "analisador_lexico/lexer_utils/lex_config.h"
    yy::parser::symbol_type yylex();
}

/* -------------------- TOKENS -------------------- */

%token <std::string> CLASS_NAME RELATION_NAME
%token <std::string> STRING NUMBER
%token <std::string> CLASS_STEREOTYPE RELATION_STEREOTYPE
%token <std::string> NATIVE_DATA_TYPE
%token <std::string> DATATYPE_NAME

/* Palavras reservadas */
%token PACKAGE IMPORT GENSET DISJOINT COMPLETE GENERAL SPECIFICS
%token SPECIALIZES DATATYPE ENUM RELATION

/* Símbolos */
%token LBRACE "{"
%token RBRACE "}"
%token COLON ":"
%token COMMA ","
%token END_OF_FILE 0 "EOF"

/* Tipos de não-terminais */
%type <std::string> package_decl

%%

/* -------------------- REGRAS -------------------- */

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

/* IMPORT */
import_decl:
    IMPORT CLASS_NAME {
        std::cout << "BISON: Importando pacote: " << $2 << std::endl;
    }
    ;

/* PACKAGE */
package_decl:
    PACKAGE CLASS_NAME {
        std::cout << "BISON: Definindo pacote: " << $2 << std::endl;
    }
    ;

class_decl:
    CLASS_STEREOTYPE CLASS_NAME opt_specializes opt_body {
        std::cout << "BISON: Classe encontrada (" << $1 << "): " << $2 << std::endl;
    }
    ;

opt_specializes:
    SPECIALIZES CLASS_NAME
    | /* vazio */
    ;

opt_body:
    "{" body_content "}"
    | /* vazio */
    ;

body_content:
    body_content attribute_decl
    | /* vazio */
    ;

attribute_decl:
    RELATION_NAME ":" type_ref opt_meta
    ;

opt_meta:
    "{" "}"
    | /* vazio */
    ;

type_ref:
    CLASS_NAME
    | NATIVE_DATA_TYPE
    | DATATYPE_NAME
    ;

/* GENSET */
genset_decl:
    opt_disjoint opt_complete GENSET CLASS_NAME "{" GENERAL CLASS_NAME SPECIFICS class_list "}"
    ;

opt_disjoint: DISJOINT | /* vazio */;
opt_complete: COMPLETE | /* vazio */;

class_list:
    class_list "," CLASS_NAME
    | CLASS_NAME
    ;

/* DATATYPE */
datatype_decl:
    DATATYPE CLASS_NAME "{" "}"
    ;

/* ENUM */
enum_decl:
    ENUM CLASS_NAME "{" class_list "}"
    ;

/* RELATION */
relation_decl:
    RELATION_STEREOTYPE RELATION CLASS_NAME
    | RELATION_STEREOTYPE CLASS_NAME
    ;

%%

/* -------------------- ERROS -------------------- */

void yy::parser::error(const std::string& m) {
    std::cerr << "[ERRO SINTATICO BISON]: " << m << std::endl;
}
