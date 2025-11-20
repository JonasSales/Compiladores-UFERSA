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

    std::filesystem::create_directory("testes");
    string listOutName = "testes/lexerTokenAnalisys.json";
    string analisysOutName = "testes/lexerTokenCount.json";
    string syntaxOutName = "testes/syntaxAnalisys.json";

    // 1. Lexer
    streambuf* old_cin = cin.rdbuf(in.rdbuf());
    yyFlexLexer lexer;
    lexer.yylex();
    cin.rdbuf(old_cin);

    writeTokenList(listOutName, tokens);
    writeTokenAnalysis(analisysOutName, tokenAnalisys);
    cout << "Analise lexica concluida." << endl;

    // 2. Bison
    currentTokenIt = tokens.begin();
    endTokenIt = tokens.end();

    cout << "Iniciando analise sintatica com Bison..." << endl;
    yy::parser parser;
    parser.parse();

    writeSyntaxAnalysis(syntaxOutName, parserData);
    cout << "Analise sintatica salva em " << syntaxOutName << endl;

    return 0;
}