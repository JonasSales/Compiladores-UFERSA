cmake_minimum_required(VERSION 3.10)
project(tonto_lexer CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Encontrar Flex e Bison
find_package(FLEX REQUIRED)
find_package(BISON REQUIRED) # Adicionado

# 2. Configuração do Flex (Mantém o seu Lexer atual)
set(LEXER_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.cc)
add_custom_command(
        OUTPUT ${LEXER_OUTPUT}
        COMMAND ${FLEX_EXECUTABLE} -o ${LEXER_OUTPUT} ${CMAKE_CURRENT_SOURCE_DIR}/analisador_lexico/lexer/lexer.l
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/analisador_lexico/lexer/lexer.l
        COMMENT "Gerando o analisador léxico (Flex)"
)

# 3. Configuração do Bison (NOVO)
set(BISON_HEADER ${CMAKE_CURRENT_BINARY_DIR}/parser.hpp)
set(BISON_SOURCE ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp)

add_custom_command(
        OUTPUT ${BISON_SOURCE} ${BISON_HEADER}
        COMMAND ${BISON_EXECUTABLE} --defines=${BISON_HEADER} -o ${BISON_SOURCE} ${CMAKE_CURRENT_SOURCE_DIR}/analisador_sintatico/parser.y
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/analisador_sintatico/parser.y
        COMMENT "Gerando o analisador sintático (Bison)"
)

# 4. Arquivos fontes
file(GLOB_RECURSE SOURCES
        "main.cpp"
        "util/*.h"
        "util/*.cpp"
        "analisador_lexico/*.h"
        "analisador_lexico/*.cpp"
        # Removemos o Parser.cpp manual antigo da compilação para não conflitar, ou mantemos se quiser comparar
        # "analisador_sintatico/Parser.cpp"
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

# 5. Executável
add_executable(tonto_lexer
        ${SOURCES}
        ${LEXER_OUTPUT}
        ${BISON_SOURCE} # Adiciona o fonte gerado pelo Bison
)

target_link_libraries(tonto_lexer ${FLEX_LIBRARIES})