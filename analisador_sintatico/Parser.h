#ifndef COMPILADORESUFERSA_PARSER_H
#define COMPILADORESUFERSA_PARSER_H

#include <vector>
#include <string>

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
};

struct SinteseGenset {
    string nome;
    string classeGeral;
    vector<string> classesEspecificas;
    bool isDisjoint;
    bool isComplete;
};

struct SinteseDatatype {
    string nome;
    string pacote;
    vector<SinteseAtributo> atributos;
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
};

class Parser {
public:
    string pacoteAtual = "default";
    vector<string> pacotesEncontrados;
    vector<SinteseClasse> classesEncontradas;
    vector<SinteseGenset> gensetsEncontrados;
    vector<SinteseDatatype> datatypesEncontrados;
    vector<SinteseEnum> enumsEncontrados;
    vector<SinteseRelacao> relacoesExternasEncontradas;
    vector<SinteseErro> errosSintaticos;

    Parser() = default;

    void addErro(int l, int c, const string& msg) {
        errosSintaticos.push_back({l, c, msg});
    }
};

#endif //COMPILADORESUFERSA_PARSER_H