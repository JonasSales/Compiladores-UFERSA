#ifndef TONTO_LEXER_SEMANTICANALYZER_H
#define TONTO_LEXER_SEMANTICANALYZER_H

#include <vector>
#include <string>
#include <iostream>

#include "../analisador_sintatico/Parser.h"

class SemanticAnalyzer {
public:
    SemanticAnalyzer() = default;
    bool analisar(const Parser& parserData);

    const std::vector<SinteseErro> &getErros() const;

private:
    std::vector<SinteseErro> errosSemanticos;

    void erro(const std::string& arquivo, int linha, int coluna, const std::string& msg);

    void erro(const SinteseClasse& contexto, const std::string& msg);
    void erro(const SinteseGenset& contexto, const std::string& msg);

    const SinteseClasse* buscarClasse(const std::string& nome, const std::string& pacoteContexto, const Parser& parserData);

    void verificarHerancas(const Parser& parserData);
    void verificarGensets(const Parser& parserData);

    void verificarSubkindPattern(const Parser& parserData);
    void verificarRolePattern(const Parser& parserData);
    void verificarPhasePattern(const Parser& parserData);
    void verificarRelatorPattern(const Parser& parserData);
    void verificarModePattern(const Parser& parserData);
    void verificarRoleMixinPattern(const Parser& parserData);
};

#endif //TONTO_LEXER_SEMANTICANALYZER_H