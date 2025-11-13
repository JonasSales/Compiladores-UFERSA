#ifndef COMPILADORESUFERSA_PARSER_H
#define COMPILADORESUFERSA_PARSER_H

#include "analisador_lexico/entities/Token.h"
#include <map>
#include <vector>
#include <string>

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
    // (Pode-se adicionar 'vector<SinteseRelacao> relacoesInternas' aqui)
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
private:
    vector<Token> tokenStream;
    unsigned int currentTokenIndex;

    // Funções de utilidade do Parser
    [[nodiscard]] Token lookAhead(int offset = 0) const;
    Token consumeToken();
    bool match(TokenType type);
    bool match(TokenType type, const string& lexeme);

    void addErro(const string& mensagem, const Token& tokenErrado);
    void addErro(const string& mensagem);

    // Funções de parsing para as regras do PDF
    void parseProgram();
    void parsePackageDecl();
    void parseClassDecl();
    SinteseAtributo parseAttributeDecl();
    void parseGeneralizationSet();
    void parseDatatypeDecl();
    void parseEnumDecl();
    void parseExternalRelationDecl();
    // void parseInternalRelationDecl(SinteseClasse& classe); // (Stub)

public:
    string pacoteAtual;
    vector<string> pacotesEncontrados;
    vector<SinteseClasse> classesEncontradas;
    vector<SinteseGenset> gensetsEncontrados;
    vector<SinteseDatatype> datatypesEncontrados;
    vector<SinteseEnum> enumsEncontrados;
    vector<SinteseRelacao> relacoesExternasEncontradas;
    vector<SinteseErro> errosSintaticos;

    explicit Parser(const map<int, Token>& tokens);
    void parse();
};

#endif //COMPILADORESUFERSA_PARSER_H