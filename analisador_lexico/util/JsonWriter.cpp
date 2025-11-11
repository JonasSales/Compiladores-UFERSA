#include "JsonWriter.h"
#include "analisador_lexico/entities/TokenAnalisys.h"

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
        fout << "    \"tipo\": \"" << tokenToString(token.getTokenType()) << "\"," << endl;
        fout << "    \"lexema\": \"" << escapeJson(token.getLexeme()) << "\"" << endl;
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
        int currentCount = it->second;

        fout << "    \"" << tokenToString(currentType) << "\": {" << endl;
        fout << "      \"count\": " << currentCount << "," << endl;
        fout << "      \"tokens\": [" << endl;

        auto listIt = result.uniqueLexemes.find(currentType);
        if (listIt != result.uniqueLexemes.end()) {
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