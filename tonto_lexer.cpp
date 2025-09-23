#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <utility>

enum class TokenType {
    TK_EOF,
    TK_IDENTIFIER,
    TK_KEYWORD,
    TK_NUMBER,
    TK_STRING,
    TK_SYMBOL,
    TK_OPERATOR,
    TK_COMMENT,
    TK_UNKNOWN,
    TK_ERROR
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int col;
};

static const char* KEYWORDS_ARR[] = {
    "package","import","class","kind","collective","quantity","quality","mode",
    "intrinsicMode","extrinsicMode","relator","type","powertype","subkind","phase",
    "role","historicalRole","event","situation","category","mixin","phaseMixin",
    "roleMixin","historicalRoleMixin","genset","where","specializes","ordered","const",
    "derived","relation","instanceOf","as","enum","datatype","name","ordered"
};

bool is_keyword(const std::string &s) {
    for (auto & i : KEYWORDS_ARR) if (s == i) return true;
    return false;
}

class Lexer {
    std::string src;
    size_t i;
    int line, col;
public:
    explicit Lexer(std::string text): src(std::move(text)), i(0), line(1), col(1) {}
    [[nodiscard]] char peek() const { return i < src.size() ? src[i] : '\0'; }
    char get() {
        const char c = peek();
        if (c == '\0') return '\0';
        i++;
        if (c == '\n') { line++; col = 1; } else col++;
        return c;
    }
    void skip_whitespace_and_comments() {
        while (true) {
            const char c = peek();
            if (c == '\0') return;
            if (isspace(static_cast<unsigned char>(c))) { get(); continue; }
            if (c == '/' && i+1 < src.size() && src[i+1] == '/') {
                get(); get();
                while (peek() != '\0' && peek() != '\n') get();
                continue;
            }
            if (c == '/' && i+1 < src.size() && src[i+1] == '*') {
                get(); get();
                while (true) {
                    const char ch = get();
                    if (ch == '\0') return;
                    if (ch == '*' && peek() == '/') { get(); break; }
                }
                continue;
            }
            break;
        }
    }

