#ifndef COMPILADORESUFERSA_LEXER_UTILS_H
#define COMPILADORESUFERSA_LEXER_UTILS_H

class Lexer_Utils {
public:
    static int curr_line;
    static int curr_col;

    static void update_pos(const char* yytext);
};


#endif //COMPILADORESUFERSA_LEXER_UTILS_H