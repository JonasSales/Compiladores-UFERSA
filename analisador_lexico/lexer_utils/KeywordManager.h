//
// Created by guilherme on 09/10/2025.
//

#ifndef COMPILADORESUFERSA_KEYWORDMANAGER_H
#define COMPILADORESUFERSA_KEYWORDMANAGER_H

#include <map>
#include <string>
#include "../entities/Token.h"

using std::map;
using std::string;


class KeywordManager {
private:
    map<string, TokenType> keyword_map;

public:
    KeywordManager();
    TokenType getType(const string& word) const;
};

#endif //COMPILADORESUFERSA_KEYWORDMANAGER_H