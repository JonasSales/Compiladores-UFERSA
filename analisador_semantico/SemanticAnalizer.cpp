#include "SemanticAnalyzer.h"
#include <sstream>
#include <algorithm>
#include <iostream>

static std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    if (std::string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

static std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        std::string trimmedToken = trim(token);
        if (!trimmedToken.empty()) {
            tokens.push_back(trimmedToken);
        }
    }
    return tokens;
}

bool SemanticAnalyzer::analisar(const Parser& parserData) {
    errosSemanticos.clear();

    verificarHerancas(parserData);

    verificarGensets(parserData);

    return errosSemanticos.empty();
}

const std::vector<SinteseErro>& SemanticAnalyzer::getErros() const {
    return errosSemanticos;
}

void SemanticAnalyzer::erro(const std::string& msg) {
    errosSemanticos.push_back({0, 0, msg});
}

void SemanticAnalyzer::verificarHerancas(const Parser& parserData) {
    for (const auto& par : parserData.classesEncontradas) {
        const SinteseClasse& classe = par.second;

        if (classe.herdaDe.empty()) continue;

        std::vector<std::string> pais = split(classe.herdaDe, ',');

        for (const auto& nomePai : pais) {
            bool encontrado = false;

            std::string chaveLocal = Parser::makeKey(classe.pacote, nomePai);

            if (parserData.classesEncontradas.count(chaveLocal)) {
                encontrado = true;
            }

            if (!encontrado) {
                if (parserData.importsPorPacote.count(classe.pacote)) {
                    const auto& pacotesImportados = parserData.importsPorPacote.at(classe.pacote);

                    for (const auto& pkgImportado : pacotesImportados) {
                        std::string chaveImportada = Parser::makeKey(pkgImportado, nomePai);

                        if (parserData.classesEncontradas.count(chaveImportada)) {
                            encontrado = true;
                            break;
                        }
                    }
                }
            }

            if (!encontrado) {
                std::string chaveDTLocal = Parser::makeKey(classe.pacote, nomePai);
                if (parserData.datatypesEncontrados.count(chaveDTLocal)) encontrado = true;

                if (!encontrado && parserData.importsPorPacote.count(classe.pacote)) {
                    const auto& imports = parserData.importsPorPacote.at(classe.pacote);
                    for (const auto& pkg : imports) {
                        std::string chaveDTImp = Parser::makeKey(pkg, nomePai);
                        if (parserData.datatypesEncontrados.count(chaveDTImp)) {
                            encontrado = true;
                            break;
                        }
                    }
                }
            }

            if (!encontrado) {
                std::string msg = "A classe '" + classe.nome + "' (pacote: " + classe.pacote +
                                  ") herda de '" + nomePai + "' que não foi encontrada";
                erro(msg);
            }
        }
    }
}

void SemanticAnalyzer::verificarGensets(const Parser& parserData) {
    for (const auto& par : parserData.gensetsEncontrados) {
        const SinteseGenset& gs = par.second;

        std::string pacoteGenset = "default";
        std::string chaveGenset = par.first;
        size_t posSeparator = chaveGenset.find("::");
        if (posSeparator != std::string::npos) {
            pacoteGenset = chaveGenset.substr(0, posSeparator);
        }

        bool generalExiste = false;
        std::string nomeGeneral = gs.classeGeral;

        if (parserData.classesEncontradas.count(Parser::makeKey(pacoteGenset, nomeGeneral))) {
            generalExiste = true;
        }
        else if (parserData.importsPorPacote.count(pacoteGenset)) {
            for (const auto& pkg : parserData.importsPorPacote.at(pacoteGenset)) {
                if (parserData.classesEncontradas.count(Parser::makeKey(pkg, nomeGeneral))) {
                    generalExiste = true;
                    break;
                }
            }
        }

        if (!generalExiste) {
            erro("No genset '" + gs.nome + "', a classe geral '" + nomeGeneral + "' nao foi encontrada.");
        }

        for (const auto& nomeEspecifica : gs.classesEspecificas) {
            bool especificaExiste = false;

            if (parserData.classesEncontradas.count(Parser::makeKey(pacoteGenset, nomeEspecifica))) {
                especificaExiste = true;
            }
            else if (parserData.importsPorPacote.count(pacoteGenset)) {
                for (const auto& pkg : parserData.importsPorPacote.at(pacoteGenset)) {
                    if (parserData.classesEncontradas.count(Parser::makeKey(pkg, nomeEspecifica))) {
                        especificaExiste = true;
                        break;
                    }
                }
            }

            if (!especificaExiste) {
                erro("No genset '" + gs.nome + "', a classe especifica '" + nomeEspecifica + "' nao foi encontrada.");
            }
        }
    }
}