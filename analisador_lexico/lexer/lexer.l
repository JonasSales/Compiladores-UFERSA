%option noyywrap c++ yylineno

%{
#include "analisador_lexico/entities/Token.h"
#include "analisador_lexico/lexer_utils/lex_config.h"
#include "analisador_lexico/lexer_utils/KeyWordGroup/KeyWordManager/KeyWordManager.h"

KeyWordManager keywords;
int column = 1;
%}

/* Expressões Regulares para as convenções da linguagem TONTO */
DELIM           [ \t\r]
WHITE_SPACES    {DELIM}+
UPPERCASE       [A-Z]
LOWERCASE       [a-z]
LETTER          [a-zA-Z]
DIGIT           [0-9]
UNDERLINE       "_"
SPECIALSYMBOLS  [{}()]

CLASS_NAME      {UPPERCASE}({LETTER}|{UNDERLINE})*
RELATION_NAME   ("@"|{LOWERCASE})({LETTER}|{UNDERLINE}|-)*
INSTANCE_NAME   {LETTER}({LETTER}|{DIGIT}|{UNDERLINE})*{DIGIT}+
DATATYPE_NAME   {LETTER}+"DataType"

CPF             {DIGIT}{3}\.{DIGIT}{3}\.{DIGIT}{3}-{DIGIT}{2}
PHONENUMBER     (\({DIGIT}{2}\)[ ]?)?9?{DIGIT}{4}-{DIGIT}{4}

CARDINALITY     "["{NUMBER}(".."({NUMBER}|"*"))?"]"

NUMBER          {DIGIT}+(\.{DIGIT}+)?
STRING          \"([^\\\"]|\\.)*\"
COMMENT_LINE    "//".*
COMMENT_BLOCK   "/*"([^*]|(\*+[^*/]))*\*+"/"
%%

{COMMENT_LINE}  |
{COMMENT_BLOCK} { 
    for (int i = 0; yytext[i] != '\0'; i++) {
            if (yytext[i] == '\n') {
                column = 1;
            } else {
                column++;
            }
        }
}

{CARDINALITY} {
Token token = {yylineno, column, TokenType::CARDINALITY, yytext};
    tokens.emplace(token.getContador(), token);
    column+= yyleng;
}

{DATATYPE_NAME} {
    Token token = {yylineno, column, TokenType::DATATYPE_NAME, yytext};
    tokens.emplace(token.getContador(), token);
    column+= yyleng;
}

{INSTANCE_NAME} {
    Token token = {yylineno, column, TokenType::INSTANCE_NAME, yytext};
    tokens.emplace(token.getContador(), token);
    column+= yyleng;
}

{CLASS_NAME} {
    Token token = {yylineno, column, TokenType::CLASS_NAME, yytext};
    tokens.emplace(token.getContador(), token);
    column+= yyleng;
}

{RELATION_NAME} {
    TokenType type = keywords.getType(yytext);
    Token token = {yylineno, column, type, yytext};
    tokens.emplace(token.getContador(), token);
    column+= yyleng;
}

{LETTER}({LETTER}|{DIGIT}|{UNDERLINE})* {
    fprintf(stderr, "[ERRO LÉXICO] Linha %d, Coluna %d: Identificador mal formado: '%s'.\n", yylineno, column, yytext);
    Token token = {yylineno, column, TokenType::UNKNOWN, yytext};
    tokens.emplace(token.getContador(), token);
    column+= yyleng;
}

{NUMBER} {
    Token token = {yylineno, column, TokenType::NUMBER, yytext};
    tokens.emplace(token.getContador(), token);
    column+= yyleng;
}

{STRING} {
    Token token = {yylineno, column, TokenType::STRING, yytext};
    tokens.emplace(token.getContador(), token);
    column+= yyleng;
}

"<>--"|"--<>"|"<>"|".."|"--"|"..."|"<o>--" {
    Token token = {yylineno, column, TokenType::RELATION, yytext};
    tokens.emplace(token.getContador(), token);
    column+= yyleng;
}

{CPF} {
    Token token = {yylineno, column, TokenType::NUMBER, yytext};
    tokens.emplace(token.getContador(), token);
    column+= yyleng;
}

{PHONENUMBER} {
    Token token = {yylineno, column, TokenType::NUMBER, yytext};
    tokens.emplace(token.getContador(), token);
    column+= yyleng;
}

{SPECIALSYMBOLS} {
    Token token = {yylineno, column, TokenType::SYMBOL, yytext};
    tokens.emplace(token.getContador(), token);
    column+= yyleng;
}

[,@:*.-] {
    Token token = {yylineno, column, TokenType::SYMBOL, yytext};
    tokens.emplace(token.getContador(), token);
    column+= yyleng;
}

\n {
    column = 1;
}

{DELIM} {
    column+= yyleng;;
}

. {
    fprintf(stderr, "[ERRO LÉXICO] Linha %d, Coluna %d: Caractere desconhecido: '%s'.\n", yylineno, column, yytext);
    Token token = {yylineno, column, TokenType::UNKNOWN, yytext};
    tokens.emplace(token.getContador(), token);
    column+= yyleng;
}
%%
