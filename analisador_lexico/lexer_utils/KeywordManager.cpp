#include "KeywordManager.h"
#include "lex_config.h"

KeywordManager::KeywordManager() {
    // Estereótipos de classe
    keywordMap = {
        {"event",               TokenType::CLASS_STEREOTYPE},
        {"situation",           TokenType::CLASS_STEREOTYPE},
        {"process",             TokenType::CLASS_STEREOTYPE},
        {"category",            TokenType::CLASS_STEREOTYPE},
        {"mixin",               TokenType::CLASS_STEREOTYPE},
        {"phaseMixin",          TokenType::CLASS_STEREOTYPE},
        {"roleMixin",           TokenType::CLASS_STEREOTYPE},
        {"historicalRoleMixin", TokenType::CLASS_STEREOTYPE},
        {"kind",                TokenType::CLASS_STEREOTYPE},
        {"collective",          TokenType::CLASS_STEREOTYPE},
        {"quantity",            TokenType::CLASS_STEREOTYPE},
        {"quality",             TokenType::CLASS_STEREOTYPE},
        {"mode",                TokenType::CLASS_STEREOTYPE},
        {"intrinsicMode",       TokenType::CLASS_STEREOTYPE},
        {"extrinsicMode",       TokenType::CLASS_STEREOTYPE},
        {"subkind",             TokenType::CLASS_STEREOTYPE},
        {"phase",               TokenType::CLASS_STEREOTYPE},
        {"role",                TokenType::CLASS_STEREOTYPE},
        {"historicalRole",      TokenType::CLASS_STEREOTYPE},
        {"relator",             TokenType::CLASS_STEREOTYPE},
        {"type",                TokenType::CLASS_STEREOTYPE},
        {"powertype",           TokenType::CLASS_STEREOTYPE},

        // Estereótipos de relação
        {"material",            TokenType::RELATION_STEREOTYPE},
        {"derivation",          TokenType::RELATION_STEREOTYPE},
        {"comparative",         TokenType::RELATION_STEREOTYPE},
        {"mediation",           TokenType::RELATION_STEREOTYPE},
        {"characterization",    TokenType::RELATION_STEREOTYPE},
        {"externalDependence",  TokenType::RELATION_STEREOTYPE},
        {"componentOf",         TokenType::RELATION_STEREOTYPE},
        {"memberOf",            TokenType::RELATION_STEREOTYPE},
        {"subCollectionOf",     TokenType::RELATION_STEREOTYPE},
        {"subQualityOf",        TokenType::RELATION_STEREOTYPE},
        {"instantiation",       TokenType::RELATION_STEREOTYPE},
        {"termination",         TokenType::RELATION_STEREOTYPE},
        {"participational",     TokenType::RELATION_STEREOTYPE},
        {"participation",       TokenType::RELATION_STEREOTYPE},
        {"historicalDependence",TokenType::RELATION_STEREOTYPE},
        {"creation",            TokenType::RELATION_STEREOTYPE},
        {"manifestation",       TokenType::RELATION_STEREOTYPE},
        {"bringsAbout",         TokenType::RELATION_STEREOTYPE},
        {"triggers",            TokenType::RELATION_STEREOTYPE},
        {"composition",         TokenType::RELATION_STEREOTYPE},
        {"aggregation",         TokenType::RELATION_STEREOTYPE},
        {"inherence",           TokenType::RELATION_STEREOTYPE},
        {"value",               TokenType::RELATION_STEREOTYPE},
        {"formal",              TokenType::RELATION_STEREOTYPE},
        {"constitution",        TokenType::RELATION_STEREOTYPE},

        // Palavras reservadas
        {"genset",              TokenType::RESERVED_WORD},
        {"disjoint",            TokenType::RESERVED_WORD},
        {"complete",            TokenType::RESERVED_WORD},
        {"general",             TokenType::RESERVED_WORD},
        {"specifics",           TokenType::RESERVED_WORD},
        {"where",               TokenType::RESERVED_WORD},
        {"package",             TokenType::RESERVED_WORD},
        {"import",              TokenType::RESERVED_WORD},
        {"functional-complexes",TokenType::RESERVED_WORD},
        {"intrinsic-modes",     TokenType::RESERVED_WORD},

        // Tipos nativos
        {"number",              TokenType::NATIVE_DATA_TYPE},
        {"string",              TokenType::NATIVE_DATA_TYPE},
        {"boolean",             TokenType::NATIVE_DATA_TYPE},
        {"date",                TokenType::NATIVE_DATA_TYPE},
        {"time",                TokenType::NATIVE_DATA_TYPE},
        {"datetime",            TokenType::NATIVE_DATA_TYPE},

        // Meta-atributos
        {"ordered",             TokenType::META_ATTRIBUTE},
        {"const",               TokenType::META_ATTRIBUTE},
        {"derived",             TokenType::META_ATTRIBUTE},
        {"subsets",             TokenType::META_ATTRIBUTE},
        {"redefines",           TokenType::META_ATTRIBUTE}
    };
}

TokenType KeywordManager::getType(const string& word) const {
    const auto it = keywordMap.find(word);
    return (it != keywordMap.end()) ? it->second : TokenType::RELATION_NAME;
}