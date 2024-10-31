#include <iostream>
#include "lexical.hpp"
#include "sintatico.hpp"
#include <filesystem>

int main(int argc, char* argv[]) {
    std::string path;
    if (argc != 2) {
        std::cout << "Usando caminho padrão 'Source.txt'." << std::endl;
        path = std::filesystem::current_path().string() + "/Source.txt";
    } else {
        path = argv[1];
    }
    

    try {
        // Ler o arquivo fonte
        lexer.lerArquivo(path);

        // Iniciar a análise sintática
        sintatico();

    } catch(const std::exception& e) {
        std::cerr << "Erro: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
