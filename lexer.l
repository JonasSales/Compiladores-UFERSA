%option noyywrap c++

%{
#include <iostream>
#include <string>
#include <vector>
#include <map>

static map<string, TokenType> keyword_map;
using std::cout;
using std::endl;
using std::string;
using std::vector;

enum class TokenType {
    CLASS_NAME, RELATION_NAME, INSTANCE_NAME, DATATYPE_NAME,
    CLASS_STEREOTYPE, RELATION_STEREOTYPE, 
    RESERVED_WORD, NATIVE_DATATYPE, META_ATTRIBUTE,
    NUMBER, STRING, SYMBOL,
    UNKNOWN, END_OF_FILE
};

string token_to_string(TokenType type) {
    switch (type) {
        case TokenType::CLASS_NAME:           return "CLASS_NAME";
        case TokenType::RELATION_NAME:        return "RELATION_NAME";
        case TokenType::INSTANCE_NAME:        return "INSTANCE_NAME";
        case TokenType::DATATYPE_NAME:        return "DATATYPE_NAME";
        case TokenType::CLASS_STEREOTYPE:     return "CLASS_STEREOTYPE";
        case TokenType::RELATION_STEREOTYPE:  return "RELATION_STEREOTYPE";
        case TokenType::RESERVED_WORD:        return "RESERVED_WORD";
        case TokenType::NATIVE_DATA_TYPE:     return "NATIVE_DATA_TYPE";
        case TokenType::META_ATTRIBUTE:       return "META_ATTRIBUTE";
        case TokenType::NUMBER:               return "NUMBER";
        case TokenType::STRING:               return "STRING";
        case TokenType::SYMBOL:               return "SYMBOL";
        case TokenType::UNKNOWN:              return "UNKNOWN";
        default:                              return "EOF";
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

/* static const vector<string> KEYWORDS = {
    "aggregation", "bringsAbout", "boolean",
    "category", "characterization", "collective",
    "complete", "componentOf", "composition", "comparative",
    "const", "constitution", "creation", "date", "datetime",
    "derivation", "derived", "disjoint", "event", "externalDependence",
    "extrinsicMode", "formal", "general", "genset", "historicalDependence",
    "historicalRole", "historicalRoleMixin", "inherence", "instantiation",
    "intrinsicMode", "kind", "manifestation", "material", "mediation", "memberOf",
    "mixin", "mode", "number", "ordered", "package", "participation", "participational",
    "phase", "phaseMixin", "process", "quality", "quantity", "redefines", "relator", "role",
    "roleMixin", "situation", "specifics", "string", "subCollectionOf", "subkind", "subQualityOf",
    "subsets", "termination", "time", "triggers", "value", "where"
};

bool is_keyword(const char* s) {
    for (const auto& kw : KEYWORDS) {
        if (kw == s) return true;
    }
    return false;
}
*/
void initialize_keywords() {
    //ESTEREÓTIPOS DE CLASSE
    keyword_map["event"]                = TokenType::CLASS_STEREOTYPE;
    keyword_map["situation"]            = TokenType::CLASS_STEREOTYPE;
    keyword_map["process"]              = TokenType::CLASS_STEREOTYPE;
    keyword_map["category"]             = TokenType::CLASS_STEREOTYPE;
    keyword_map["mixin"]                = TokenType::CLASS_STEREOTYPE;
    keyword_map["phaseMixin"]           = TokenType::CLASS_STEREOTYPE;
    keyword_map["roleMixin"]            = TokenType::CLASS_STEREOTYPE;
    keyword_map["historicalRoleMixin"]  = TokenType::CLASS_STEREOTYPE;
    keyword_map["kind"]                 = TokenType::CLASS_STEREOTYPE;
    keyword_map["collective"]           = TokenType::CLASS_STEREOTYPE;
    keyword_map["quantity"]             = TokenType::CLASS_STEREOTYPE;
    keyword_map["quality"]              = TokenType::CLASS_STEREOTYPE;
    keyword_map["mode"]                 = TokenType::CLASS_STEREOTYPE;
    keyword_map["intrisicMode"]        = TokenType::CLASS_STEREOTYPE;
    keyword_map["extrinsicMode"]        = TokenType::CLASS_STEREOTYPE;
    keyword_map["subkind"]              = TokenType::CLASS_STEREOTYPE;
    keyword_map["phase"]                = TokenType::CLASS_STEREOTYPE;
    keyword_map["role"]                 = TokenType::CLASS_STEREOTYPE;
    keyword_map["historicalRole"]       = TokenType::CLASS_STEREOTYPE;

    //ESTEREÓTIPOS DE RELAÇÃO
    keyword_map["material"]             = TokenType::RELATION_STEREOTYPE;
    keyword_map["derivation"]           = TokenType::RELATION_STEREOTYPE;
    keyword_map["comparative"]          = TokenType::RELATION_STEREOTYPE;
    keyword_map["mediation"]            = TokenType::RELATION_STEREOTYPE;
    keyword_map["characterization"]     = TokenType::RELATION_STEREOTYPE;
    keyword_map["externalDependence"]   = TokenType::RELATION_STEREOTYPE;
    keyword_map["componentOf"]         = TokenType::RELATION_STEREOTYPE;
    keyword_map["memberOf"]             = TokenType::RELATION_STEREOTYPE;
    keyword_map["subCollectionOf"]      = TokenType::RELATION_STEREOTYPE;
    keyword_map["subQualityOf"]         = TokenType::RELATION_STEREOTYPE;
    keyword_map["instantiation"]        = TokenType::RELATION_STEREOTYPE;
    keyword_map["termination"]          = TokenType::RELATION_STEREOTYPE;
    keyword_map["participational"]      = TokenType::RELATION_STEREOTYPE;
    keyword_map["participation"]        = TokenType::RELATION_STEREOTYPE;
    keyword_map["historicalDependence"] = TokenType::RELATION_STEREOTYPE;
    keyword_map["creation"]             = TokenType::RELATION_STEREOTYPE;
    keyword_map["manifestation"]        = TokenType::RELATION_STEREOTYPE;
    keyword_map["bringsAbout"]          = TokenType::RELATION_STEREOTYPE;
    keyword_map["triggers"]             = TokenType::RELATION_STEREOTYPE;    
    keyword_map["composition"]          = TokenType::RELATION_STEREOTYPE;
    keyword_map["aggregation"]          = TokenType::RELATION_STEREOTYPE;
    keyword_map["inherence"]            = TokenType::RELATION_STEREOTYPE;
    keyword_map["value"]                = TokenType::RELATION_STEREOTYPE;
    keyword_map["formal"]               = TokenType::RELATION_STEREOTYPE;
    keyword_map["constitution"]         = TokenType::RELATION_STEREOTYPE;

    //PALAVRAS RESERVADAS
    keyword_map["genset"]       = TokenType::RESERVED_WORD;
    keyword_map["disjoint"]     = TokenType::RESERVED_WORD;
    keyword_map["complete"]     = TokenType::RESERVED_WORD;
    keyword_map["general"]      = TokenType::RESERVED_WORD;
    keyword_map["specifics"]    = TokenType::RESERVED_WORD;
    keyword_map["where"]        = TokenType::RESERVED_WORD;
    keyword_map["package"]      = TokenType::RESERVED_WORD;

    //TIPOS DE DADOS NATIVOS
    keyword_map["number"]   = TokenType::NATIVE_DATA_TYPE;
    keyword_map["string"]   = TokenType::NATIVE_DATA_TYPE;
    keyword_map["boolean"]  = TokenType::NATIVE_DATA_TYPE;
    keyword_map["date"]     = TokenType::NATIVE_DATA_TYPE;
    keyword_map["time"]     = TokenType::NATIVE_DATA_TYPE;
    keyword_map["datetime"] = TokenType::NATIVE_DATA_TYPE;

    //META-ATRIBUTOS
    keyword_map["ordered"]      = TokenType::META_ATTRIBUTE;
    keyword_map["const"]        = TokenType::META_ATTRIBUTE;
    keyword_map["derived"]      = TokenType::META_ATTRIBUTE;
    keyword_map["subsets"]      = TokenType::META_ATTRIBUTE;
    keyword_map["redefines"]    = TokenType::META_ATTRIBUTE;
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
DELIM           [ \t\n]
WHITE_SPACES    {DELIM}+
UPPERCASE       [A-Z]
LOWERCASE       [a-z]
LETTER          [a-zA-Z]
DIGIT           [0-9]
UNDERLINE       "_"

CLASS_NAME      {UPPERCASE}({LETTER}|{UNDERLINE})*
/*CLASS_NAME      [A-Z][a-zA-Z_]* *\

RELATION_NAME   {LOWERCASE}({LETTER}|{UNDERLINE})*
/*RELATION_NAME   [a-z][a-zA-Z_]* *\

INSTANCE_NAME   {LETTER}({LETTER}|{UNDERLINE})*{DIGIT}+
/* INSTANCE_NAME   [a-zA-Z][a-zA-Z_]*[0-9]+ *\

DATATYPE_NAME   {LETTER}+"DataType"
/*DATATYPE_NAME   [a-zA-Z]+DataType *\

NUMBER          {DIGIT}+(\.{DIGIT}+)?
/* NUMBER          [0-9]+(\.[0-9]+)? *\

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
                    emit_token(TokenType::CLASS_NAME, yytext, start_line, start_col);
                }

{RELATION_NAME} {
                    int start_line = curr_line, start_col = curr_col;
                    update_pos(yytext);
                    auto it = keyword_map.find(yytext);
                    TokenType type = (it != keyword_map.end()) ? it->second : TokenType::RELATION_NAME;
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

"<>--" |"--<>" |".."|"--" |"..." {
                    int start_line = curr_line, start_col = curr_col;
                    update_pos(yytext);
                    emit_token(TokenType::SYMBOL, yytext, start_line, start_col);
                }

[{}()\[\],@:*]    { /* @ REINTEGRADO */
                    int start_line = curr_line, start_col = curr_col;
                    update_pos(yytext);
                    emit_token(TokenType::SYMBOL, yytext, start_line, start_col);
                }

[ \t\r\n]+      { update_pos(yytext); }

.               {
                    int start_line = curr_line, start_col = curr_col;
                    fprintf(stderr, "[ERRO LÉXICO] Linha %d, Coluna %d: Caractere desconhecido: '%s'.\n", start_line, start_col, yytext);
                    update_pos(yytext);
                    emit_token(TokenType::UNKNOWN, yytext, start_line, start_col);
                }

%%