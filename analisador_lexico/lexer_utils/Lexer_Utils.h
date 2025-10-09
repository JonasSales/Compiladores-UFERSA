//
// Created by guilherme on 09/10/2025.
//

#ifndef COMPILADORESUFERSA_LEXER_UTILS_H
#define COMPILADORESUFERSA_LEXER_UTILS_H

#include <string>

class Lexer_Utils {
public:
    static int curr_line;
    static int curr_col;

    static void update_pos(const char* yytext);
};


#endif //COMPILADORESUFERSA_LEXER_UTILS_H