    Token nextToken() {
        skip_whitespace_and_comments();
        Token tk; tk.line = line; tk.col = col;
        char c = peek();
        if (c == '\0') { tk.type = TokenType::TK_EOF; tk.lexeme=""; return tk; }

        if (isalpha(static_cast<unsigned char>(c)) || c == '_') {
            std::string s;
            int startCol = col;
            while (isalnum(static_cast<unsigned char>(peek())) || peek() == '_' || peek() == '.') {
                s.push_back(get());
            }
            tk.lexeme = s;
            tk.type = is_keyword(s) ? TokenType::TK_KEYWORD : TokenType::TK_IDENTIFIER;
            tk.col = startCol;
            return tk;
        }

        if (isdigit(static_cast<unsigned char>(c))) {
            std::string s;
            int startCol = col;
            while (isdigit(static_cast<unsigned char>(peek()))) s.push_back(get());
            if (peek() == '.') {
                s.push_back(get());
                while (isdigit(static_cast<unsigned char>(peek()))) s.push_back(get());
            }
            tk.type = TokenType::TK_NUMBER;
            tk.lexeme = s;
            tk.col = startCol;
            return tk;
        }

        if (c == '"') {
            int startCol = col;
            get();
            std::string s;
            bool closed = false;
            while (true) {
                char ch = peek();
                if (ch == '\0') {
                    tk.type = TokenType::TK_ERROR;
                    tk.lexeme = "Unterminated string";
                    tk.col = startCol;
                    return tk;
                }
                if (ch == '"') { get(); closed = true; break; }
                if (ch == '\\') {
                    get();
                    char esc = get();
                    if (esc == 'n') s.push_back('\n');
                    else s.push_back(esc);
                } else s.push_back(get());
            }
            if (!closed) {
                tk.type = TokenType::TK_ERROR;
                tk.lexeme = "Unterminated string";
            } else {
                tk.type = TokenType::TK_STRING;
                tk.lexeme = s;
            }
            tk.col = startCol;
            return tk;
        }

        if (c == '<') {
            int startCol = col;
            std::string temp;
            temp.push_back(get());
            if (peek() == 'o') { temp.push_back(get()); if (peek() == '>') { temp.push_back(get()); if (peek() == '-') {
                temp.push_back(get());
                if (peek() == '-') { temp.push_back(get()); tk.type = TokenType::TK_OPERATOR; tk.lexeme = temp; tk.col=startCol; return tk; }
            }}}
            if (peek() == '>') {
                temp.push_back(get());
                if (peek() == '-') {
                    temp.push_back(get());
                    if (peek() == '-') { temp.push_back(get()); tk.type = TokenType::TK_OPERATOR; tk.lexeme = temp; tk.col=startCol; return tk; }
                }
            }
            tk.type = TokenType::TK_SYMBOL; tk.lexeme = "<"; tk.col = startCol; return tk;
        }

        if (c == '-') {
            int startCol = col;
            get();
            if (peek() == '-') { get(); tk.type = TokenType::TK_OPERATOR; tk.lexeme = "--"; tk.col=startCol; return tk; }
            tk.type = TokenType::TK_SYMBOL; tk.lexeme = "-"; tk.col=startCol; return tk;
        }

        if (c == '>') { int startCol = col; get(); tk.type = TokenType::TK_SYMBOL; tk.lexeme = ">"; tk.col=startCol; return tk; }

        if (const std::string singleSymbols = "{}()[],:."; singleSymbols.find(c) != std::string::npos) {
            int startCol = col;
            std::string s(1, get());
            tk.type = TokenType::TK_SYMBOL; tk.lexeme = s; tk.col=startCol; return tk;
        }

        if (c == '@') {
            int startCol = col;
            std::string s;
            s.push_back(get());
            while (isalnum(static_cast<unsigned char>(peek())) || peek()=='_') s.push_back(get());
            tk.type = TokenType::TK_IDENTIFIER; tk.lexeme = s; tk.col=startCol; return tk;
        }

        int startCol = col;
        std::string s(1, get());
        tk.type = TokenType::TK_UNKNOWN;
        tk.lexeme = s;
        tk.col = startCol;
        return tk;
    }
};

// converte enum para string (para o CSV)
std::string tokenTypeToStr(TokenType t) {
    switch (t) {
        case TokenType::TK_EOF: return "EOF";
        case TokenType::TK_IDENTIFIER: return "IDENT";
        case TokenType::TK_KEYWORD: return "KEYWORD";
        case TokenType::TK_NUMBER: return "NUMBER";
        case TokenType::TK_STRING: return "STRING";
        case TokenType::TK_SYMBOL: return "SYMBOL";
        case TokenType::TK_OPERATOR: return "OP";
        case TokenType::TK_COMMENT: return "COMMENT";
        case TokenType::TK_UNKNOWN: return "UNK";
        case TokenType::TK_ERROR: return "ERROR";
    }
    return "UNK";
}

int main(int argc, char** argv) {
    if (argc < 2) { std::cerr << "Uso: " << argv[0] << " arquivo.tonto\n"; return 1; }
    std::ifstream in(argv[1], std::ios::in | std::ios::binary);
    if (!in) { std::cerr << "Não consegui abrir " << argv[1] << "\n"; return 1; }
    std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

    Lexer lx(content);

    // nome do arquivo de saída
    std::string outName = std::string(argv[1]) + ".tok";
    std::ofstream out(outName);
    if (!out) { std::cerr << "Não consegui criar " << outName << "\n"; return 1; }

    while (true) {
        Token t = lx.nextToken();
        if (t.type == TokenType::TK_EOF) break;
        out << t.line << "," << t.col << "," << tokenTypeToStr(t.type) << "," << t.lexeme << "\n";

        if (t.type == TokenType::TK_ERROR) {
            std::cerr << "Erro léxico na linha " << t.line << ", coluna " << t.col
                      << ": " << t.lexeme << std::endl;
            break;
        }
    }

    std::cout << "Tokens salvos em: " << outName << std::endl;
    return 0;
}
