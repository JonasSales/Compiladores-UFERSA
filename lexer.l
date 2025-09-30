%option noyywrap c++
%option case-insensitive

%{
#include <iostream>
#include <string>
using namespace std;

// linha e coluna globais
int curr_line = 1;
int curr_col  = 1;

// função auxiliar para atualizar colunas
void update_pos(const char* yytext) {
    for (int i = 0; yytext[i] != '\0'; i++) {
        if (yytext[i] == '\n') { curr_line++; curr_col = 1; }
        else curr_col++;
    }
}

// keywords
static const char* KEYWORDS[] = {
    "package","import","class","kind","collective","quantity","quality","mode",
    "intrinsicMode","extrinsicMode","relator","type","powertype","subkind","phase",
    "role","historicalRole","event","situation","category","mixin","phaseMixin",
    "roleMixin","historicalRoleMixin","genset","where","specializes","ordered","const",
    "derived","relation","instanceOf","as","enum","datatype","name"
};

bool is_keyword(const char* s) {
    for (auto kw : KEYWORDS) {
        if (strcasecmp(s, kw) == 0) return true;
    }
    return false;
}
%}

/* expressões regulares */

DIGIT       [0-9]
ID          [a-zA-Z_][a-zA-Z0-9_\.]*
STRING      \"([^\\\"]|\\.)*\"
COMMENT1    "//".*


%%

{COMMENT1}   { update_pos(yytext); /* ignora comentário */ }
"/*"([^*]*\*+([^/*][^*]*\*+)*)*"/"   { update_pos(yytext); /* ignora comentário */ }

{DIGIT}+(\.{DIGIT}+)?   { update_pos(yytext); cout << curr_line << "," << curr_col << ",NUMBER," << yytext << endl; }
{ID}                    {
                           update_pos(yytext);
                           if (is_keyword(yytext))
                               cout << curr_line << "," << curr_col << ",KEYWORD," << yytext << endl;
                           else
                               cout << curr_line << "," << curr_col << ",IDENT," << yytext << endl;
                        }

{STRING}     { update_pos(yytext); cout << curr_line << "," << curr_col << ",STRING," << yytext << endl; }

"<o>--"      { update_pos(yytext); cout << curr_line << "," << curr_col << ",OP,<o>--" << endl; }
"<>--"       { update_pos(yytext); cout << curr_line << "," << curr_col << ",OP,<>--" << endl; }
"--"         { update_pos(yytext); cout << curr_line << "," << curr_col << ",OP,--" << endl; }

"="          { update_pos(yytext); cout << curr_line << "," << curr_col << ",ASSIGN,=" << endl; }
"+"          { update_pos(yytext); cout << curr_line << "," << curr_col << ",PLUS,+" << endl; }
"-"          { update_pos(yytext); cout << curr_line << "," << curr_col << ",MINUS,-" << endl; }
"*"          { update_pos(yytext); cout << curr_line << "," << curr_col << ",MULT,*" << endl; }
"/"          { update_pos(yytext); cout << curr_line << "," << curr_col << ",DIV,/" << endl; }

[{}()\[\],:\.]   { update_pos(yytext); cout << curr_line << "," << curr_col << ",SYMBOL," << yytext << endl; }

"@"{ID}      { update_pos(yytext); cout << curr_line << "," << curr_col << ",IDENT," << yytext << endl; }

[ \t\r\n]+   { update_pos(yytext); /* ignora espaços */ }

.            { update_pos(yytext); cout << curr_line << "," << curr_col << ",UNK," << yytext << endl; }

%%

int yywrap() { return 1; }
