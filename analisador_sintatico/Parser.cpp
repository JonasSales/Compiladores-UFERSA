#include "Parser.h"
#include <iostream>

#include "analisador_lexico/lexer_utils/lex_config.h"

Parser::Parser(const map<int, Token>& tokens) : currentTokenIndex(0), pacoteAtual("default") {
    // Converte o mapa de tokens (saída do léxico) em um vetor (stream)
    for (const auto&[fst, snd] : tokens) {
        tokenStream.push_back(snd);
    }
    // Adiciona um token de fim de arquivo
    tokenStream.emplace_back(0, 0, TokenType::END_OF_FILE, "EOF");
}

Token Parser::lookAhead(int offset) const {
    if (currentTokenIndex + offset < tokenStream.size()) {
        return tokenStream[currentTokenIndex + offset];
    }
    return tokenStream.back(); // Retorna EOF
}

Token Parser::consumeToken() {
    if (currentTokenIndex < tokenStream.size() - 1) { // Não consome o EOF
        return tokenStream[currentTokenIndex++];
    }
    return tokenStream.back(); // Retorna EOF
}

bool Parser::match(TokenType type) {
    if (lookAhead().getTokenType() == type) {
        consumeToken();
        return true;
    }
    return false;
}

bool Parser::match(TokenType type, const string& lexeme) {
    if (lookAhead().getTokenType() == type && lookAhead().getLexeme() == lexeme) {
        consumeToken();
        return true;
    }
    return false;
}

void Parser::addErro(const string& mensagem, const Token& tokenErrado) {
     errosSintaticos.push_back({
        tokenErrado.getLine(),
        tokenErrado.getColumn(),
        mensagem
    });
}

void Parser::addErro(const string& mensagem) {
    Token tokenErrado = lookAhead();
    if (tokenErrado.getTokenType() == TokenType::END_OF_FILE && currentTokenIndex > 0) {
        tokenErrado = tokenStream[currentTokenIndex - 1]; // Pega o último token válido
    }
    addErro(mensagem, tokenErrado);
}

// Ponto de entrada do parser
void Parser::parse() {
    try {
        parseProgram();
        if (lookAhead().getTokenType() != TokenType::END_OF_FILE) {
            addErro("Token(s) inesperado(s) '" + lookAhead().getLexeme() + "' apos o fim do parsing.");
        }
    } catch (const std::exception& e) {
        addErro(string("Excecao do parser: ") + e.what());
    }
    
    // Imprime o relatório de erros no console (Requisito 106)
    if (!errosSintaticos.empty()) {
        std::cerr << "\n--- Relatorio de Erros Sintaticos ---" << std::endl;
        for (const auto& erro : errosSintaticos) {
            std::cerr << "[ERRO SINTATICO] Linha " << erro.linha << ", Coluna " << erro.coluna
                      << ": " << erro.mensagem << std::endl;
        }
    } else {
        std::cout << "\nAnalise sintatica concluida com sucesso. Nenhum erro encontrado." << std::endl;
    }
}

// program ::= (packageDecl | classDecl | generalizationSet | datatypeDecl | enumDecl | relationDecl)*
void Parser::parseProgram() {
    while (lookAhead().getTokenType() != TokenType::END_OF_FILE) {
        
        // 1. Declaracao de Pacote [cite: 48]
        if (match(TokenType::RESERVED_WORD, "package")) {
            parsePackageDecl();
        } 
        // 2. Declaracao de Classe [cite: 53, 57]
        else if (lookAhead().getTokenType() == TokenType::CLASS_STEREOTYPE) {
            parseClassDecl();
        }
        // 5. Declaracao de Generalization Set [cite: 81, 82]
        else if (match(TokenType::RESERVED_WORD, "disjoint") ||
                 match(TokenType::RESERVED_WORD, "complete") ||
                 match(TokenType::RESERVED_WORD, "genset")) {
            parseGeneralizationSet();
        }
        // 3. Declaracao de Datatype [cite: 61]
        else if (match(TokenType::RESERVED_WORD, "datatype")) {
            parseDatatypeDecl();
        }
        // 4. Declaracao de Enum [cite: 69]
        else if (match(TokenType::RESERVED_WORD, "enum")) {
            parseEnumDecl();
        }
        // 6. Declaracao de Relacao Externa [cite: 100]
        else if (lookAhead().getTokenType() == TokenType::RELATION_STEREOTYPE) {
            parseExternalRelationDecl();
        }
        else {
            addErro("Token inesperado no escopo global: '" + lookAhead().getLexeme() + "'. Esperava-se o inicio de uma declaracao (package, kind, genset, etc.).");
            consumeToken(); // Evita loop infinito
        }
    }
}

