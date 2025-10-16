//
// Created by jonas on 10/15/25.
//

#ifndef COMPILADORESUFERSA_METAATTRIBUTEKEYWORDS_H
#define COMPILADORESUFERSA_METAATTRIBUTEKEYWORDS_H

#include "analisador_lexico/lexer_utils/lex_config.h"
#include "../KeyWordGroup.h"


class MetaAttributeKeywords final : public KeywordGroup {
public:
    void initialize() override {
        keywords = {
            {"ordered", TokenType::META_ATTRIBUTE},
            {"const", TokenType::META_ATTRIBUTE},
            {"derived", TokenType::META_ATTRIBUTE},
            {"subsets", TokenType::META_ATTRIBUTE},
            {"redefines", TokenType::META_ATTRIBUTE}
        };
    }
};

#endif //COMPILADORESUFERSA_METAATTRIBUTEKEYWORDS_H