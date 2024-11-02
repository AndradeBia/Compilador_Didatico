#ifndef TABELA_SIMBOLOS_HPP
#define TABELA_SIMBOLOS_HPP

#include <vector>
#include <string>

using namespace std;

// Estrutura do Token
struct Tabela {
    string lexema;
    string tipo;
    string memoria;
    char escopo;
};

class Tabela_simbolos {
private:
    vector<Tabela> pilha;  // Armazena objetos da estrutura Tabela

public:
    // Adiciona um elemento no topo
    void insere_tabela(string lexema,  string memoria, string tipo, char escopo = ' ');

    // Navegação direta para imprimir a pilha
    void imprimirPilha() const;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool pesquisa(string lexema);                               //verifica existencia do lexema

    void coloca_tipo_func(string lexema, string tipo);          //coloca o tipo na função 

    string pesquisa_tipovar_tabela(string lexema);              //com base no lexema busca o tipo da variavel na tabela de simbolos e retorna se ela é sinteira ou sbooleana

    bool pesquisa_duplicvar_tabela(string lexema);              //Pesquisa se tem variavel com esse lexema dentro do escopo 

    bool pesquisa_declvar_tabela(string lexema);                //pesquisa se a variavel existe na tabela de simbolos

    bool pesquisa_decl_proc_func_tabela(string lexema);         //essa é pra qnd for chamar a func/proc pra ver se ela existe

    void coloca_tipo_tabela_var(string tipo);                   //coloca tipo de variavel 

    void desempilha();    

    string pesquisa_tipo_proc_func_tabela(string lexema);                                   
    
};

#endif // TABELA_SIMBOLOS_HPP
