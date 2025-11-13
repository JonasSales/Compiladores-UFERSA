#include <fstream>
#include <iostream>
#include <filesystem>
#include "FlexLexer.h"
#include "analisador_lexico/lexer_utils/lex_config.h"
#include "analisador_lexico/util/JsonWriter.h"
#include "analisador_sintatico/Parser.h"

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
        cerr << "Nao foi possivel abrir o arquivo de entrada." << endl;
        return 1;
    }

    // Cria a pasta "teste" se não existir
    std::filesystem::create_directory("testes");

    // Nome dos arquivos de saída dentro da pasta "teste"
    string filename = std::filesystem::path(argv[1]).filename().string();

    //Nome do arquivo para a lista de Tokens
    string listOutName = "testes/lexerTokenAnalisys.json";
    string analisysOutName = "testes/lexerTokenCount.json";
    // --- SAIDA DO NOVO ANALISADOR SINTATICO ---
    string syntaxOutName = "testes/syntaxAnalisys.json"; // <-- 2. NOVO ARQUIVO DE SAIDA SINTATICO

    streambuf* old_cin = cin.rdbuf(in.rdbuf());

    yyFlexLexer lexer;
    lexer.yylex(); // <-- O LEXICO RODA E POPULA O MAPA 'tokens'

    cin.rdbuf(old_cin);

    // Escreve a saída do léxico (como antes)
    writeTokenList(listOutName, tokens);
    writeTokenAnalysis(analisysOutName, tokenAnalisys);
    cout << "Analise lexica salva em " << listOutName << " e " << analisysOutName << endl;

    // --- 3. EXECUTAR O ANALISADOR SINTATICO ---
    cout << "Iniciando analise sintatica..." << endl;
    Parser parser(tokens); // <-- O parser e alimentado com a saida do lexico
    parser.parse();        // <-- O parser consome os tokens

    // --- 4. ESCREVER A SAIDA SINTATICA ---
    writeSyntaxAnalysis(syntaxOutName, parser);
    cout << "Analise sintatica (Tabela de Sintese e Erros) salva em " << syntaxOutName << endl;

    return 0;
}