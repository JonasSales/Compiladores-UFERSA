#include <fstream>
#include <iostream>
#include <filesystem>
#include <map>
#include <string>

#include "FlexLexer.h"
#include "analisador_lexico/lexer_utils/lex_config.h"
#include "util/JsonWriter.h"
#include "parser.hpp"                     // gerado pelo bison (build/parser.hpp)
#include "analisador_sintatico/Parser.h"  // sua struct Parser (parserData)

using namespace std;

// Declarações/variáveis globais
extern std::map<int, Token> tokens;
extern TokenAnalisys tokenAnalisys;
extern Parser parserData;

/* Iteradores usados pelo adaptador yylex() */
extern std::map<int, Token>::iterator currentTokenIt;
extern std::map<int, Token>::iterator endTokenIt;


/* --- ADAPTER (YYLEX) ---*/
yy::parser::symbol_type yylex() {
    yy::parser::location_type loc;

    if (currentTokenIt != endTokenIt) {
        const Token &t = currentTokenIt->second;
        loc.begin.line = t.getLine();
        loc.begin.column = t.getColumn();
        loc.end = loc.begin;
    }

    if (currentTokenIt == endTokenIt) {
        return yy::parser::make_END_OF_FILE(loc);
    }

    Token token = currentTokenIt->second;
    TokenType type = token.getTokenType();
    string lexeme = token.getLexeme();
    ++currentTokenIt;

    switch (type) {
        /* Tokens com payload string -> passe lexeme + loc */
        case TokenType::CLASS_NAME:
            return yy::parser::make_CLASS_NAME(lexeme, loc);

        case TokenType::INSTANCE_NAME:
            return yy::parser::make_INSTANCE_NAME(lexeme, loc);

        case TokenType::DATATYPE_NAME:
            return yy::parser::make_DATATYPE_NAME(lexeme, loc);

        case TokenType::RELATION_NAME:
            return yy::parser::make_RELATION_NAME(lexeme, loc);

        case TokenType::CLASS_STEREOTYPE:
            return yy::parser::make_CLASS_STEREOTYPE(lexeme, loc);

        case TokenType::RELATION_STEREOTYPE:
            return yy::parser::make_RELATION_STEREOTYPE(lexeme, loc);

        case TokenType::NATIVE_DATA_TYPE:
            return yy::parser::make_NATIVE_DATA_TYPE(lexeme, loc);

        /* --- NOVOS TOKENS (Essenciais para Car.tonto) --- */
        case TokenType::CARDINALITY:
            return yy::parser::make_CARDINALITY(lexeme, loc);

        case TokenType::RELATION: // Símbolos como --, <>, ..
            return yy::parser::make_RELATION_OP(lexeme, loc);
        /* ------------------------------------------------ */

        /* Símbolos sem payload -> apenas loc */
        case TokenType::SYMBOL:
            if (lexeme == "{") return yy::parser::make_LBRACE(loc);
            if (lexeme == "}") return yy::parser::make_RBRACE(loc);
            if (lexeme == ":") return yy::parser::make_COLON(loc);
            if (lexeme == ",") return yy::parser::make_COMMA(loc);
            break;

        /* Palavras reservadas */
        case TokenType::RESERVED_WORD:
            if (lexeme == "package") return yy::parser::make_PACKAGE(loc);
            if (lexeme == "import") return yy::parser::make_IMPORT(loc);
            if (lexeme == "genset") return yy::parser::make_GENSET(loc);

            /* Mapeia disjoint/complete para o token DISJOINT_COMPLETE */
            if (lexeme == "disjoint")
                return yy::parser::make_DISJOINT_COMPLETE(std::string("disjoint"), loc);
            if (lexeme == "complete")
                return yy::parser::make_DISJOINT_COMPLETE(std::string("complete"), loc);
            if (lexeme == "disjoint_complete")
                return yy::parser::make_DISJOINT_COMPLETE(std::string("disjoint_complete"), loc);

            if (lexeme == "general") return yy::parser::make_GENERAL(loc);
            if (lexeme == "specifics") return yy::parser::make_SPECIFICS(loc);
            if (lexeme == "specializes") return yy::parser::make_SPECIALIZES(loc);
            if (lexeme == "datatype") return yy::parser::make_DATATYPE(loc);
            if (lexeme == "enum") return yy::parser::make_ENUM(loc);
            if (lexeme == "relation") return yy::parser::make_RELATION(loc);
            break;

        case TokenType::END_OF_FILE:
            return yy::parser::make_END_OF_FILE(loc);

        default:
            break;
    }

    // Ignora token desconhecido e tenta o próximo
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
    try { tokens.clear(); } catch(...) {}
    try { parserData = Parser(); } catch(...) {}

    // ------------ LEXER ------------
    ifstream fin(input.string());
    if (!fin) {
        cerr << "Não foi possível abrir o arquivo: " << input.string() << endl;
        return;
    }

    streambuf* old_cin = cin.rdbuf(fin.rdbuf());
    yyFlexLexer lexer;
    try {
        lexer.yylex();
    } catch(const std::exception &e) {
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

    cout << "Analise sintatica salva em " << syntaxAnalysis << endl << endl;
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