#include <fstream>
#include <iostream>
#include "FlexLexer.h"

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

    string outname = string(argv[1]) + ".json";

    ofstream out(outname);
    if (!out) {
        cerr << "Não foi possível criar o arquivo de saída." << endl;
        return 1;
    }

    streambuf* old_cin = cin.rdbuf(in.rdbuf());
    streambuf* old_cout = cout.rdbuf(out.rdbuf());

    out << "[" << endl;

    yyFlexLexer lexer;
    lexer.yylex();

    out << endl << "]" << endl;

    cin.rdbuf(old_cin);
    cout.rdbuf(old_cout);

    return 0;
}