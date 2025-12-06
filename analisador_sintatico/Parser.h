#ifndef COMPILADORESUFERSA_PARSER_H
#define COMPILADORESUFERSA_PARSER_H

#include <vector>
#include <string>
#include <map>

using std::map;
using std::vector;
using std::string;

struct SinteseErro {
    int linha;
    int coluna;
    string mensagem;
};

struct SinteseAtributo {
    string nome;
    string tipo;
    string metaAtributo;
};

struct SinteseClasse {
    string estereotipo;
    string nome;
    string pacote;
    string herdaDe;
    vector<SinteseAtributo> atributos;

    // Novos campos de localização
    int linha = 0;
    int coluna = 0;
    string arquivo;
};

struct SinteseGenset {
    string nome;
    string classeGeral;
    vector<string> classesEspecificas;
    bool isDisjoint;
    bool isComplete;

    // Novos campos de localização
    int linha = 0;
    int coluna = 0;
    string arquivo;
};

struct SinteseDatatype {
    string nome;
    string pacote;
    vector<SinteseAtributo> atributos;

    // Novos campos de localização
    int linha = 0;
    int coluna = 0;
    string arquivo;
};

struct SinteseEnum {
    string nome;
    string pacote;
    vector<string> instancias;
};

struct SinteseRelacao {
    string estereotipo;
    string nome;
    string dominio;
    string cardinalidadeDominio;
    string imagem;
    string cardinalidadeImagem;
    string simboloRelacao;

    // Novos campos de localização
    int linha = 0;
    int coluna = 0;
    string arquivo;
};

class Parser {
public:
    string pacoteAtual = "default";
    string arquivoAtual; // Novo: Armazena o nome do arquivo sendo processado
    string ultimoLexema;

    vector<string> pacotesEncontrados;

    vector<string> importsTemporarios;
    map<string, vector<string>> importsPorPacote;

    map<string, SinteseClasse> classesEncontradas;
    map<string, SinteseGenset> gensetsEncontrados;
    map<string, SinteseDatatype> datatypesEncontrados;
    map<string, SinteseEnum> enumsEncontrados;

    vector<SinteseRelacao> relacoesExternasEncontradas;
    vector<SinteseErro> errosSintaticos;

    Parser() = default;

    void addErro(const int l, const int c, const string& msg) {
        errosSintaticos.push_back({l, c, msg});
    }

    static string makeKey(const string& pacote, const string& nome) {
        return pacote + "::" + nome;
    }

    void addImport(const string& pkg) {
        if (pacoteAtual == "default") {
            importsTemporarios.push_back(pkg);
        } else {
            importsPorPacote[pacoteAtual].push_back(pkg);
        }
    }

    void definirPacote(const string& pkg) {
        pacoteAtual = pkg;
        pacotesEncontrados.push_back(pkg);

        if (!importsTemporarios.empty()) {
            importsPorPacote[pkg].insert(
                importsPorPacote[pkg].end(),
                importsTemporarios.begin(),
                importsTemporarios.end()
            );
            importsTemporarios.clear();
        }
    }
};

#endif //COMPILADORESUFERSA_PARSER_H