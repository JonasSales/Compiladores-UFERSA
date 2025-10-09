// entities/Token.h

#ifndef COMPILADORESUFERSA_TOKEN_H
#define COMPILADORESUFERSA_TOKEN_H

#include <string>

enum class TokenType;

class Token {
private:
    static int id;
    int line;
    int column;
    TokenType type;
    std::string lexeme; // Usar std::string

public:
    Token();
    Token(int line, int column, TokenType type, const std::string &lexeme); // Usar std::string

    [[nodiscard]] int getLine() const;
    void setLine(int line);

    [[nodiscard]] int getColumn() const;
    void setColumn(int column);

    [[nodiscard]] TokenType getTokenType() const;
    void setTokenType(TokenType type);

    std::string getLexeme(); // Usar std::string
    void setLexeme(std::string lexeme); // Usar std::string

    int getContador() const;
};

#endif //COMPILADORESUFERSA_TOKEN_H