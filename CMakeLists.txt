cmake_minimum_required(VERSION 3.25.1)
project(CompiladoresUFERSA)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Encontrar o Flex e configurar a geração do lexer ---
find_package(FLEX REQUIRED)

# Define o local do arquivo de saída do Flex
set(FLEX_OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.cc)

# Isso irá gerar o arquivo ${FLEX_OUTPUT_FILE} a partir do lexer.l
add_custom_command(
        OUTPUT ${FLEX_OUTPUT_FILE}
        COMMAND ${FLEX_EXECUTABLE} -o ${FLEX_OUTPUT_FILE} ${CMAKE_CURRENT_SOURCE_DIR}/analisador_lexico/lexer/lexer.l
        MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/analisador_lexico/lexer/lexer.l
)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})

file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_executable(tonto_lexer
        main.cpp
        ./analisador_lexico/entities/Token.cpp
        ./analisador_lexico/entities/TokenAnalisys.cpp
        ./analisador_lexico/lexer_utils/lex_config.cpp
        ./analisador_lexico/lexer_utils/KeyWordGroup/KeyWordManager/KeyWordManager.cpp
        ./analisador_lexico/util/JsonWriter.cpp

        ${FLEX_OUTPUT_FILE}
        ./analisador_lexico/entities/Token.h
        ./analisador_lexico/entities/TokenAnalisys.h
        ./analisador_lexico/lexer_utils/KeyWordGroup/ClassStereotypeKeywords/ClassStereotypeKeywords.h
        analisador_lexico/lexer_utils/KeyWordGroup/KeyWordManager/KeyWordManager.h
        ./analisador_lexico/lexer_utils/KeyWordGroup/MetaAttributeKeywords/MetaAttributeKeywords.h
        ./analisador_lexico/lexer_utils/KeyWordGroup/NativeTypeKeywords/NativeTypeKeywords.h
        ./analisador_lexico/lexer_utils/KeyWordGroup/RelationStereotypeKeywords/RelationStereotypeKeywords.h
        ./analisador_lexico/lexer_utils/KeyWordGroup/ReservedWordKeywords/ReservedWordKeywords.h
        ./analisador_lexico/lexer_utils/KeyWordGroup/KeyWordGroup.h
        ./analisador_lexico/lexer_utils/lex_config.h
        ./analisador_lexico/util/JsonWriter.h
)

target_include_directories(tonto_lexer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(tonto_lexer PRIVATE ${CMAKE_CURRENT_BINARY_DIR})