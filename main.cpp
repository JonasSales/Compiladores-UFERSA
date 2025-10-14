#include <fstream>
#include <iostream>
#include <filesystem>
#include "FlexLexer.h"
#include "analisador_lexico/lexer_utils/lex_config.h"
#include "analisador_lexico/util/JsonWriter.h"
#include "analisador_lexico/entities/TokenAnalisys.h"

using std::ifstream;
using std::ofstream;
using std::string;
using std::endl;
using std::cerr;
using std::streambuf;
using std::cin;
using std::cout;

class yyFlexLexer;

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

    // Nome dos arquivos de saída dentro da pasta "teste"
    string filename = std::filesystem::path(argv[1]).filename().string();
    //Nome do arquivo para a lista de Tokens
    string listOutName = "testes/" + filename + "_list.json";
    string analisysOutName = "testes/" + filename + "_analisys.json";

    streambuf* old_cin = cin.rdbuf(in.rdbuf());

    yyFlexLexer lexer;
    lexer.yylex();

    cin.rdbuf(old_cin);

    writeTokenList(listOutName, tokens);
    writeTokenAnalysis(analisysOutName, tokenAnalisys);

    return 0;
}