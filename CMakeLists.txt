cmake_minimum_required(VERSION 3.10)
project(tonto_lexer CXX)

# Define o padrão do C++ para C++17, que é bom para projetos modernos.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Automação do Flex (Analisador Léxico) ---

# 1. Encontra o pacote Flex no sistema.
find_package(FLEX REQUIRED)

# 2. Define o caminho para o arquivo de saída do Flex.
set(LEXER_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.cc)

# 3. Cria uma regra customizada para compilar o lexer.
#    Esta regra executa o Flex no seu arquivo .l e gera o .cc.
#    O arquivo .cc é gerado na pasta de build para manter o projeto limpo.
add_custom_command(
        OUTPUT ${LEXER_OUTPUT}
        COMMAND ${FLEX_EXECUTABLE} -o ${LEXER_OUTPUT} ${CMAKE_CURRENT_SOURCE_DIR}/analisador_lexico/lexer/lexer.l
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/analisador_lexico/lexer/lexer.l
        COMMENT "Gerando o analisador léxico a partir de lexer.l"
)

# --- Descoberta Automática de Arquivos Fonte ---

# 4. Encontra todos os arquivos .h e .cpp recursivamente.
#    Isso evita que você precise adicionar cada novo arquivo manualmente.
file(GLOB_RECURSE SOURCES
        "main.cpp"
        "analisador_lexico/*.h"
        "analisador_lexico/*.cpp"
)

# --- Configuração do Executável ---

# 5. Adiciona o diretório principal ao caminho de includes.
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# 6. Define o nome do executável e suas fontes.
#    Note que incluímos o arquivo gerado pelo Flex (${LEXER_OUTPUT}).
add_executable(tonto_lexer
        ${SOURCES}
        ${LEXER_OUTPUT}
)

# 7. Vincula a biblioteca do Flex ao seu executável.
target_link_libraries(tonto_lexer ${FLEX_LIBRARIES})

# Mensagem para o desenvolvedor
message(STATUS "Build do tonto_lexer configurada com sucesso.")