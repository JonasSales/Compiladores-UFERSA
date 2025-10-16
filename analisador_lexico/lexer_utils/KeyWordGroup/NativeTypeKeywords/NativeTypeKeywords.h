//
// Created by jonas on 10/15/25.
//

#ifndef COMPILADORESUFERSA_NATIVETYPEKEYWORDS_H
#define COMPILADORESUFERSA_NATIVETYPEKEYWORDS_H

#include "../KeyWordGroup.h"

class NativeTypeKeywords final : public KeywordGroup {
public:
    void initialize() override {
        keywords = {
            {"number", TokenType::NATIVE_DATA_TYPE},
            {"string", TokenType::NATIVE_DATA_TYPE},
            {"boolean", TokenType::NATIVE_DATA_TYPE},
            {"date", TokenType::NATIVE_DATA_TYPE},
            {"time", TokenType::NATIVE_DATA_TYPE},
            {"datetime", TokenType::NATIVE_DATA_TYPE}
        };
    }
};

#endif //COMPILADORESUFERSA_NATIVETYPEKEYWORDS_H