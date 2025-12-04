#include <fstream>
#include <iostream>
#include <filesystem>
#include <map>
#include <string>
#include <vector>

#include "FlexLexer.h"
#include "analisador_lexico/lexer_utils/lex_config.h"
#include "util/JsonWriter.h"
#include "parser.hpp"
#include "analisador_semantico/SemanticAnalyzer.h"
#include "analisador_sintatico/Parser.h"

using namespace std;

extern std::map<int, Token> tokens;
extern TokenAnalisys tokenAnalisys;
extern Parser parserData;
extern std::map<int, Token>::iterator currentTokenIt;
extern std::map<int, Token>::iterator endTokenIt;

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

    parserData.ultimoLexema = lexeme;
    ++currentTokenIt;

    switch (type) {
        case TokenType::CLASS_NAME: return yy::parser::make_CLASS_NAME(lexeme, loc);
        case TokenType::INSTANCE_NAME: return yy::parser::make_INSTANCE_NAME(lexeme, loc);
        case TokenType::DATATYPE_NAME: return yy::parser::make_DATATYPE_NAME(lexeme, loc);
        case TokenType::RELATION_NAME: return yy::parser::make_RELATION_NAME(lexeme, loc);
        case TokenType::CLASS_STEREOTYPE: return yy::parser::make_CLASS_STEREOTYPE(lexeme, loc);
        case TokenType::RELATION_STEREOTYPE: return yy::parser::make_RELATION_STEREOTYPE(lexeme, loc);
        case TokenType::NATIVE_DATA_TYPE: return yy::parser::make_NATIVE_DATA_TYPE(lexeme, loc);
        case TokenType::CARDINALITY: return yy::parser::make_CARDINALITY(lexeme, loc);
        case TokenType::RELATION: return yy::parser::make_RELATION_OP(lexeme, loc);
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
            if (lexeme == "disjoint") return yy::parser::make_DISJOINT(loc);
            if (lexeme == "complete") return yy::parser::make_COMPLETE(loc);
            if (lexeme == "disjoint_complete") return yy::parser::make_DISJOINT(loc);
            if (lexeme == "general") return yy::parser::make_GENERAL(loc);
            if (lexeme == "specifics") return yy::parser::make_SPECIFICS(loc);
            if (lexeme == "specializes") return yy::parser::make_SPECIALIZES(loc);
            if (lexeme == "datatype") return yy::parser::make_DATATYPE(loc);
            if (lexeme == "enum") return yy::parser::make_ENUM(loc);
            if (lexeme == "relation") return yy::parser::make_RELATION(loc);
            break;
        case TokenType::END_OF_FILE: return yy::parser::make_END_OF_FILE(loc);
        default: break;
    }
    return yylex();
}

static void processarArquivo(const std::filesystem::path &input) {
    std::filesystem::path parent = input.parent_path();
    string base = input.stem().string();

    std::filesystem::path lexerAnalysis = parent / (base + "LexerAnalysis.json");
    std::filesystem::path syntaxAnalysis = parent / (base + "SyntaxAnalysis.json");

    parserData.pacoteAtual = "default";
    parserData.importsTemporarios.clear();

    try { tokens.clear(); } catch(...) {}

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

    cout << "Analise lexica salva em " << lexerAnalysis.string() << endl;

    currentTokenIt = tokens.begin();
    endTokenIt = tokens.end();

    cout << "Iniciando analise sintatica com Bison para " << base << ".tonto" << endl;

    size_t qtdErrosAntes = parserData.errosSintaticos.size();

    yy::parser parser;
    try {
        parser.parse();
    } catch (const std::exception &e) {
        cerr << "\033[1;31m" << "Erro durante analise sintatica: " << e.what() << "\033[0m" << endl;
    }

    size_t qtdErrosDepois = parserData.errosSintaticos.size();
    for (size_t i = qtdErrosAntes; i < qtdErrosDepois; i++) {
        parserData.errosSintaticos[i].mensagem =
            "["+ base + ".tonto] " + parserData.errosSintaticos[i].mensagem;
    }

    try { writeSyntaxAnalysis(syntaxAnalysis.string(), parserData); }
    catch (const std::exception &e) {
        cerr << "Erro ao salvar analise sintatica em " << syntaxAnalysis << ": " << e.what() << endl;
    }
    cout << "Analise sintatica salva em " << syntaxAnalysis.string() << endl << endl;

}

