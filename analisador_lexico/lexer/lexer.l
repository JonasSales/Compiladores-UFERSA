%option noyywrap c++

%{
#include "analisador_lexico/entities/Token.h"
#include "analisador_lexico/lexer_utils/lex_config.h"
#include "analisador_lexico/lexer_utils/KeywordManager.h"
#include "analisador_lexico/lexer_utils/Lexer_Utils.h"

KeywordManager keywords;

%}

/* Expressões Regulares para as convenções da linguagem TONTO */
DELIM           [ \t\n]
WHITE_SPACES    {DELIM}+
UPPERCASE       [A-Z]
LOWERCASE       [a-z]
LETTER          [a-zA-Z]
DIGIT           [0-9]
UNDERLINE       "_"

CLASS_NAME      {UPPERCASE}({LETTER}|{UNDERLINE})*
RELATION_NAME   {LOWERCASE}({LETTER}|{UNDERLINE})*
INSTANCE_NAME   {LETTER}({LETTER}|{DIGIT}|{UNDERLINE})*{DIGIT}+
DATATYPE_NAME   {LETTER}+"DataType"

NUMBER          {DIGIT}+(\.{DIGIT}+)?
STRING          \"([^\\\"]|\\.)*\"
COMMENT_LINE    "//".*
COMMENT_BLOCK   "/*"([^*]|(\*+[^*/]))*\*+"/"
%%

{COMMENT_LINE}  |
{COMMENT_BLOCK} { Lexer_Utils::update_pos(yytext); }

{DATATYPE_NAME} {
    Token token = {Lexer_Utils::curr_line, Lexer_Utils::curr_col, TokenType::DATATYPE_NAME, yytext};
    Lexer_Utils::update_pos(yytext);
    tokens.push_back(token);
}

{INSTANCE_NAME} {
    Token token = {Lexer_Utils::curr_line, Lexer_Utils::curr_col, TokenType::INSTANCE_NAME, yytext};
    Lexer_Utils::update_pos(yytext);
    tokens.push_back(token);
}

{CLASS_NAME} {
    Token token = {Lexer_Utils::curr_line, Lexer_Utils::curr_col, TokenType::CLASS_NAME, yytext};
    Lexer_Utils::update_pos(yytext);
    tokens.push_back(token);
}

{RELATION_NAME} {
    TokenType type = keywords.getType(yytext);
    Token token = {Lexer_Utils::curr_line, Lexer_Utils::curr_col, type, yytext};
    Lexer_Utils::update_pos(yytext);
    tokens.push_back(token);
}

{NUMBER} {
    Token token = {Lexer_Utils::curr_line, Lexer_Utils::curr_col, TokenType::NUMBER, yytext};
    Lexer_Utils::update_pos(yytext);
    tokens.push_back(token);
}

{STRING} {
    Token token = {Lexer_Utils::curr_line, Lexer_Utils::curr_col, TokenType::STRING, yytext};
    Lexer_Utils::update_pos(yytext);
    tokens.push_back(token);
}

"<>--"|"--<>"|"<>"|".."|"--"|"..." {
    Token token = {Lexer_Utils::curr_line, Lexer_Utils::curr_col, TokenType::SYMBOL, yytext};
    Lexer_Utils::update_pos(yytext);
    tokens.push_back(token);
}

[{}()\[\],@:*.] {
    Token token = {Lexer_Utils::curr_line, Lexer_Utils::curr_col, TokenType::SYMBOL, yytext};
    Lexer_Utils::update_pos(yytext);
    tokens.push_back(token);
}

[ \t\r\n]+ { Lexer_Utils::update_pos(yytext); }

. {
    fprintf(stderr, "[ERRO LÉXICO] Linha %d, Coluna %d: Caractere desconhecido: '%s'.\n",
            Lexer_Utils::curr_line, Lexer_Utils::curr_col, yytext);
    Token token = {Lexer_Utils::curr_line, Lexer_Utils::curr_col, TokenType::UNKNOWN, yytext};
    Lexer_Utils::update_pos(yytext);
    tokens.push_back(token);
}
%%