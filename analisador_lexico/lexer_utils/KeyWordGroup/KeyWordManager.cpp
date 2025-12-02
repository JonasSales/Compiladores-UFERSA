#include "KeyWordManager.h"

#include "ClassStereotypeKeywords.h"
#include "MetaAttributeKeywords.h"
#include "NativeTypeKeywords.h"
#include "RelationStereotypeKeywords.h"
#include "ReservedWordKeywords.h"

KeyWordManager::KeyWordManager() {
    groups.emplace_back(std::make_unique<ClassStereotypeKeywords>());
    groups.emplace_back(std::make_unique<RelationStereotypeKeywords>());
    groups.emplace_back(std::make_unique<ReservedWordKeywords>());
    groups.emplace_back(std::make_unique<NativeTypeKeywords>());
    groups.emplace_back(std::make_unique<MetaAttributeKeywords>());

    for (const auto& g : groups)
        g->initialize();
}

TokenType KeyWordManager::getType(const std::string& word) const {
    for (const auto& group : groups) {
        if (const TokenType type = group->getType(word); type != TokenType::RELATION_NAME)  // RELATION_NAME = “não encontrado”
            return type;
    }
    return TokenType::RELATION_NAME;
}