// 1. packageDecl ::= 'package' CLASS_NAME
void Parser::parsePackageDecl() {
    Token nomePacote = lookAhead();
    if (match(TokenType::CLASS_NAME)) {
        pacoteAtual = nomePacote.getLexeme();
        pacotesEncontrados.push_back(pacoteAtual);
    } else {
        addErro("Esperava um nome de pacote (CLASS_NAME) apos 'package'.", nomePacote);
    }
}

// 2. classDecl ::= CLASS_STEREOTYPE CLASS_NAME ('specializes' CLASS_NAME)? ('{' (attributeDecl | internalRelationDecl)* '}')?
void Parser::parseClassDecl() {
    SinteseClasse novaClasse;
    novaClasse.pacote = pacoteAtual;

    Token estereotipo = consumeToken(); // Já sabemos que é CLASS_STEREOTYPE pelo parseProgram()
    novaClasse.estereotipo = estereotipo.getLexeme();

    Token nomeClasse = lookAhead();
    if (match(TokenType::CLASS_NAME)) {
        novaClasse.nome = nomeClasse.getLexeme();
    } else {
        addErro("Esperava um nome de classe (CLASS_NAME) apos o estereotipo '" + estereotipo.getLexeme() + "'.", nomeClasse);
        return; // Erro grave, não continuar
    }
    
    // Trata especializacao (ex: "phase Child specializes Person") [cite: 57]
    if (match(TokenType::RESERVED_WORD, "specializes")) {
        Token pai = lookAhead();
        if (match(TokenType::CLASS_NAME)) {
            novaClasse.herdaDe = pai.getLexeme();
        } else {
            addErro("Esperava um nome de classe (CLASS_NAME) apos 'specializes'.", pai);
        }
    }

    // Trata bloco da classe (atributos e relacoes internas) [cite: 53]
    if (match(TokenType::SYMBOL, "{")) {
        while (lookAhead().getTokenType() != TokenType::SYMBOL || lookAhead().getLexeme() != "}") {
            if (lookAhead().getTokenType() == TokenType::END_OF_FILE) {
                addErro(" '}' esperado, mas o arquivo terminou (classe '" + novaClasse.nome + "').");
                break;
            }

            // Atributo (ex: "name: string") [cite: 55]
            if (lookAhead(1).getLexeme() == ":") {
                novaClasse.atributos.push_back(parseAttributeDecl());
            }
            // Relacao Interna (ex: "@componentOf ...") [cite: 97]
            else if (lookAhead().getTokenType() == TokenType::RELATION_STEREOTYPE) {
                // parseInternalRelationDecl(novaClasse); // (Implementar)
                // Por enquanto, apenas consumimos para evitar erros
                addErro("Parser de relacao interna (ex: @componentOf) nao implementado. Pulando tokens.", lookAhead());
                while(lookAhead().getTokenType() != TokenType::RELATION_NAME && lookAhead().getLexeme() != "}") consumeToken();
            }
            else {
                addErro("Esperava declaracao de atributo (nome: tipo) ou relacao interna (@estereotipo ...).", lookAhead());
                consumeToken(); // Evita loop
            }
        }
        if (!match(TokenType::SYMBOL, "}")) {
            addErro("Esperava '}' para fechar a declaracao da classe '" + novaClasse.nome + "'.");
        }
    }
    
    classesEncontradas.push_back(novaClasse);
}

// attributeDecl ::= RELATION_NAME ':' (NATIVE_DATA_TYPE | CLASS_NAME) ('{' META_ATTRIBUTE '}')?
SinteseAtributo Parser::parseAttributeDecl() {
    SinteseAtributo atributo;
    
    Token nomeAtributo = lookAhead();
    if (match(TokenType::RELATION_NAME)) {
        atributo.nome = nomeAtributo.getLexeme();
    } else {
        addErro("Esperava um nome de atributo (RELATION_NAME).", nomeAtributo);
        return atributo;
    }
    
    if (!match(TokenType::SYMBOL, ":")) {
        addErro("Esperava ':' apos o nome do atributo '" + atributo.nome + "'.", lookAhead());
        return atributo;
    }

    Token tipoAtributo = lookAhead();
    if (match(TokenType::NATIVE_DATA_TYPE)) {
        atributo.tipo = tipoAtributo.getLexeme();
    } else if (match(TokenType::CLASS_NAME)) {
        atributo.tipo = tipoAtributo.getLexeme();
    } else if (match(TokenType::DATATYPE_NAME)) { // Suporte ao lexer rule
        atributo.tipo = tipoAtributo.getLexeme();
    } else {
        addErro("Esperava um tipo (NATIVE_DATA_TYPE ou CLASS_NAME) para o atributo '" + atributo.nome + "'.", tipoAtributo);
        return atributo;
    }

    // Trata meta-atributo (ex: {const}) [cite: 56]
    if (match(TokenType::SYMBOL, "{")) {
        Token meta = lookAhead();
        if (match(TokenType::META_ATTRIBUTE)) {
            atributo.metaAtributo = meta.getLexeme();
        } else {
            addErro("Esperava um meta-atributo (const, ordered, etc.) dentro de {}.", meta);
        }
        if (!match(TokenType::SYMBOL, "}")) {
            addErro("Esperava '}' apos o meta-atributo.", lookAhead());
        }
    }
    return atributo;
}

