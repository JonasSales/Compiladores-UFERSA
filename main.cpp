#include <fstream>
#include <iostream>
#include <filesystem>
#include "FlexLexer.h"
#include "analisador_lexico/lexer_utils/lex_config.h"
#include "util/JsonWriter.h"
#include "parser.hpp"
#include "analisador_sintatico/Parser.h"

using namespace std;

Parser parserData;

map<int, Token>::iterator currentTokenIt;
map<int, Token>::iterator endTokenIt;

// --- ADAPTER (YYLEX) ---
yy::parser::symbol_type yylex() {
    if (currentTokenIt == endTokenIt) {
        return yy::parser::make_END_OF_FILE();
    }

    Token token = currentTokenIt->second;
    TokenType type = token.getTokenType();
    string lexeme = token.getLexeme();
    ++currentTokenIt;
    switch (type) {
        case TokenType::CLASS_NAME:
            return yy::parser::make_CLASS_NAME(lexeme);

        case TokenType::INSTANCE_NAME:
            return yy::parser::make_INSTANCE_NAME(lexeme);

        case TokenType::DATATYPE_NAME:
            return yy::parser::make_DATATYPE_NAME(lexeme);

        case TokenType::RELATION_NAME:
            return yy::parser::make_RELATION_NAME(lexeme);
        case TokenType::CLASS_STEREOTYPE:
            return yy::parser::make_CLASS_STEREOTYPE(lexeme);
        case TokenType::RELATION_STEREOTYPE:
            return yy::parser::make_RELATION_STEREOTYPE(lexeme);
        case TokenType::NATIVE_DATA_TYPE:
            return yy::parser::make_NATIVE_DATA_TYPE(lexeme);

        case TokenType::SYMBOL:
            if (lexeme == "{") return yy::parser::make_LBRACE();
            if (lexeme == "}") return yy::parser::make_RBRACE();
            if (lexeme == ":") return yy::parser::make_COLON();
            if (lexeme == ",") return yy::parser::make_COMMA();
            break;

        case TokenType::RESERVED_WORD:
            if (lexeme == "package") return yy::parser::make_PACKAGE();
            if (lexeme == "import") return yy::parser::make_IMPORT();
            if (lexeme == "genset") return yy::parser::make_GENSET();
            if (lexeme == "disjoint") return yy::parser::make_DISJOINT();
            if (lexeme == "complete") return yy::parser::make_COMPLETE();
            if (lexeme == "general") return yy::parser::make_GENERAL();
            if (lexeme == "specifics") return yy::parser::make_SPECIFICS();
            if (lexeme == "specializes") return yy::parser::make_SPECIALIZES();
            if (lexeme == "datatype") return yy::parser::make_DATATYPE();
            if (lexeme == "enum") return yy::parser::make_ENUM();
            if (lexeme == "relation") return yy::parser::make_RELATION();
            break;

        case TokenType::END_OF_FILE:
             return yy::parser::make_END_OF_FILE();

        default:
            break;
    }
    return yylex();
}

static void processarArquivo(const std::filesystem::path &input) {
    cout << "Processando " << input.string() << endl;

    // prepare output filenames in the same directory
    std::filesystem::path parent = input.parent_path();
    string base = input.stem().string();

    std::filesystem::path lexerAnalysis = parent / (base + "LexerAnalysis.json");
    std::filesystem::path lexerCount = parent / (base + "TokenCount.json");
    std::filesystem::path syntaxAnalysis = parent / (base + "SyntaxAnalysis.json");

    // limpar dados globais
    try {tokens.clear();} catch(...){}
    try {parserData = Parser();} catch(...){}

    // ------------ LEXER ------------
    ifstream fin(input.string());
    if (!fin) {
        cerr << "Não foi possível abrir o arquivo: " << input.string() << endl;
    }

    streambuf* old_cin = cin.rdbuf(fin.rdbuf());
    yyFlexLexer lexer;
    try {
        lexer.yylex();
    } catch(const std::exception e) {
        cerr << "Erro durante a análise léxica: " << e.what() << endl;
    }

    cin.rdbuf(old_cin);
    fin.close();

    // escrever arquivos JSON
    try { writeTokenList(lexerAnalysis.string(), tokens); }
    catch (const std::exception &e) {
        cerr << "Erro ao salvar token list em " << lexerAnalysis << ": " << e.what() << endl;
    }

    try { writeTokenAnalysis(lexerCount.string(), tokenAnalisys); }
    catch (const std::exception &e) {
        cerr << "Erro ao salvar token analysis em " << lexerCount << ": " << e.what() << endl;
    }

    cout << "Analise lexica concluida para " << base << endl;

    // ------------ PARSER ------------
    currentTokenIt = tokens.begin();
    endTokenIt = tokens.end();

    cout << "Iniciando analise sintatica com Bison para " << base << "..." << endl;
    yy::parser parser;

    try { parser.parse(); }
    catch (const std::exception &e) {
        cerr << "Erro durante analise sintatica: " << e.what() << endl;
    }

    try { writeSyntaxAnalysis(syntaxAnalysis.string(), parserData); }
    catch (const std::exception &e) {
        cerr << "Erro ao salvar analise sintatica em " << syntaxAnalysis << ": " << e.what() << endl;
    }

    cout << "Analise sintatica salva em " << syntaxAnalysis << endl;
}

int main(int argc, char* argv[]) {

    // Se o usuário passar um arquivo específico, processa só ele
    if (argc >= 2) {
        std::filesystem::path arquivoUnico(argv[1]);
        if (!std::filesystem::exists(arquivoUnico)) {
            cerr << "Arquivo informado nao existe: " << arquivoUnico << endl;
            return 1;
        }
        processarArquivo(arquivoUnico);
        return 0;
        }

    // Caso contrário, processa TODOS os arquivos .tonto da pasta testes/
    std::filesystem::path testes = "../testes";
    if (!std::filesystem::exists(testes) || !std::filesystem::is_directory(testes)) {
        cerr << "pasta \'" << testes << "\' não encontrada! " << endl;
        return 1;
    }

    if (std::filesystem::is_empty(testes)) {
        cerr << "Pasta \'" << testes << "\' vazia! " << endl;
        return 1;
    }

    for (auto &entry : std::filesystem::recursive_directory_iterator(testes)) {
        if (!entry.is_regular_file()) continue;
        if (entry.path().extension() == ".tonto") {
            processarArquivo(entry.path());
        }
    }

    cout << "Processamento concluido." << endl;
    return 0;
    }