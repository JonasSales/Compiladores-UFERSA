#include "TokenAnalisys.h"

void TokenAnalisys::incrementTokenCount(const TokenType type) {
    tokenTypeCount[type]++;
}

void TokenAnalisys::incrementLexemeCount(const string &lexeme) {
    lexemeCount[lexeme]++;
}

void TokenAnalisys::addToken(TokenType type, const string &lexeme) {
    incrementTokenCount(type);
    incrementLexemeCount(lexeme);
    tokenList[type].push_back(lexeme);
}