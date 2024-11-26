// geracao_codigo.hpp
#ifndef GERACAO_CODIGO_HPP
#define GERACAO_CODIGO_HPP

#include <string>
#include <vector>

class GeracaoCodigo {
public:
    GeracaoCodigo();

    // Método para gerar uma linha de código
    void gera(const std::string& rotulo, const std::string& instrucao, const std::string& atr1, const std::string& atr2);

    // Método para obter todo o código gerado
    const std::vector<std::string>& getCodigo() const;

    // Novo método para imprimir o código gerado
    void imprimirCodigo() const;

    void salvarEmArquivo(const std::string& nomeArquivo) const;

private:
    // Vetor para armazenar as linhas de código
    std::vector<std::string> codigo;
};

#endif // GERACAO_CODIGO_HPP
