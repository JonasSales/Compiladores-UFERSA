#include <fstream>
#include <iostream>
#include <filesystem>
#include <map>
#include <string>

#include "FlexLexer.h"
#include "analisador_lexico/lexer_utils/lex_config.h"
#include "util/JsonWriter.h"
#include "parser.hpp"                    
#include "analisador_sintatico/Parser.h"  

using namespace std;

extern std::map<int, Token> tokens;
extern TokenAnalisys tokenAnalisys;
extern Parser parserData;

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
        parserData.ultimoLexema = "EOF";
        return yy::parser::make_END_OF_FILE(loc);
    }

    Token token = currentTokenIt->second;
    TokenType type = token.getTokenType();
    string lexeme = token.getLexeme();

     --- SALVANDO O LEXEMA ATUAL ---
    parserData.ultimoLexema = lexeme;

    ++currentTokenIt;
    switch (type) {
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

        case TokenType::CARDINALITY:
            return yy::parser::make_CARDINALITY(lexeme, loc);
        case TokenType::RELATION:
            return yy::parser::make_RELATION_OP(lexeme, loc);

        case TokenType::SYMBOL:
            if (lexeme == "{") return yy::parser::make_LBRACE(loc);
            if (lexeme == "}") return yy::parser::make_RBRACE(loc);
            if (lexeme == ":") return yy::parser::make_COLON(loc);
            if (lexeme == ",") return yy::parser::make_COMMA(loc);
            break;

        case TokenType::RESERVED_WORD:
            if (lexeme == "package") return yy::parser::make_PACKAGE(loc);
            if (lexeme == "import") return yy::parser::make_IMPORT(loc);
            if (lexeme == "genset") return yy::parser::make_GENSET(loc);

            /* ATUALIZAÇÃO: Mapeamento para os novos tokens separados */
            if (lexeme == "disjoint") return yy::parser::make_DISJOINT(loc);
            if (lexeme == "complete") return yy::parser::make_COMPLETE(loc);

            if (lexeme == "disjoint_complete") {
                 return yy::parser::make_DISJOINT(loc);
            }

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

    return yylex();
}

static void processarArquivo(const std::filesystem::path &input) {
    std::filesystem::path parent = input.parent_path();
    string base = input.stem().string();

    std::filesystem::path lexerAnalysis = parent / (base + "LexerAnalysis.json");
    std::filesystem::path lexerCount = parent / (base + "TokenCount.json");
    std::filesystem::path syntaxAnalysis = parent / (base + "SyntaxAnalysis.json");

    try { tokens.clear(); } catch(...) {}
    try { parserData = Parser(); } catch(...) {}

    ifstream fin(input.string());
    if (!fin) {
        cerr << "\033[1;31m" << "Não foi possível abrir o arquivo: " << input.string() << "\033[0m" << endl;
        return;
    }

    cout << "Iniciando analise lexica com Flex para " << base << ".tonto" << endl;

    streambuf* old_cin = cin.rdbuf(fin.rdbuf());
    yyFlexLexer lexer;
    try {
        lexer.yylex();
    } catch(const std::exception &e) {
        cerr << "\033[1;31m" << "Erro durante a análise léxica: " << e.what() << "\033[0m" << endl;
    }

    cin.rdbuf(old_cin);
    fin.close();

    try { writeTokenList(lexerAnalysis.string(), tokens); }
    catch (const std::exception &e) {
        cerr << "Erro ao salvar token list em " << lexerAnalysis << ": " << e.what() << endl;
    }

    cout << "Analise lexica salva em " << lexerAnalysis << endl;

    currentTokenIt = tokens.begin();
    endTokenIt = tokens.end();

    cout << "Iniciando analise sintatica com Bison para " << base << ".tonto" << endl;
    yy::parser parser;

    try {
        parser.parse();
    } catch (const std::exception &e) {
        cerr << "\033[1;31m" << "Erro durante analise sintatica: " << e.what() << "\033[0m" << endl;
    }

    try { writeSyntaxAnalysis(syntaxAnalysis.string(), parserData); }
    catch (const std::exception &e) {
        cerr << "Erro ao salvar analise sintatica em " << syntaxAnalysis << ": " << e.what() << endl;
    }

    try {
        parser.parse();
    } catch (const std::exception &e) {
        cerr << "\033[1;31m" << "Erro durante analise sintatica: " << e.what() << "\033[0m" << endl;
    }

    try { writeSyntaxAnalysis(syntaxAnalysis.string(), parserData); }
    catch (const std::exception &e) {
        cerr << "Erro ao salvar analise sintatica em " << syntaxAnalysis << ": " << e.what() << endl;
    }

    cout << "Analise sintatica salva em " << syntaxAnalysis << endl;

    cout << "\n========================================" << endl;
    cout << "      RESUMO DA ANÁLISE SINTÁTICA       " << endl;
    cout << "========================================" << endl;
    cout << "Pacotes encontrados:   " << parserData.pacotesEncontrados.size() << endl;
    cout << "Classes/Stereotypes:   " << parserData.classesEncontradas.size() << endl;
    cout << "Gensets (Disjunções):  " << parserData.gensetsEncontrados.size() << endl;
    cout << "Datatypes Custom:      " << parserData.datatypesEncontrados.size() << endl;
    cout << "Enums:                 " << parserData.enumsEncontrados.size() << endl;
    cout << "Relações Externas:     " << parserData.relacoesExternasEncontradas.size() << endl;

    if (!parserData.errosSintaticos.empty()) {
        cout << "\033[1;31m" << "Erros Sintáticos:      " << parserData.errosSintaticos.size() << "\033[0m" << endl;
    } else {
        cout << "\033[1;32m" << "Erros Sintáticos:      0" << "\033[0m" << endl;
    }
    cout << "========================================\n" << endl;
}

int main(int argc, char* argv[]) {

    if (argc >= 2) {
        std::filesystem::path arquivoUnico(argv[1]);
        if (!std::filesystem::exists(arquivoUnico)) {
            cerr << "Arquivo informado nao existe: " << arquivoUnico << endl;
            return 1;
        }
        processarArquivo(arquivoUnico);
        return 0;
    }

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
