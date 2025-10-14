#ifndef COMPILADORESUFERSA_LEX_CONFIG_H
#define COMPILADORESUFERSA_LEX_CONFIG_H
#include <format>
#include <string>
#include "map"
#include "../entities/Token.h"

class TokenAnalisys;
using std::map;
using std::string;

extern map<int, Token> tokens;
extern TokenAnalisys tokenAnalisys;

enum class TokenType {
    CLASS_NAME, RELATION_NAME, INSTANCE_NAME, DATATYPE_NAME,
    CLASS_STEREOTYPE, RELATION_STEREOTYPE,
    RESERVED_WORD, NATIVE_DATA_TYPE, META_ATTRIBUTE,
    NUMBER, STRING, SYMBOL,
    UNKNOWN, END_OF_FILE
};

string tokenToString(TokenType type);

#endif //COMPILADORESUFERSA_LEX_CONFIG_H