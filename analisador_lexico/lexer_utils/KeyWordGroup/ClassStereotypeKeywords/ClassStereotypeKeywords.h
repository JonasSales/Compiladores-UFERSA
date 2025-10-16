#ifndef COMPILADORESUFERSA_CLASSSTEREOTYPEKEYWORDS_H
#define COMPILADORESUFERSA_CLASSSTEREOTYPEKEYWORDS_H

#include "../KeyWordGroup.h"

class ClassStereotypeKeywords final : public KeywordGroup {
public:
    void initialize() override {
        keywords = {
            {"event", TokenType::CLASS_STEREOTYPE},
            {"situation", TokenType::CLASS_STEREOTYPE},
            {"process", TokenType::CLASS_STEREOTYPE},
            {"category", TokenType::CLASS_STEREOTYPE},
            {"mixin", TokenType::CLASS_STEREOTYPE},
            {"phaseMixin", TokenType::CLASS_STEREOTYPE},
            {"roleMixin", TokenType::CLASS_STEREOTYPE},
            {"historicalRoleMixin", TokenType::CLASS_STEREOTYPE},
            {"kind", TokenType::CLASS_STEREOTYPE},
            {"collective", TokenType::CLASS_STEREOTYPE},
            {"quantity", TokenType::CLASS_STEREOTYPE},
            {"quality", TokenType::CLASS_STEREOTYPE},
            {"mode", TokenType::CLASS_STEREOTYPE},
            {"intrinsicMode", TokenType::CLASS_STEREOTYPE},
            {"extrinsicMode", TokenType::CLASS_STEREOTYPE},
            {"subkind", TokenType::CLASS_STEREOTYPE},
            {"phase", TokenType::CLASS_STEREOTYPE},
            {"role", TokenType::CLASS_STEREOTYPE},
            {"historicalRole", TokenType::CLASS_STEREOTYPE},
            {"relator", TokenType::CLASS_STEREOTYPE},
            {"type", TokenType::CLASS_STEREOTYPE},
            {"powertype", TokenType::CLASS_STEREOTYPE}
        };
    }
};

#endif //COMPILADORESUFERSA_CLASSSTEREOTYPEKEYWORDS_H