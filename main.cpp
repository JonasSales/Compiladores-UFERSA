#include <fstream>
#include <iostream>
#include "FlexLexer.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " arquivo.tonto" << std::endl;
        return 1;
    }

    std::ifstream in(argv[1]);
    if (!in) {
        std::cerr << "Não foi possível abrir o arquivo de entrada." << std::endl;
        return 1;
    }

    std::string outname = std::string(argv[1]) + ".tok";
    std::ofstream out(outname);
    if (!out) {
        std::cerr << "Não foi possível criar o arquivo de saída." << std::endl;
        return 1;
    }

    std::streambuf* old_cin = std::cin.rdbuf(in.rdbuf());
    std::streambuf* old_cout = std::cout.rdbuf(out.rdbuf());

    yyFlexLexer lexer;
    lexer.yylex();

    std::cin.rdbuf(old_cin);
    std::cout.rdbuf(old_cout);

    return 0;
}