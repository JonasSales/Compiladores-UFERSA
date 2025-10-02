%option noyywrap c++

%{
#include <iostream>
#include <string>
#include <vector>

using std::cout;
using std::endl;
using std::string;
using std::vector;

enum class TokenType {
    CLASS_NAME, RELATION_NAME, INSTANCE_NAME, DATATYPE_NAME,
    KEYWORD, NUMBER, STRING, SYMBOL,
    UNKNOWN, END_OF_FILE
};

string token_to_string(TokenType type) {
    switch (type) {
        case TokenType::CLASS_NAME:     return "CLASS_NAME";
        case TokenType::RELATION_NAME:  return "RELATION_NAME";
        case TokenType::INSTANCE_NAME:  return "INSTANCE_NAME";
        case TokenType::DATATYPE_NAME:  return "DATATYPE_NAME";
        case TokenType::KEYWORD:        return "KEYWORD";
        case TokenType::NUMBER:         return "NUMBER";
        case TokenType::STRING:         return "STRING";
        case TokenType::SYMBOL:         return "SYMBOL";
        case TokenType::UNKNOWN:        return "UNKNOWN";
        default:                        return "EOF";
    }
}

// =================================================================
// NOVA FUNÇÃO: Prepara uma string para ser incluída em um JSON
// =================================================================
string escape_json(const string& s) {
    string escaped;
    escaped.reserve(s.length());
    for (char c : s) {
        switch (c) {
            case '"':  escaped += "\\\""; break;
            case '\\': escaped += "\\\\"; break;
            case '\b': escaped += "\\b";  break;
            case '\f': escaped += "\\f";  break;
            case '\n': escaped += "\\n";  break;
            case '\r': escaped += "\\r";  break;
            case '\t': escaped += "\\t";  break;
            default:   escaped += c;      break;
        }
    }
    return escaped;
}


int curr_line = 1;
int curr_col  = 1;

void update_pos(const char* yytext) {
    for (int i = 0; yytext[i] != '\0'; i++) {
        if (yytext[i] == '\n') {
            curr_line++;
            curr_col = 1;
        } else {
            curr_col++;
        }
    }
}

static const vector<string> KEYWORDS = {
    "aggregation", "bringsAbout", "boolean",
    "category", "characterization", "collective",
    "complete", "componentOf", "composition", "comparative",
    "const", "constitution", "creation", "date", "datetime",
    "derivation", "derived", "disjoint", "event", "externalDependence",
    "extrinsicMode", "formal", "general", "genset", "historicalDependence",
    "historicalRole", "historicalRoleMixin", "inherence", "instantiation",
    "intrisicMode", "kind", "manifestation", "material", "mediation", "memberOf",
    "mixin", "mode", "number", "ordered", "package", "participation", "participational",
    "phase", "phaseMixin", "process", "quality", "quantity", "redefines", "relator", "role",
    "roleMixin", "situation", "specifics", "string", "subCollectionof", "subkind", "subQualityOf",
    "subsets", "termination", "time", "triggers", "value", "where"
};

bool is_keyword(const char* s) {
    for (const auto& kw : KEYWORDS) {
        if (kw == s) return true;
    }
    return false;
}

// =================================================================
// MODIFICADO: Agora emite um objeto JSON para cada token
// =================================================================
void emit_token(TokenType type, const char* yytext, int line, int col) {
    static bool is_first_token = true;
    if (!is_first_token) {
        cout << "," << endl;
    }
    is_first_token = false;

    cout << "  {" << endl;
    cout << "    \"line\": " << line << "," << endl;
    cout << "    \"column\": " << col << "," << endl;
    cout << "    \"type\": \"" << token_to_string(type) << "\"," << endl;
    cout << "    \"lexeme\": \"" << escape_json(yytext) << "\"" << endl;
    cout << "  }";
}

%}

/* Expressões Regulares para as convenções da linguagem TONTO */
CLASS_NAME      [A-Z][a-zA-Z_]*
RELATION_NAME   [a-z][a-zA-Z_]*
INSTANCE_NAME   [a-zA-Z][a-zA-Z_]*[0-9]+
DATATYPE_NAME   [a-zA-Z]+DataType
NUMBER          [0-9]+(\.[0-9]+)?
STRING          \"([^\\\"]|\\.)*\"
COMMENT_LINE    "//".*
COMMENT_BLOCK   "/*"([^*]*\*+([^/*][^*]*\*+)*)*"/"

%%

{COMMENT_LINE}  |
{COMMENT_BLOCK} { update_pos(yytext); }

{DATATYPE_NAME} {
                    int start_line = curr_line, start_col = curr_col;
                    update_pos(yytext);
                    emit_token(TokenType::DATATYPE_NAME, yytext, start_line, start_col);
                }

{INSTANCE_NAME} {
                    int start_line = curr_line, start_col = curr_col;
                    update_pos(yytext);
                    emit_token(TokenType::INSTANCE_NAME, yytext, start_line, start_col);
                }

{CLASS_NAME}    {
                    int start_line = curr_line, start_col = curr_col;
                    update_pos(yytext);
                    TokenType type = is_keyword(yytext) ? TokenType::KEYWORD : TokenType::CLASS_NAME;
                    emit_token(type, yytext, start_line, start_col);
                }

{RELATION_NAME} {
                    int start_line = curr_line, start_col = curr_col;
                    update_pos(yytext);
                    TokenType type = is_keyword(yytext) ? TokenType::KEYWORD : TokenType::RELATION_NAME;
                    emit_token(type, yytext, start_line, start_col);
                }

{NUMBER}        {
                    int start_line = curr_line, start_col = curr_col;
                    update_pos(yytext);
                    emit_token(TokenType::NUMBER, yytext, start_line, start_col);
                }

{STRING}        {
                    int start_line = curr_line, start_col = curr_col;
                    update_pos(yytext);
                    emit_token(TokenType::STRING, yytext, start_line, start_col);
                }

"--"            { /* REGRA REINTEGRADA */
                    int start_line = curr_line, start_col = curr_col;
                    update_pos(yytext);
                    emit_token(TokenType::SYMBOL, yytext, start_line, start_col);
                }

"..."           {
                    int start_line = curr_line, start_col = curr_col;
                    update_pos(yytext);
                    emit_token(TokenType::SYMBOL, yytext, start_line, start_col);
                }

[{}()\[\],@]    { /* @ REINTEGRADO */
                    int start_line = curr_line, start_col = curr_col;
                    update_pos(yytext);
                    emit_token(TokenType::SYMBOL, yytext, start_line, start_col);
                }

[ \t\r\n]+      { update_pos(yytext); }

.               {
                    int start_line = curr_line, start_col = curr_col;
                    update_pos(yytext);
                    emit_token(TokenType::UNKNOWN, yytext, start_line, start_col);
                }

%%