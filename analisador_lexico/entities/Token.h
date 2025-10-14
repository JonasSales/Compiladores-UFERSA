#ifndef COMPILADORESUFERSA_TOKEN_H
#define COMPILADORESUFERSA_TOKEN_H

#include <string>

using std::string;

enum class TokenType;

class Token {
private:
    static int contador;
    int id;
    int line;
    int column;
    TokenType type;
    string lexeme;

public:
    Token();
    Token(int line, int column, TokenType type, const string &lexeme);

    [[nodiscard]] int getLine() const;
    void setLine(int line);

    [[nodiscard]] int getColumn() const;
    void setColumn(int column);

    [[nodiscard]] TokenType getTokenType() const;
    void setTokenType(TokenType type);

    [[nodiscard]] string getLexeme() const;
    void setLexeme(string lexeme);

    [[nodiscard]] int getContador() const;
};

#endif //COMPILADORESUFERSA_TOKEN_H