// lexer_utils/lex_config.cpp

#include "lex_config.h"
#include "../entities/Token.h" // Inclua Token.h aqui

// Definição do vetor global de tokens
std::vector<Token> tokens;

std::string token_to_string(const TokenType type) {
    switch (type) {
        case TokenType::CLASS_NAME:           return "CLASS_NAME";
        case TokenType::RELATION_NAME:        return "RELATION_NAME";
        case TokenType::INSTANCE_NAME:        return "INSTANCE_NAME";
        case TokenType::DATATYPE_NAME:        return "DATATYPE_NAME";
        case TokenType::CLASS_STEREOTYPE:     return "CLASS_STEREOTYPE";
        case TokenType::RELATION_STEREOTYPE:  return "RELATION_STEREOTYPE";
        case TokenType::RESERVED_WORD:        return "RESERVED_WORD";
        case TokenType::NATIVE_DATA_TYPE:     return "NATIVE_DATA_TYPE";
        case TokenType::META_ATTRIBUTE:       return "META_ATTRIBUTE";
        case TokenType::NUMBER:               return "NUMBER";
        case TokenType::STRING:               return "STRING";
        case TokenType::SYMBOL:               return "SYMBOL";
        case TokenType::UNKNOWN:              return "UNKNOWN";
        default:                              return "EOF";
    }
}