// 5. genset ::= ('disjoint'?) ('complete'?) 'genset' CLASS_NAME '{' 'general' CLASS_NAME 'specifics' CLASS_NAME (',' CLASS_NAME)* '}' [cite: 82-85]
void Parser::parseGeneralizationSet() {
    SinteseGenset genset;
    
    // Modificadores (ordem não importa)
    genset.isDisjoint = match(TokenType::RESERVED_WORD, "disjoint");
    genset.isComplete = match(TokenType::RESERVED_WORD, "complete");
    // (Pode-se verificar novamente caso a ordem seja invertida)
    if (!genset.isDisjoint) genset.isDisjoint = match(TokenType::RESERVED_WORD, "disjoint");

    // (Ainda não tratamos a forma '... where ...' [cite: 81])
    
    if (!match(TokenType::RESERVED_WORD, "genset")) {
        addErro("Esperava 'genset' para iniciar a declaracao do conjunto de generalizacao.", lookAhead());
        return;
    }

    Token nomeGenset = lookAhead();
    if (match(TokenType::CLASS_NAME)) {
        genset.nome = nomeGenset.getLexeme();
    } else {
        addErro("Esperava o nome do genset (CLASS_NAME).", nomeGenset);
    }

    if (!match(TokenType::SYMBOL, "{")) {
        addErro("Esperava '{' para abrir o genset '" + genset.nome + "'.", lookAhead());
        return;
    }

    if (match(TokenType::RESERVED_WORD, "general")) {
        Token geral = lookAhead();
        if (match(TokenType::CLASS_NAME)) {
            genset.classeGeral = geral.getLexeme();
        } else {
            addErro("Esperava o nome da classe 'general' (CLASS_NAME).", geral);
        }
    } else {
         addErro("Esperava 'general' dentro do genset.", lookAhead());
    }

    if (match(TokenType::RESERVED_WORD, "specifics")) {
        do {
            Token especifico = lookAhead();
            if (match(TokenType::CLASS_NAME)) {
                genset.classesEspecificas.push_back(especifico.getLexeme());
            } else {
                addErro("Esperava o nome da classe 'specifics' (CLASS_NAME).", especifico);
                break;
            }
        } while (match(TokenType::SYMBOL, ","));
    } else {
        addErro("Esperava 'specifics' dentro do genset.", lookAhead());
    }
    
    if (!match(TokenType::SYMBOL, "}")) {
        addErro("Esperava '}' para fechar o genset '" + genset.nome + "'.", lookAhead());
    }
    gensetsEncontrados.push_back(genset);
}

// 3. datatypeDecl ::= 'datatype' CLASS_NAME '{' attributeDecl* '}' [cite: 61]
void Parser::parseDatatypeDecl() {
    SinteseDatatype novoDatatype;
    novoDatatype.pacote = pacoteAtual;

    Token nome = lookAhead();
    if (match(TokenType::CLASS_NAME)) {
        novoDatatype.nome = nome.getLexeme();
    } else {
        addErro("Esperava um nome (CLASS_NAME) apos 'datatype'.", nome);
        return;
    }

    if (!match(TokenType::SYMBOL, "{")) {
        addErro("Esperava '{' para abrir o datatype '" + novoDatatype.nome + "'.", lookAhead());
        return;
    }

    while (lookAhead().getTokenType() != TokenType::SYMBOL || lookAhead().getLexeme() != "}") {
        if (lookAhead().getTokenType() == TokenType::END_OF_FILE) {
            addErro(" '}' esperado, mas o arquivo terminou (datatype '" + novoDatatype.nome + "').");
            break;
        }
        
        if (lookAhead(1).getLexeme() == ":") {
            novoDatatype.atributos.push_back(parseAttributeDecl());
        } else {
            addErro("Esperava declaracao de atributo (nome: tipo) dentro do datatype.", lookAhead());
            consumeToken(); // Evita loop
        }
    }
    
    if (!match(TokenType::SYMBOL, "}")) {
        addErro("Esperava '}' para fechar o datatype '" + novoDatatype.nome + "'.", lookAhead());
    }
    datatypesEncontrados.push_back(novoDatatype);
}

