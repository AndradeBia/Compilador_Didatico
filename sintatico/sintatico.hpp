// sintatico.hpp
#ifndef SINTATICO_HPP
#define SINTATICO_HPP

#include <string>
#include "lexical.hpp" // Inclua o header do analisador léxico para acessar Token


// sintatico.hpp

// Declaração externa do lexer
extern LexicalAnalyzer lexer;


// Declaração das funções
Token Lexico(); //todo Lexico() precisa ser o metodo Lexico presente no lexical.cpp
Token analisa_tipo(Token token);
Token analisa_variaveis(Token token);
Token analisa_et_variaveis(Token token);
Token analisa_declaracao_proc(Token token);
Token analisa_declaracao_func(Token token);
Token analisa_sub_rotinas(Token token);
Token analisa_atribuicao(Token token, std::string variavel);
Token chamada_procedimento(Token token);
Token analisa_atrib_chprocedimento(Token token, std::string variavel);
Token analisa_se(Token token);
Token analisa_chamada_funcao(Token token);
Token analisa_fator(Token token);
Token analisa_termo(Token token);
Token analisa_expressao_simples(Token token);
Token analisa_expressao(Token token);
Token analisa_enquanto(Token token);
Token analisa_leia(Token token);
Token analisa_escreva(Token token);
Token analisa_comando_simples(Token token);
Token analisa_comandos(Token token);
Token analisa_bloco();
void sintatico();


std::string getBaseType(const std::string& tipo);

#endif // SINTATICO_HPP
