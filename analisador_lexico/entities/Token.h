#ifndef COMPILADORESUFERSA_TOKEN_H
#define COMPILADORESUFERSA_TOKEN_H

#include <string>

enum class TokenType;

class Token {
private:
    static int contador;
    int id;
    int line;
    int column;
    TokenType type;
    std::string lexeme;

public:
    Token();
    Token(int line, int column, TokenType type, const std::string &lexeme);

    [[nodiscard]] int getLine() const;
    void setLine(int line);

    [[nodiscard]] int getColumn() const;
    void setColumn(int column);

    [[nodiscard]] TokenType getTokenType() const;
    void setTokenType(TokenType type);

    std::string getLexeme();
    void setLexeme(std::string lexeme);

    int getContador() const;
};

#endif //COMPILADORESUFERSA_TOKEN_H