#ifndef COMPILADORESUFERSA_RELATIONSTEREOTYPEKEYWORDS_H
#define COMPILADORESUFERSA_RELATIONSTEREOTYPEKEYWORDS_H

#include "KeyWordGroup.h"

class RelationStereotypeKeywords final : public KeywordGroup {
public:
    void initialize() override {
        keywords = {
            {"material", TokenType::RELATION_STEREOTYPE},
            {"@material", TokenType::RELATION_STEREOTYPE},
            {"derivation", TokenType::RELATION_STEREOTYPE},
            {"comparative", TokenType::RELATION_STEREOTYPE},
            {"mediation", TokenType::RELATION_STEREOTYPE},
            {"@mediation", TokenType::RELATION_STEREOTYPE},
            {"characterization", TokenType::RELATION_STEREOTYPE},
            {"externalDependence", TokenType::RELATION_STEREOTYPE},
            {"componentOf", TokenType::RELATION_STEREOTYPE},
            {"@componentOf", TokenType::RELATION_STEREOTYPE},
            {"memberOf", TokenType::RELATION_STEREOTYPE},
            {"@memberOf", TokenType::RELATION_STEREOTYPE},
            {"subCollectionOf", TokenType::RELATION_STEREOTYPE},
            {"@subCollectionOf", TokenType::RELATION_STEREOTYPE},
            {"subQualityOf", TokenType::RELATION_STEREOTYPE},
            {"instantiation", TokenType::RELATION_STEREOTYPE},
            {"termination", TokenType::RELATION_STEREOTYPE},
            {"participational", TokenType::RELATION_STEREOTYPE},
            {"participation", TokenType::RELATION_STEREOTYPE},
            {"historicalDependence", TokenType::RELATION_STEREOTYPE},
            {"creation", TokenType::RELATION_STEREOTYPE},
            {"manifestation", TokenType::RELATION_STEREOTYPE},
            {"bringsAbout", TokenType::RELATION_STEREOTYPE},
            {"triggers", TokenType::RELATION_STEREOTYPE},
            {"composition", TokenType::RELATION_STEREOTYPE},
            {"aggregation", TokenType::RELATION_STEREOTYPE},
            {"inherence", TokenType::RELATION_STEREOTYPE},
            {"value", TokenType::RELATION_STEREOTYPE},
            {"formal", TokenType::RELATION_STEREOTYPE},
            {"constitution", TokenType::RELATION_STEREOTYPE}
        };
    }
};

#endif //COMPILADORESUFERSA_RELATIONSTEREOTYPEKEYWORDS_H