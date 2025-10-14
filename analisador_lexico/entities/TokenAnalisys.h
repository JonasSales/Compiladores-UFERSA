#ifndef COMPILADORESUFERSA_TOKENANALISYS_H
#define COMPILADORESUFERSA_TOKENANALISYS_H
#include <string>
#include <map>
#include <set>
#include <vector>
#include "analisador_lexico/lexer_utils/lex_config.h"

using std::map;
using std::set;
using std::string;
using std::vector;

class TokenAnalisys {
private:
    void incrementTokenCount(TokenType type);
    void incrementLexemeCount(const string &lexeme);
public:
    map<TokenType, int> tokenTypeCount;
    map<string, int> lexemeCount;
    map<TokenType, vector<string>> tokenList;

    void addToken(TokenType type, const string& lexeme);
};

#endif //COMPILADORESUFERSA_TOKENANALISYS_H