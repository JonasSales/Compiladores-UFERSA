#ifndef COMPILADORESUFERSA_JSONWRITER_H
#define COMPILADORESUFERSA_JSONWRITER_H
#include <fstream>
#include <iostream>
#include <filesystem>

using std::ifstream;
using std::ofstream;
using std::string;
using std::endl;
using std::cerr;
using std::streambuf;
using std::cin;
using std::cout;

string escape_json(const string& s);
#endif //COMPILADORESUFERSA_JSONWRITER_H