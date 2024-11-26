cmake_minimum_required(VERSION 3.28)
project(CompilerProject VERSION 0.1)

# Configurar o padrão C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Forçar compilação estática
set(BUILD_SHARED_LIBS OFF)

# Configurar os diretórios de bibliotecas
set(CMAKE_PREFIX_PATH "C:/MinGW")

# Adicionar a biblioteca lexical
add_library(lexical_analyzer STATIC
    lexical/lexical.cpp
)

# Configurar os diretórios de inclusão para a biblioteca lexical
target_include_directories(lexical_analyzer PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/lexical
)

# Adicionar a biblioteca sintática
add_library(sintatic_analyzer STATIC
    sintatico/sintatico.cpp
)

# Configurar os diretórios de inclusão para a biblioteca sintática
target_include_directories(sintatic_analyzer PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/sintatico
    ${CMAKE_CURRENT_SOURCE_DIR}/lexical
    ${CMAKE_CURRENT_SOURCE_DIR}/Geracao_codigo
)

# Linkar a biblioteca lexical_analyzer ao sintatic_analyzer
target_link_libraries(sintatic_analyzer PUBLIC lexical_analyzer)

# Adicionar a biblioteca Tabela_simbolos
add_library(tabela_simbolos STATIC
    Tabela_simbolos/tabela_simbolos.cpp
)

# Configurar os diretórios de inclusão para a biblioteca Tabela_simbolos
target_include_directories(tabela_simbolos PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Tabela_simbolos
)

# Linkar a biblioteca Tabela_simbolos ao sintatic_analyzer
target_link_libraries(sintatic_analyzer PUBLIC tabela_simbolos)

# Adicionar a biblioteca geracao_codigo
add_library(geracao_codigo STATIC
    Geracao_codigo/geracao_codigo.cpp
)

# Configurar os diretórios de inclusão para a biblioteca geracao_codigo
target_include_directories(geracao_codigo PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Geracao_codigo
)

# Linkar a biblioteca geracao_codigo ao sintatic_analyzer e tabela_simbolos
target_link_libraries(sintatic_analyzer PUBLIC geracao_codigo)

# Adicionar o executável principal
add_executable(main_program main.cpp)

# Configurar os diretórios de inclusão para o executável principal
target_include_directories(main_program PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/lexical
    ${CMAKE_CURRENT_SOURCE_DIR}/sintatico
    ${CMAKE_CURRENT_SOURCE_DIR}/Tabela_simbolos
    ${CMAKE_CURRENT_SOURCE_DIR}/Geracao_codigo
)

# Vincular o executável principal com as bibliotecas necessárias
target_link_libraries(main_program PRIVATE sintatic_analyzer tabela_simbolos geracao_codigo)

# Configurar as flags de compilação e linkagem
if(MSVC)
    target_compile_options(lexical_analyzer PRIVATE /W4)
    target_compile_options(sintatic_analyzer PRIVATE /W4)
    target_compile_options(tabela_simbolos PRIVATE /W4)
    target_compile_options(geracao_codigo PRIVATE /W4)
    target_compile_options(main_program PRIVATE /W4)
    target_compile_options(main_program PRIVATE /MT) # Linka estaticamente o runtime
else()
    target_compile_options(lexical_analyzer PRIVATE -Wall -Wextra -pedantic)
    target_compile_options(sintatic_analyzer PRIVATE -Wall -Wextra -pedantic)
    target_compile_options(tabela_simbolos PRIVATE -Wall -Wextra -pedantic)
    target_compile_options(geracao_codigo PRIVATE -Wall -Wextra -pedantic)
    target_compile_options(main_program PRIVATE -Wall -Wextra -pedantic)

    # Adiciona opções de linkagem estática
    target_link_options(main_program PRIVATE -static -static-libgcc -static-libstdc++)
endif()
