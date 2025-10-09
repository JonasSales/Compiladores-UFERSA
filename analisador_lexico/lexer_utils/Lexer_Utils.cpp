//
// Created by guilherme on 09/10/2025.
//

#include "Lexer_Utils.h"

int Lexer_Utils::curr_line = 1;
int Lexer_Utils::curr_col = 1;

void Lexer_Utils::update_pos(const char* yytext) {
    for (int i = 0; yytext[i] != '\0'; i++) {
        if (yytext[i] == '\n') {
            curr_line++;
            curr_col = 1;
        } else {
            curr_col++;
        }
    }
}