// 4. enumDecl ::= 'enum' CLASS_NAME '{' CLASS_NAME (',' CLASS_NAME)* '}' [cite: 69]
void Parser::parseEnumDecl() {
    SinteseEnum novoEnum;
    novoEnum.pacote = pacoteAtual;

    Token nome = lookAhead();
    if (match(TokenType::CLASS_NAME)) {
        novoEnum.nome = nome.getLexeme();
    } else {
        addErro("Esperava um nome (CLASS_NAME) apos 'enum'.", nome);
        return;
    }

    if (!match(TokenType::SYMBOL, "{")) {
        addErro("Esperava '{' para abrir o enum '" + novoEnum.nome + "'.", lookAhead());
        return;
    }

    do {
        Token instancia = lookAhead();
        // No PDF[cite: 69], as instancias sao 'Blue', 'Green', etc.
        // No lexer [file: analisador_lexico/lexer/lexer.l], 'Blue' é um CLASS_NAME.
        if (match(TokenType::CLASS_NAME)) {
            novoEnum.instancias.push_back(instancia.getLexeme());
        }
        // A regra do lexer para INSTANCE_NAME [file: analisador_lexico/lexer/lexer.l] exige um digito no final.
        // O PDF [cite: 68] menciona que a regra do lexer era diferente, mas o exemplo [cite: 69] não usa digitos.
        // Vamos aceitar CLASS_NAME ou INSTANCE_NAME para flexibilidade.
        else if (match(TokenType::INSTANCE_NAME)) {
             novoEnum.instancias.push_back(instancia.getLexeme());
        }
        else {
            addErro("Esperava um nome de instancia (CLASS_NAME ou INSTANCE_NAME) dentro do enum.", instancia);
            break;
        }
    } while (match(TokenType::SYMBOL, ","));

    if (!match(TokenType::SYMBOL, "}")) {
        addErro("Esperava '}' para fechar o enum '" + novoEnum.nome + "'.", lookAhead());
    }
    enumsEncontrados.push_back(novoEnum);
}

// 6. relationDecl ::= RELATION_STEREOTYPE 'relation' CLASS_NAME CARDINALITY RELATION CARDINALITY CLASS_NAME [cite: 100]
void Parser::parseExternalRelationDecl() {
    SinteseRelacao rel;
    
    Token estereo = consumeToken(); // Já sabemos que é RELATION_STEREOTYPE
    rel.estereotipo = estereo.getLexeme();
    
    if (!match(TokenType::RESERVED_WORD, "relation")) {
        // (Pode ser uma relacao sem nome, ex: '@mediation Person [1] -- [1] Address')
        // permitir ambas as formas.
    }

    Token dominio = lookAhead();
    if (match(TokenType::CLASS_NAME)) {
        rel.dominio = dominio.getLexeme();
    } else {
        addErro("Esperava o Dominio (CLASS_NAME) da relacao.", dominio);
        return;
    }
    
    Token cardDom = lookAhead();
    if (match(TokenType::CARDINALITY)) {
        rel.cardinalidadeDominio = cardDom.getLexeme();
    } else {
        addErro("Esperava a Cardinalidade do Dominio (ex: [1..*]).", cardDom);
        return;
    }
    
    Token simbolo = lookAhead();
    if (match(TokenType::RELATION)) {
        rel.simboloRelacao = simbolo.getLexeme();
    } else {
        addErro("Esperava um simbolo de relacao (ex: --, <>--).", simbolo);
        return;
    }
    
    Token cardImg = lookAhead();
    if (match(TokenType::CARDINALITY)) {
        rel.cardinalidadeImagem = cardImg.getLexeme();
    } else {
        addErro("Esperava a Cardinalidade da Imagem (ex: [1]).", cardImg);
        return;
    }
    
    Token imagem = lookAhead();
    if (match(TokenType::CLASS_NAME)) {
        rel.imagem = imagem.getLexeme();
    } else {
        addErro("Esperava a Imagem (CLASS_NAME) da relacao.", imagem);
        return;
    }
    
    relacoesExternasEncontradas.push_back(rel);
}