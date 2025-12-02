//
// Created by jonas on 10/15/25.
//

#ifndef COMPILADORESUFERSA_RESERVEDWORDKEYWORDS_H
#define COMPILADORESUFERSA_RESERVEDWORDKEYWORDS_H

#include "KeyWordGroup.h"

class ReservedWordKeywords final : public KeywordGroup {
public:
    void initialize() override {
        keywords = {
            {"genset", TokenType::RESERVED_WORD},
            {"disjoint", TokenType::RESERVED_WORD},
            {"complete", TokenType::RESERVED_WORD},
            {"general", TokenType::RESERVED_WORD},
            {"specifics", TokenType::RESERVED_WORD},
            {"where", TokenType::RESERVED_WORD},
            {"package", TokenType::RESERVED_WORD},
            {"import", TokenType::RESERVED_WORD},
            {"functional-complexes", TokenType::RESERVED_WORD},
            {"intrinsic-modes", TokenType::RESERVED_WORD},
            {"disjoint_complete", TokenType::RESERVED_WORD},
            {"has", TokenType::RESERVED_WORD},
            {"specializes", TokenType::RESERVED_WORD},
            {"datatype", TokenType::RESERVED_WORD}, // [cite: 61]
            {"enum", TokenType::RESERVED_WORD},     // [cite: 69]
            {"relation", TokenType::RESERVED_WORD},
            {"of", TokenType::RESERVED_WORD}// [cite: 100]
        };
    }
};

#endif //COMPILADORESUFERSA_RESERVEDWORDKEYWORDS_H