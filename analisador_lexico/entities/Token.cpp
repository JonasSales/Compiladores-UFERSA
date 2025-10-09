// entities/Token.cpp

#include "Token.h"
#include "../lexer_utils/lex_config.h" // Adicionado para a definição completa de TokenType

int Token::id = 0;

// Construtor padrão adicionado para evitar possíveis problemas
Token::Token() : line(0), column(0), type(TokenType::UNKNOWN), lexeme("") {}

Token::Token(const int line, const int column, const TokenType type, const std::string &lexeme) {
    this->line = line;
    this->column = column;
    this->type = type;
    this->lexeme = lexeme;
}

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