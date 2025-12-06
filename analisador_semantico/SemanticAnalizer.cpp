#include "SemanticAnalyzer.h"
#include <sstream>
#include <algorithm>
#include <iostream>

// Códigos ANSI para uso interno
static const std::string RESET_C   = "\033[0m";
static const std::string CYAN_C    = "\033[36m";
static const std::string MAGENTA_C = "\033[1;35m";
static const std::string RED_C     = "\033[1;31m";

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

    verificarSubkindPattern(parserData);
    verificarRolePattern(parserData);
    verificarPhasePattern(parserData);
    verificarRelatorPattern(parserData);
    verificarModePattern(parserData);
    verificarRoleMixinPattern(parserData);

    return errosSemanticos.empty();
}

const std::vector<SinteseErro>& SemanticAnalyzer::getErros() const {
    return errosSemanticos;
}

void SemanticAnalyzer::erro(const std::string& arquivo, int linha, int coluna, const std::string& msg) {
    // Colore o nome do arquivo [Arquivo] em Ciano
    std::string arquivoColorido = CYAN_C + "[" + arquivo + "]" + RESET_C;

    std::string msgColorida;

    // Verifica se a mensagem começa com uma tag de padrão [Pattern Name]
    if (!msg.empty() && msg[0] == '[') {
        size_t pos = msg.find(']');
        if (pos != std::string::npos) {
            std::string tag = msg.substr(0, pos + 1);
            std::string resto = msg.substr(pos + 1);

            // Tag em Magenta, Resto da mensagem em Vermelho
            msgColorida = MAGENTA_C + tag + RESET_C + RED_C + resto + RESET_C;
        } else {
            // Se não fechar colchete, pinta tudo de vermelho
            msgColorida = RED_C + msg + RESET_C;
        }
    } else {
        // Mensagem sem tag de padrão, pinta tudo de vermelho
        msgColorida = RED_C + msg + RESET_C;
    }

    std::string msgFinal = arquivoColorido + " " + msgColorida;
    errosSemanticos.push_back({linha, coluna, msgFinal});
}

void SemanticAnalyzer::erro(const SinteseClasse& ctx, const std::string& msg) {
    erro(ctx.arquivo, ctx.linha, ctx.coluna, msg);
}

void SemanticAnalyzer::erro(const SinteseGenset& ctx, const std::string& msg) {
    erro(ctx.arquivo, ctx.linha, ctx.coluna, msg);
}

const SinteseClasse* SemanticAnalyzer::buscarClasse(const std::string& nome, const std::string& pacoteContexto, const Parser& parserData) {
    std::string chaveLocal = Parser::makeKey(pacoteContexto, nome);
    if (parserData.classesEncontradas.count(chaveLocal)) {
        return &parserData.classesEncontradas.at(chaveLocal);
    }

    if (parserData.importsPorPacote.count(pacoteContexto)) {
        const auto& imports = parserData.importsPorPacote.at(pacoteContexto);
        for (const auto& pkg : imports) {
            std::string chaveImportada = Parser::makeKey(pkg, nome);
            if (parserData.classesEncontradas.count(chaveImportada)) {
                return &parserData.classesEncontradas.at(chaveImportada);
            }
        }
    }

    return nullptr;
}

