cmake_minimum_required(VERSION 3.10)
project(tonto_lexer CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Encontra o pacote Flex no sistema.
find_package(FLEX REQUIRED)

# 2. Define o caminho para o arquivo de saída do Flex.
set(LEXER_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.cc)

# 3. Cria uma regra customizada para compilar o lexer.
add_custom_command(
        OUTPUT ${LEXER_OUTPUT}
        COMMAND ${FLEX_EXECUTABLE} -o ${LEXER_OUTPUT} ${CMAKE_CURRENT_SOURCE_DIR}/analisador_lexico/lexer/lexer.l
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/analisador_lexico/lexer/lexer.l
        COMMENT "Gerando o analisador léxico a partir de lexer.l"
)

# 4. Encontra todos os arquivos .h e .cpp recursivamente.
file(GLOB_RECURSE SOURCES
        "main.cpp"
        "analisador_lexico/*.h"
        "analisador_lexico/*.cpp"
        "analisador_sintatico/*.cpp"
        "analisador_sintatico/*.h"
)

# 5. Adiciona o diretório principal ao caminho de includes.
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# 6. Define o nome do executável e suas fontes.
add_executable(tonto_lexer
        ${SOURCES}
        ${LEXER_OUTPUT}
)

# 7. Vincula a biblioteca do Flex ao seu executável.
target_link_libraries(tonto_lexer ${FLEX_LIBRARIES})