#ifndef COMPILADORESUFERSA_JSONWRITER_H
#define COMPILADORESUFERSA_JSONWRITER_H

#include <fstream>
#include <iostream>
#include <filesystem>
#include "analisador_lexico/lexer_utils/lex_config.h"

class TokenAnalisys;
class Parser;
using std::ifstream;
using std::ofstream;
using std::string;
using std::endl;
using std::cerr;
using std::streambuf;
using std::cin;
using std::cout;

string escapeJson(const string& s);

void writeTokenAnalysis(const string& filename, const TokenAnalisys& result);
void writeTokenList(const string &filename, const map<int, Token> &tokens);
void writeSyntaxAnalysis(const string& filename, const Parser& parser);

#endif //COMPILADORESUFERSA_JSONWRITER_H