int main(int argc, char* argv[]) {
    parserData = Parser();
    std::vector<std::filesystem::path> arquivosParaProcessar;

    if (argc >= 2) {
        std::filesystem::path arquivoUnico(argv[1]);
        if (std::filesystem::exists(arquivoUnico)) {
            arquivosParaProcessar.push_back(arquivoUnico);
        } else {
             cerr << "\033[1;31m" << "[ERRO] Arquivo informado não existe: " << arquivoUnico << "\033[0m" << endl;
             return 1;
        }
    } else {
        std::filesystem::path testes = "../testes";
        if (std::filesystem::exists(testes)) {
            for (auto &entry : std::filesystem::recursive_directory_iterator(testes)) {
                if (entry.is_regular_file() && entry.path().extension() == ".tonto") {
                    arquivosParaProcessar.push_back(entry.path());
                }
            }
        } else {
            cerr << "\033[1;31m" << "[AVISO] Pasta '../testes' não encontrada!" << "\033[0m" << endl;
            return 1;
        }
    }

    if (arquivosParaProcessar.empty()) {
        cerr << "\033[1;33m" << "[AVISO] Nenhum arquivo .tonto encontrado para processar." << "\033[0m" << endl;
        return 0;
    }

    for (const auto& arq : arquivosParaProcessar) {
        processarArquivo(arq);
    }

    bool temErrosSintaticos = !parserData.errosSintaticos.empty();

    if (temErrosSintaticos) {
        cout << "\033[1;31m" << "[ERROS SINTÁTICOS]" << "\033[0m" << endl;
        for (const auto& erro : parserData.errosSintaticos) {
            cout << "  - [Linha " << erro.linha << ", Coluna " << erro.coluna << "] " << erro.mensagem << endl;
        }
    }

    SemanticAnalyzer semantico;
    bool semanticoValido = semantico.analisar(parserData);
    bool temErrosSemanticos = !semanticoValido;

    if (temErrosSemanticos) {
        if (temErrosSintaticos) cout << endl;
        cout << "\033[1;31m" << "[ERROS SEMÂNTICOS]" << "\033[0m" << endl;
        const auto& erros = semantico.getErros();
        for (const auto& erro : erros) {
             cout << "  - " << erro.mensagem << endl;
        }
    }

    if (!temErrosSintaticos && !temErrosSemanticos) {
        cout << "\033[1;32m" << "[SUCESSO] O projeto esta consistente (Sintaxe e Semantica OK)." << "\033[0m" << endl;
    } else {
        cout << "\n\033[1;31m" << "[FALHA] O projeto contém erros (Sintáticos: "
             << parserData.errosSintaticos.size() << ", Semânticos: "
             << semantico.getErros().size() << ")." << "\033[0m" << endl;
    }

    cout << "\n========================================" << endl;
    cout << "      RESUMO GLOBAL DO PROJETO          " << endl;
    cout << "========================================" << endl;
    cout << "Classes/Stereotypes:   " << parserData.classesEncontradas.size() << endl;
    cout << "Gensets (Disjunções):  " << parserData.gensetsEncontrados.size() << endl;
    cout << "Enums:                 " << parserData.enumsEncontrados.size() << endl;
    cout << "Relações Externas:     " << parserData.relacoesExternasEncontradas.size() << endl;
    cout << "Erros Sintáticos:      " << parserData.errosSintaticos.size() << endl;
    cout << "========================================\n" << endl;

    return 0;
}