cmake_minimum_required(VERSION 3.10)
project(tonto_lexer CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED) 

set(LEXER_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.cc)
add_custom_command(
        OUTPUT ${LEXER_OUTPUT}
        COMMAND ${FLEX_EXECUTABLE} -o ${LEXER_OUTPUT} ${CMAKE_CURRENT_SOURCE_DIR}/analisador_lexico/lexer/lexer.l
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/analisador_lexico/lexer/lexer.l
        COMMENT "Gerando o analisador léxico (Flex)"
)

set(BISON_HEADER ${CMAKE_CURRENT_BINARY_DIR}/parser.hpp)
set(BISON_SOURCE ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp)

add_custom_command(
        OUTPUT ${BISON_SOURCE} ${BISON_HEADER}
        COMMAND ${BISON_EXECUTABLE} --defines=${BISON_HEADER} -o ${BISON_SOURCE} ${CMAKE_CURRENT_SOURCE_DIR}/analisador_sintatico/parser.y
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/analisador_sintatico/parser.y
        COMMENT "Gerando o analisador sintático (Bison)"
)

file(GLOB_RECURSE SOURCES
        "main.cpp"
        "util/*.h"
        "util/*.cpp"
        "analisador_lexico/*.h"
        "analisador_lexico/*.cpp"
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

add_executable(tonto_lexer
        ${SOURCES}
        ${LEXER_OUTPUT}
        ${BISON_SOURCE} 
)

target_link_libraries(tonto_lexer ${FLEX_LIBRARIES})
