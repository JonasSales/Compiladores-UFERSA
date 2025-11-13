#include "Token.h"
#include <utility>
#include "../lexer_utils/lex_config.h"

int Token::contador = 1;

Token::Token() {
    this->id = 0;
    this->line = 0;
    this->column = 0;
    this->type = TokenType::UNKNOWN;
}

Token::Token(const int line, const int column, const TokenType type, const string &lexeme)
    : id(contador++), line(line), column(column), type(type), lexeme(lexeme) {}

int Token::getLine() const {
    return line;
}

void Token::setLine(int line) {
    this->line = line;
}

int Token::getColumn() const {
    return column;
}

void Token::setColumn(int column) {
    this->column = column;
}

TokenType Token::getTokenType() const {
    return type;
}

void Token::setTokenType(TokenType type) {
    this->type = type;
}

std::string Token::getLexeme() const {
    return lexeme;
}

void Token::setLexeme(std::string lexeme) {
    this->lexeme = std::move(lexeme);
}

int Token::getContador() const {
    return id;
}