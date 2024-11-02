#ifndef LEXICAL_ANALYZER_H
#define LEXICAL_ANALYZER_H

#include <string>
#include <unordered_map>
#include <vector>

// Estrutura do Token
struct Token {
    std::string lexema;
    std::string simbolo;
};

class LexicalAnalyzer {
private:
    // Mapa de símbolos existente
    static const std::unordered_map<std::string, std::string> symbolMap;

    std::vector<Token> tokens;
    Token token_atual;

    // Atributos internos
    std::string arquivo; // Conteúdo do arquivo lido
    size_t i = 0;        // Índice atual na string 'arquivo'

    // NOVO: Contador de linhas
    int linhaAtual;

    // Métodos auxiliares
    int trata_digito(int i);
    int trata_letra(int i);
    int atribuicao(int i);
    int operador_aritmetico(int i);
    int operador_relacional(int i);
    int pontuacao(int i);
    int pega_token(int index);

public:
    // Construtor
    LexicalAnalyzer() : linhaAtual(1) {} // Inicializa linhaAtual como 1

    // Métodos existentes
    void lerArquivo(const std::string &path);
    void lexical();
    void visualizarTokens() const;
    Token getTokenAtual() const;
    void setTokenAtual(const Token &token);
    const std::vector<Token> &getTokens() const { return tokens; }
    Token Lexic();

    // NOVO: Método para obter a linha atual
    int getLinhaAtual() const;
};

#endif // LEXICAL_ANALYZER_H
