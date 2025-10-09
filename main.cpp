#include <fstream>
#include <iostream>
#include <filesystem>
#include "FlexLexer.h"
#include "analisador_lexico/lexer_utils/lex_config.h"
#include "analisador_lexico/util/JsonWriter.h"

using std::ifstream;
using std::ofstream;
using std::string;
using std::endl;
using std::cerr;
using std::streambuf;
using std::cin;
using std::cout;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Uso: " << argv[0] << " teste.tonto" << endl;
        return 1;
    }

    ifstream in(argv[1]);
    if (!in) {
        cerr << "Não foi possível abrir o arquivo de entrada." << endl;
        return 1;
    }

    // Cria a pasta "teste" se não existir
    std::filesystem::create_directory("testes");

    // Nome do arquivo de saída dentro da pasta "teste"
    std::string filename = std::filesystem::path(argv[1]).filename().string();
    string outname = "testes/" + filename + ".json";

    ofstream out(outname);
    if (!out) {
        cerr << "Não foi possível criar o arquivo de saída." << endl;
        return 1;
    }

    streambuf* old_cin = cin.rdbuf(in.rdbuf());

    yyFlexLexer lexer;
    lexer.yylex();
    out << "[" << endl;

    for (size_t i = 0; i < tokens.size(); ++i) {
        out << "  {" << endl;
        out << "    \"id\": " << tokens[i].getContador() << "," << endl;
        out << "    \"linha\": " << tokens[i].getLine() << "," << endl;
        out << "    \"coluna\": " << tokens[i].getColumn() << "," << endl;
        out << "    \"tipo\": \"" << token_to_string(tokens[i].getTokenType()) << "\"," << endl;
        out << "    \"lexema\": \"" << escape_json(tokens[i].getLexeme()) << "\"" << endl;
        out << "  }";
        if (i < tokens.size() - 1) {
            out << ",";
        }
        out << endl;
    }

    out << "]" << endl;

    // Restaura o cin
    cin.rdbuf(old_cin);

    return 0;
}