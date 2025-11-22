#include "lex_config.h"
#include "../entities/Token.h"
#include "analisador_lexico/entities/TokenAnalisys.h"
#include "../../analisador_sintatico/Parser.h" //

using namespace std;

// Definição das variáveis globais (aqui elas são criadas na memória)
map<int, Token> tokens;
TokenAnalisys tokenAnalisys;

// Definição das variáveis que estavam faltando (e causando o erro do linker)
Parser parserData;
std::map<int, Token>::iterator currentTokenIt;
std::map<int, Token>::iterator endTokenIt;

string tokenToString(const TokenType type) {
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
        case TokenType::CARDINALITY:          return "CARDINALITY";
        case TokenType::RELATION:             return "RELATION";
        default:                              return "EOF";
    }
}