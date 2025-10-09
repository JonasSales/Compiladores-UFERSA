#include "Token.h"
#include "../lexer_utils/lex_config.h" // Adicionado para a definição completa de TokenType

int Token::contador = 1;

Token::Token() : id(contador++), line(0), column(0), type(TokenType::UNKNOWN), lexeme("") {}

Token::Token(const int line, const int column, const TokenType type, const std::string &lexeme)
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

std::string Token::getLexeme() {
    return lexeme;
}

void Token::setLexeme(std::string lexeme) {
    this->lexeme = lexeme;
}

int Token::getContador() const {
    return id;
}