void SemanticAnalyzer::verificarHerancas(const Parser& parserData) {
    for (const auto& par : parserData.classesEncontradas) {
        const SinteseClasse& classe = par.second;

        if (classe.herdaDe.empty()) continue;

        std::vector<std::string> pais = split(classe.herdaDe, ',');

        for (const auto& nomePai : pais) {
            if (!buscarClasse(nomePai, classe.pacote, parserData)) {
                 bool encontradoComoDataType = false;
                 std::string chaveDT = Parser::makeKey(classe.pacote, nomePai);
                 if (parserData.datatypesEncontrados.count(chaveDT)) encontradoComoDataType = true;

                 if (!encontradoComoDataType) {
                     erro(classe, "A classe '" + classe.nome + "' herda de '" + nomePai + "' que nao foi encontrada.");
                 }
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

        if (!buscarClasse(gs.classeGeral, pacoteGenset, parserData)) {
            erro(gs, "No genset '" + gs.nome + "', a classe geral '" + gs.classeGeral + "' nao foi encontrada.");
        }

        for (const auto& nomeEspecifica : gs.classesEspecificas) {
            if (!buscarClasse(nomeEspecifica, pacoteGenset, parserData)) {
                erro(gs, "No genset '" + gs.nome + "', a classe especifica '" + nomeEspecifica + "' nao foi encontrada.");
            }
        }
    }
}

void SemanticAnalyzer::verificarSubkindPattern(const Parser& parserData) {
    for (const auto& par : parserData.classesEncontradas) {
        const SinteseClasse& classe = par.second;
        if (classe.estereotipo != "subkind") continue;

        if (classe.herdaDe.empty()) {
            erro(classe, "[Subkind Pattern] A classe subkind deve especializar um Kind.");
            continue;
        }

        std::vector<std::string> pais = split(classe.herdaDe, ',');
        bool paiRigidoEncontrado = false;
        for (const auto& nomePai : pais) {
            const SinteseClasse* classePai = buscarClasse(nomePai, classe.pacote, parserData);
            if (classePai && (classePai->estereotipo == "kind" || classePai->estereotipo == "subkind")) {
                paiRigidoEncontrado = true;
            }
        }

        if (!paiRigidoEncontrado) {
             erro(classe, "[Subkind Pattern] O subkind deve herdar de um Kind ou outro Subkind.");
        }

        bool participaDeGenset = false;
        for (const auto& gsPar : parserData.gensetsEncontrados) {
            const SinteseGenset& gs = gsPar.second;
            for (const auto& especifica : gs.classesEspecificas) {
                if (especifica == classe.nome) {
                    participaDeGenset = true; break;
                }
            }
            if (participaDeGenset) break;
        }

        if (!participaDeGenset) {
            erro(classe, "[Subkind Pattern] O subkind deve ser declarado em um genset.");
        }
    }
}

void SemanticAnalyzer::verificarRolePattern(const Parser& parserData) {
    for (const auto& par : parserData.classesEncontradas) {
        const SinteseClasse& classe = par.second;
        if (classe.estereotipo != "role") continue;

        if (classe.herdaDe.empty()) {
            erro(classe, "[Role Pattern] A role deve especializar uma classe.");
        } else {
            std::vector<std::string> pais = split(classe.herdaDe, ',');
            bool paiValido = false;
            for (const auto& nomePai : pais) {
                const SinteseClasse* classePai = buscarClasse(nomePai, classe.pacote, parserData);
                if (classePai && (classePai->estereotipo == "kind" || classePai->estereotipo == "role")) {
                    paiValido = true;
                }
            }
            if (!paiValido) {
                erro(classe, "[Role Pattern] A role herda de uma classe invalida para o padrao.");
            }
        }

        bool participaDeGenset = false;
        for (const auto& gsPar : parserData.gensetsEncontrados) {
            const SinteseGenset& gs = gsPar.second;
            for (const auto& especifica : gs.classesEspecificas) {
                if (especifica == classe.nome) {
                    participaDeGenset = true; break;
                }
            }
            if (participaDeGenset) break;
        }

        if (!participaDeGenset) {
            erro(classe, "[Role Pattern] A role deve participar de um genset.");
        }
    }
}

void SemanticAnalyzer::verificarPhasePattern(const Parser& parserData) {
    for (const auto& par : parserData.classesEncontradas) {
        const SinteseClasse& classe = par.second;
        if (classe.estereotipo != "phase") continue;

        if (classe.herdaDe.empty()) {
            erro(classe, "[Phase Pattern] A phase deve especializar uma classe.");
        }

        bool participaDeGensetDisjoint = false;
        bool participaDeAlgumGenset = false;

        for (const auto& gsPar : parserData.gensetsEncontrados) {
            const SinteseGenset& gs = gsPar.second;
            for (const auto& especifica : gs.classesEspecificas) {
                if (especifica == classe.nome) {
                    participaDeAlgumGenset = true;
                    if (gs.isDisjoint) participaDeGensetDisjoint = true;
                    break;
                }
            }
        }

        if (!participaDeAlgumGenset) {
            erro(classe, "[Phase Pattern] A phase deve participar de um genset.");
        } else if (!participaDeGensetDisjoint) {
            erro(classe, "[Phase Pattern] A phase deve participar de um genset marcado como 'disjoint'.");
        }
    }
}

void SemanticAnalyzer::verificarRelatorPattern(const Parser& parserData) {
    for (const auto& par : parserData.classesEncontradas) {
        const SinteseClasse& classe = par.second;
        if (classe.estereotipo != "relator") continue;

        int contagemMediacao = 0;
        for (const auto& attr : classe.atributos) {
            if (attr.metaAtributo == "@mediation") {
                contagemMediacao++;
                const SinteseClasse* alvo = buscarClasse(attr.tipo, classe.pacote, parserData);
                if (alvo) {
                    if (alvo->estereotipo != "role") {
                        erro(classe, "[Relator Pattern] Mediacao aponta para '" + attr.tipo + "' que e '" + alvo->estereotipo + "' (deveria ser 'role').");
                    }
                } else {
                    erro(classe, "[Relator Pattern] Classe alvo da mediacao '" + attr.tipo + "' nao encontrada.");
                }
            }
        }

        if (contagemMediacao < 2) {
            erro(classe, "[Relator Pattern] O relator deve ter pelo menos 2 mediacoes (@mediation).");
        }
    }
}

void SemanticAnalyzer::verificarModePattern(const Parser& parserData) {
    for (const auto& par : parserData.classesEncontradas) {
        const SinteseClasse& classe = par.second;
        if (classe.estereotipo != "mode") continue;

        bool temCharacterization = false;
        bool temExternalDependence = false;

        for (const auto& attr : classe.atributos) {
            if (attr.metaAtributo == "@characterization") temCharacterization = true;
            if (attr.metaAtributo == "@externalDependence") temExternalDependence = true;
        }

        if (!temCharacterization) erro(classe, "[Mode Pattern] Deve possuir @characterization.");
        if (!temExternalDependence) erro(classe, "[Mode Pattern] Deve possuir @externalDependence.");
    }
}

void SemanticAnalyzer::verificarRoleMixinPattern(const Parser& parserData) {
    for (const auto& par : parserData.classesEncontradas) {
        const SinteseClasse& classe = par.second;
        string ester = classe.estereotipo;
        if (ester != "roleMixin" && ester != "rolemixin") continue;

        bool ehGeneralDeGensetValido = false;
        for (const auto& gsPar : parserData.gensetsEncontrados) {
            const SinteseGenset& gs = gsPar.second;
            if (gs.classeGeral == classe.nome) {
                if (gs.isDisjoint && gs.isComplete) {
                    ehGeneralDeGensetValido = true;
                    for (const auto& specName : gs.classesEspecificas) {
                        const SinteseClasse* spec = buscarClasse(specName, classe.pacote, parserData);
                        if (spec && spec->estereotipo != "role" && spec->estereotipo != "roleMixin" && spec->estereotipo != "rolemixin") {
                             erro(classe, "[RoleMixin Pattern] A classe filha '" + specName + "' deve ser Role.");
                        }
                    }
                } else {
                     erro(classe, "[RoleMixin Pattern] O genset '" + gs.nome + "' deve ser 'disjoint' e 'complete'.");
                }
            }
        }

        if (!ehGeneralDeGensetValido) {
            erro(classe, "[RoleMixin Pattern] Deve ser a classe geral de um genset disjoint e complete.");
        }
    }
}