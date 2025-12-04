#include "JsonWriter.h"
#include "analisador_sintatico/Parser.h"
#include <fstream>
#include <iostream>
#include <iterator>

using namespace std;

string escapeJson(const string& s) {
    string escaped;
    escaped.reserve(s.length());
    for (const char c : s) {
        switch (c) {
            case '"':  escaped += "\\\""; break;
            case '\\': escaped += "\\\\"; break;
            case '\b': escaped += "\\b";  break;
            case '\f': escaped += "\\f";  break;
            case '\n': escaped += "\\n";  break;
            case '\r': escaped += "\\r";  break;
            case '\t': escaped += "\\t";  break;
            default:   escaped += c;      break;
        }
    }
    return escaped;
}

void writeTokenList(const string &filename, const map<int, Token> &tokens) {
    ofstream fout(filename);

    if (!fout.is_open()) {
        cerr << "Erro: Nao foi possivel abrir o arquivo de saida: " << filename << endl;
        return;
    }

    fout << "[" << endl;

    for (auto it = tokens.begin(); it != tokens.end(); ++it) {
        const auto& token = it->second;

        fout << "  {" << endl;
        fout << "    \"id\": " << token.getContador() << "," << endl;
        fout << "    \"linha\": " << token.getLine() << "," << endl;
        fout << "    \"coluna\": " << token.getColumn() << "," << endl;
        fout << R"(    "tipo": ")" << tokenToString(token.getTokenType()) << "\"," << endl;
        fout << R"(    "lexema": ")" << escapeJson(token.getLexeme()) << "\"" << endl;
        fout << "  }";

        if (next(it) != tokens.end()) {
            fout << ",";
        }
        fout << endl;
    }

    fout << "]" << endl;
    fout.close();
}

