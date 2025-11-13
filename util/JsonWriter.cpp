#include "JsonWriter.h"
#include "analisador_lexico/entities/TokenAnalisys.h"
#include "analisador_sintatico/Parser.h"

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
        cerr << "Erro: Não foi possível abrir o arquivo de saída: " << filename << endl;
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

    for (const auto& pair : tokens) {
        tokenAnalisys.addToken(pair.second.getTokenType(), pair.second.getLexeme());
    }
    fout.close();
}

void writeTokenAnalysis(const string &filename, const TokenAnalisys &result) {
    ofstream fout(filename);

    if (!fout.is_open()) {
        cerr << "Erro: Não foi possível abrir o arquivo de saída: " << filename << endl;
        return;
    }

    fout << "{" << endl;
    fout << "  \"Contagem de Tokens\": {" << endl;

    for (auto it = result.tokenTypeCount.begin(); it != result.tokenTypeCount.end(); ++it) {
        TokenType currentType = it->first;
        const int currentCount = it->second;

        fout << "    \"" << tokenToString(currentType) << "\": {" << endl;
        fout << "      \"count\": " << currentCount << "," << endl;
        fout << "      \"tokens\": [" << endl;

        if (auto listIt = result.uniqueLexemes.find(currentType); listIt != result.uniqueLexemes.end()) {
            const auto& uniqueTokenSet = listIt->second;
            for (auto set_it = uniqueTokenSet.begin(); set_it != uniqueTokenSet.end(); ++set_it) {
                fout << "        \"" << escapeJson(*set_it) << "\"";
                if (next(set_it) != uniqueTokenSet.end()) {
                    fout << ",";
                }
                fout << endl;
            }
        }

        fout << "      ]" << endl;
        fout << "    }";

        if (next(it) != result.tokenTypeCount.end()) {
            fout << ",";
        }
        fout << endl;
    }

    fout << "  }" << endl;
    fout << "}" << endl;

    fout.close();
}

void writeSyntaxAnalysis(const string &filename, const Parser &parser) {
    ofstream fout(filename);
    if (!fout.is_open()) {
        cerr << "Erro: Nao foi possivel abrir o arquivo de saida: " << filename << endl;
        return;
    }

    fout << "{" << endl;
    fout << "  \"Tabela de Sintese\": {" << endl;

    // Pacotes
    fout << "    \"pacotes\": [" << endl;
    for (size_t i = 0; i < parser.pacotesEncontrados.size(); ++i) {
        fout << "      \"" << escapeJson(parser.pacotesEncontrados[i]) << "\"";
        if (i < parser.pacotesEncontrados.size() - 1) fout << ",";
        fout << endl;
    }
    fout << "    ]," << endl;

    fout << "    \"classes\": [" << endl;
    for (size_t i = 0; i < parser.classesEncontradas.size(); ++i) {
        const auto& classe = parser.classesEncontradas[i];
        fout << "      {" << endl;
        fout << R"(        "pacote": ")" << escapeJson(classe.pacote) << "\"," << endl;
        fout << R"(        "estereotipo": ")" << escapeJson(classe.estereotipo) << "\"," << endl;
        fout << R"(        "nome": ")" << escapeJson(classe.nome) << "\"," << endl;
        fout << R"(        "herdaDe": ")" << escapeJson(classe.herdaDe) << "\"," << endl;
        fout << "        \"atributos\": [" << endl;
        for (size_t j = 0; j < classe.atributos.size(); ++j) {
            const auto& attr = classe.atributos[j];
            fout << R"(          { "nome": ")" << escapeJson(attr.nome) << R"(", "tipo": ")" << escapeJson(attr.tipo) << R"(", "meta": ")" << escapeJson(attr.metaAtributo) << "\" }";
            if (j < classe.atributos.size() - 1) fout << ",";
            fout << endl;
        }
        fout << "        ]" << endl;
        fout << "      }";
        if (i < parser.classesEncontradas.size() - 1) fout << ",";
        fout << endl;
    }
    fout << "    ]," << endl;

    // (Adicione outras sinteses aqui: gensets, datatypes, enums, relacoes...)

    fout << "    \"generalizationSets\": [" << endl;
     for (size_t i = 0; i < parser.gensetsEncontrados.size(); ++i) {
        const auto&[nome, classeGeral, classesEspecificas, isDisjoint, isComplete] = parser.gensetsEncontrados[i];
        fout << "      {" << endl;
        fout << R"(        "nome": ")" << escapeJson(nome) << "\"," << endl;
        fout << R"(        "general": ")" << escapeJson(classeGeral) << "\"," << endl;
        fout << "        \"disjoint\": " << (isDisjoint ? "true" : "false") << "," << endl;
        fout << "        \"complete\": " << (isComplete ? "true" : "false") << "," << endl;
        fout << "        \"specifics\": [";
        for(size_t j=0; j < classesEspecificas.size(); ++j) {
            fout << "\"" << escapeJson(classesEspecificas[j]) << "\"";
            if (j < classesEspecificas.size() - 1) fout << ", ";
        }
        fout << "]" << endl;
        fout << "      }";
        if (i < parser.gensetsEncontrados.size() - 1) fout << ",";
        fout << endl;
     }
    fout << "    ]" << endl;

    fout << "  }," << endl;

    // --- 2. Relatorio de Erros ---
    fout << "  \"Relatorio de Erros\": [" << endl;
    for (size_t i = 0; i < parser.errosSintaticos.size(); ++i) {
        const auto& erro = parser.errosSintaticos[i];
        fout << "    {" << endl;
        fout << "      \"linha\": " << erro.linha << "," << endl;
        fout << "      \"coluna\": " << erro.coluna << "," << endl;
        fout << R"(      "mensagem": ")" << escapeJson(erro.mensagem) << "\"" << endl;
        fout << "    }";
        if (i < parser.errosSintaticos.size() - 1) fout << ",";
        fout << endl;
    }
    fout << "  ]" << endl;

    fout << "}" << endl;
    fout.close();
}