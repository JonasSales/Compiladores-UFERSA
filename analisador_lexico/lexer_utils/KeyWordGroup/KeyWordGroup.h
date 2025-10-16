#ifndef COMPILADORESUFERSA_KEYWORDGROUP_H
#define COMPILADORESUFERSA_KEYWORDGROUP_H

#include <string>
#include <unordered_map>
#include "analisador_lexico/lexer_utils/lex_config.h"

using namespace std;

class KeywordGroup {
protected:
    unordered_map<string, TokenType> keywords;

public:
    virtual ~KeywordGroup() = default;
    virtual void initialize() = 0;

    TokenType getType(const string& word) const {
        const auto it = keywords.find(word);
        return (it != keywords.end()) ? it->second : TokenType::RELATION_NAME;
    }
};

#endif //COMPILADORESUFERSA_KEYWORDGROUP_H