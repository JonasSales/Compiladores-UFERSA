#include <fstream>
#include <iostream>
#include <filesystem>
#include <vector>
#include "FlexLexer.h"
#include "analisador_lexico/lexer_utils/lex_config.h"
#include "util/JsonWriter.h"

#include "parser.hpp"

using namespace std;


// Iterador para percorrer o mapa de tokens gerado pelo Lexer
map<int, Token>::iterator currentTokenIt;
map<int, Token>::iterator endTokenIt;

// --- IMPLEMENTAÇÃO DO YYLEX (O ADAPTER) ---
// Esta função é chamada automaticamente pelo parser.parse()
yy::parser::symbol_type yylex() {
    if (currentTokenIt == endTokenIt) {
        return yy::parser::make_END_OF_FILE();
    }

    Token token = currentTokenIt->second;
    TokenType type = token.getTokenType();
    string lexeme = token.getLexeme();

    // Avança o iterador para a próxima chamada
    currentTokenIt++;

    // Mapeamento: TokenType (Seu Enum) -> Token do Bison (parser.y)
    switch (type) {
        case TokenType::CLASS_NAME:
            return yy::parser::make_CLASS_NAME(lexeme);
        case TokenType::RELATION_NAME:
            return yy::parser::make_RELATION_NAME(lexeme);
        case TokenType::CLASS_STEREOTYPE:
            return yy::parser::make_CLASS_STEREOTYPE(lexeme);
        case TokenType::NATIVE_DATA_TYPE:
            return yy::parser::make_NATIVE_DATA_TYPE(lexeme);
        case TokenType::SYMBOL:
            if (lexeme == "{") return yy::parser::make_LBRACE();
            if (lexeme == "}") return yy::parser::make_RBRACE();
            if (lexeme == ":") return yy::parser::make_COLON();
            if (lexeme == ",") return yy::parser::make_COMMA();
            break;

        case TokenType::RESERVED_WORD:
            // O seu Lexer agrupa tudo como RESERVED_WORD. O Bison precisa saber qual é.
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

        // Adicione outros casos conforme necessário (NUMBER, STRING, etc)
        default:
            break;
    }

    // Se não reconheceu, retorna EOF ou lança erro (aqui retornando EOF por segurança)
    return yy::parser::make_END_OF_FILE();
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

    // 1. EXECUÇÃO DO LEXER (Fase Sequencial 1)
    // Mantém a sua lógica original: roda o Flex, popula o map 'tokens'
    streambuf* old_cin = cin.rdbuf(in.rdbuf());
    yyFlexLexer lexer;
    lexer.yylex();
    cin.rdbuf(old_cin);

    // Gera os JSONs (Requisito mantido)
    writeTokenList(listOutName, tokens);
    writeTokenAnalysis(analisysOutName, tokenAnalisys);
    cout << "Analise lexica concluida." << endl;

    // 2. PREPARAÇÃO PARA O PARSER (A Ponte)
    // Inicializa os iteradores que o yylex() vai usar
    currentTokenIt = tokens.begin();
    endTokenIt = tokens.end();

    // 3. EXECUÇÃO DO PARSER BISON (Fase Sequencial 2)
    cout << "Iniciando analise sintatica com Bison..." << endl;
    yy::parser parser;
    int res = parser.parse();

    if (res == 0) {
        cout << "Analise sintatica (Bison) concluida com sucesso!" << endl;
    } else {
        cerr << "Falha na analise sintatica." << endl;
    }

    return 0;
}