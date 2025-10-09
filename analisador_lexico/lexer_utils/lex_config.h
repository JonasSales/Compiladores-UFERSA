#ifndef COMPILADORESUFERSA_LEX_CONFIG_H
#define COMPILADORESUFERSA_LEX_CONFIG_H
#include <format>
#include <string>
#include <vector>
#include "../entities/Token.h"

class Token;

extern std::vector<Token> tokens;

enum class TokenType {
    CLASS_NAME, RELATION_NAME, INSTANCE_NAME, DATATYPE_NAME,
    CLASS_STEREOTYPE, RELATION_STEREOTYPE,
    RESERVED_WORD, NATIVE_DATA_TYPE, META_ATTRIBUTE,
    NUMBER, STRING, SYMBOL,
    UNKNOWN, END_OF_FILE
};

std::string token_to_string(TokenType type);

#endif //COMPILADORESUFERSA_LEX_CONFIG_H