// geracao_codigo.cpp
#include "geracao_codigo.hpp"
#include <iomanip>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream> // Incluído para permitir impressão no console

GeracaoCodigo::GeracaoCodigo() {
    // Implementação do construtor, se necessário
}

void GeracaoCodigo::gera(const std::string& rotulo, const std::string& instrucao, const std::string& atr1, const std::string& atr2) {
    // Cada campo é ajustado ao tamanho especificado com espaços à direita
    std::ostringstream linha;

    // Ajustar o rótulo para 4 caracteres
    linha << std::left << std::setw(4) << std::setfill(' ') << rotulo.substr(0, 4);

    // Ajustar a instrução para 8 caracteres
    linha << std::left << std::setw(8) << std::setfill(' ') << instrucao.substr(0, 8);

    // Ajustar ATR1 para 4 caracteres
    linha << std::left << std::setw(4) << std::setfill(' ') << atr1.substr(0, 4);

    // Ajustar ATR2 para 4 caracteres
    linha << std::left << std::setw(4) << std::setfill(' ') << atr2.substr(0, 4);

    // Adicionar a linha ao vetor de código
    codigo.push_back(linha.str());
}

const std::vector<std::string>& GeracaoCodigo::getCodigo() const {
    return codigo;
}


// Novo método para imprimir o código gerado
void GeracaoCodigo::imprimirCodigo() const {
    for (const auto& linha : codigo) {
        std::cout << linha << std::endl;
    }
}
