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
  static const std::unordered_map<std::string, std::string> symbolMap;
  std::vector<Token> tokens;
  Token token_atual;

  // Atributos internos
  std::string arquivo; // Conteúdo do arquivo lido
  size_t i = 0;        // Índice atual na string 'arquivo'

  // Métodos auxiliares
  int trata_digito(int i);
  int trata_letra(int i);
  int atribuicao(int i);
  int operador_aritmetico(int i);
  int operador_relacional(int i);
  int pontuacao(int i);
  int pega_token(int index);

public:
  void lerArquivo(const std::string &path);
  void lexical();
  void visualizarTokens() const;
  Token getTokenAtual() const;
  void setTokenAtual(const Token &token);
  const std::vector<Token> &getTokens() const { return tokens; }
  Token Lexic();
};

#endif
