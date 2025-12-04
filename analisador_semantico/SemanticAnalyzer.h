#ifndef TONTO_LEXER_SEMANTICANALYZER_H
#define TONTO_LEXER_SEMANTICANALYZER_H

#include <vector>
#include <string>
#include <iostream>

#include "analisador_sintatico/Parser.h"

class SemanticAnalyzer {
public:
    SemanticAnalyzer() = default;
    bool analisar(const Parser& parserData);

    const std::vector<SinteseErro> &getErros() const;

private:
    std::vector<SinteseErro> errosSemanticos;

    void verificarHerancas(const Parser& parserData);
    void verificarGensets(const Parser& parserData);

    void erro(const std::string& msg);
};

#endif //TONTO_LEXER_SEMANTICANALYZER_H