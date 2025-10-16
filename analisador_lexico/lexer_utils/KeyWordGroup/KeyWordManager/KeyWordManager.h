#ifndef COMPILADORESUFERSA_KEYWORDMANAGER_H
#define COMPILADORESUFERSA_KEYWORDMANAGER_H

#include <vector>
#include <memory>
#include "../KeyWordGroup.h"

class KeyWordManager {
private:
    std::vector<std::unique_ptr<KeywordGroup>> groups;

public:
    KeyWordManager();

    [[nodiscard]] TokenType getType(const std::string& word) const;
};
#endif //COMPILADORESUFERSA_KEYWORDMANAGER_H