void writeSyntaxAnalysis(const string &filename, const Parser &parser) {
    ofstream fout(filename);

    if (!fout.is_open()) {
        cerr << "Erro: Nao foi possivel abrir o arquivo de saida: " << filename << endl;
        return;
    }

    fout << "{" << endl;

    fout << "  \"classes\": [" << endl;
    for (auto it = parser.classesEncontradas.begin(); it != parser.classesEncontradas.end(); ++it) {
        const auto& c = it->second;
        fout << "    {" << endl;
        fout << "      \"pacote\": \"" << escapeJson(c.pacote) << "\"," << endl;
        fout << "      \"nome\": \"" << escapeJson(c.nome) << "\"," << endl;
        fout << "      \"estereotipo\": \"" << escapeJson(c.estereotipo) << "\"," << endl;
        fout << "      \"herdaDe\": \"" << escapeJson(c.herdaDe) << "\"," << endl;
        fout << "      \"atributos\": [" << endl;
        for (size_t i = 0; i < c.atributos.size(); ++i) {
            fout << "        { \"nome\": \"" << escapeJson(c.atributos[i].nome) << "\", "
                 << "\"tipo\": \"" << escapeJson(c.atributos[i].tipo) << "\", "
                 << "\"metaAtributo\": \"" << escapeJson(c.atributos[i].metaAtributo) << "\" }";
            if (i < c.atributos.size() - 1) fout << ",";
            fout << endl;
        }
        fout << "      ]" << endl;
        fout << "    }";
        if (next(it) != parser.classesEncontradas.end()) fout << ",";
        fout << endl;
    }
    fout << "  ]," << endl;

    fout << "  \"gensets\": [" << endl;
    for (auto it = parser.gensetsEncontrados.begin(); it != parser.gensetsEncontrados.end(); ++it) {
        const auto& gs = it->second;
        fout << "    {" << endl;
        fout << "      \"nome\": \"" << escapeJson(gs.nome) << "\"," << endl;
        fout << "      \"classeGeral\": \"" << escapeJson(gs.classeGeral) << "\"," << endl;
        fout << "      \"disjoint\": " << (gs.isDisjoint ? "true" : "false") << "," << endl;
        fout << "      \"complete\": " << (gs.isComplete ? "true" : "false") << "," << endl;
        fout << "      \"especificas\": [";
        for (size_t i = 0; i < gs.classesEspecificas.size(); ++i) {
            fout << "\"" << escapeJson(gs.classesEspecificas[i]) << "\"";
            if (i < gs.classesEspecificas.size() - 1) fout << ", ";
        }
        fout << "]" << endl;
        fout << "    }";
        if (next(it) != parser.gensetsEncontrados.end()) fout << ",";
        fout << endl;
    }
    fout << "  ]," << endl;

    fout << "  \"datatypes\": [" << endl;
    for (auto it = parser.datatypesEncontrados.begin(); it != parser.datatypesEncontrados.end(); ++it) {
        const auto& dt = it->second;
        fout << "    {" << endl;
        fout << "      \"nome\": \"" << escapeJson(dt.nome) << "\"," << endl;
        fout << "      \"pacote\": \"" << escapeJson(dt.pacote) << "\"," << endl;
        fout << "      \"atributos\": [" << endl;
        for (size_t i = 0; i < dt.atributos.size(); ++i) {
            fout << "        { \"nome\": \"" << escapeJson(dt.atributos[i].nome) << "\", "
                 << "\"tipo\": \"" << escapeJson(dt.atributos[i].tipo) << "\" }";
            if (i < dt.atributos.size() - 1) fout << ",";
            fout << endl;
        }
        fout << "      ]" << endl;
        fout << "    }";
        if (next(it) != parser.datatypesEncontrados.end()) fout << ",";
        fout << endl;
    }
    fout << "  ]," << endl;

    fout << "  \"enums\": [" << endl;
    for (auto it = parser.enumsEncontrados.begin(); it != parser.enumsEncontrados.end(); ++it) {
        const auto& en = it->second;
        fout << "    {" << endl;
        fout << "      \"nome\": \"" << escapeJson(en.nome) << "\"," << endl;
        fout << "      \"pacote\": \"" << escapeJson(en.pacote) << "\"," << endl;
        fout << "      \"instancias\": [";
        for (size_t i = 0; i < en.instancias.size(); ++i) {
            fout << "\"" << escapeJson(en.instancias[i]) << "\"";
            if (i < en.instancias.size() - 1) fout << ", ";
        }
        fout << "]" << endl;
        fout << "    }";
        if (next(it) != parser.enumsEncontrados.end()) fout << ",";
        fout << endl;
    }
    fout << "  ]," << endl;

    fout << "  \"relacoes\": [" << endl;
    for (size_t i = 0; i < parser.relacoesExternasEncontradas.size(); ++i) {
        const auto& rel = parser.relacoesExternasEncontradas[i];
        fout << "    {" << endl;
        fout << "      \"nome\": \"" << escapeJson(rel.nome) << "\"," << endl;
        fout << "      \"estereotipo\": \"" << escapeJson(rel.estereotipo) << "\"," << endl;
        fout << "      \"dominio\": \"" << escapeJson(rel.dominio) << "\"," << endl;
        fout << "      \"cardinalidadeDominio\": \"" << escapeJson(rel.cardinalidadeDominio) << "\"," << endl;
        fout << "      \"simbolo\": \"" << escapeJson(rel.simboloRelacao) << "\"," << endl;
        fout << "      \"imagem\": \"" << escapeJson(rel.imagem) << "\"," << endl;
        fout << "      \"cardinalidadeImagem\": \"" << escapeJson(rel.cardinalidadeImagem) << "\"" << endl;
        fout << "    }";
        if (i < parser.relacoesExternasEncontradas.size() - 1) fout << ",";
        fout << endl;
    }
    fout << "  ]," << endl;

    fout << "  \"erros\": [" << endl;
    for (size_t i = 0; i < parser.errosSintaticos.size(); ++i) {
        const auto& erro = parser.errosSintaticos[i];
        fout << "    {" << endl;
        fout << "      \"linha\": " << erro.linha << "," << endl;
        fout << "      \"coluna\": " << erro.coluna << "," << endl;
        fout << "      \"mensagem\": \"" << escapeJson(erro.mensagem) << "\"" << endl;
        fout << "    }";
        if (i < parser.errosSintaticos.size() - 1) fout << ",";
        fout << endl;
    }
    fout << "  ]" << endl;

    fout << "}" << endl;
    fout.close();
}