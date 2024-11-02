#include "tabela_simbolos.hpp"
#include <iostream>

// Implementação da função push
void Tabela_simbolos::insere_tabela(string lexema,  string memoria, string tipo, char escopo) {
    Tabela linha;
    linha.lexema = lexema;
    linha.memoria = memoria;
    linha.tipo = tipo;
    linha.escopo = escopo;
    pilha.push_back(linha);
}


// Implementação da função imprimirPilha
void Tabela_simbolos::imprimirPilha() const {
    std::cout << "Conteudo da Pilha: \n";
    for (const auto& simbolo : pilha) {
        std::cout << "Lexema: " << simbolo.lexema 
                  << ", Tipo: " << simbolo.tipo 
                  << ", Memoria: " << simbolo.memoria 
                  << ", Escopo: " << simbolo.escopo << std::endl;
    }
}

bool Tabela_simbolos::pesquisa_duplicvar_tabela(string lexema){
    int max = pilha.size() - 1;

    for(int i = max; i >= 0 ; i--){

        if(pilha[i].lexema == lexema){
            return true;
        }

        if(pilha[i].escopo == '*'){ //chegou no escopo saiu
            break;
        }

    }
    return false;
}

bool Tabela_simbolos::pesquisa_declvar_tabela(string lexema){ //essa é pra qnd for chamar a varivel pra ver se ela existe
    int max = pilha.size() - 1;

    for(int i = max; i >= 0 ; i--){

        if(pilha[i].lexema == lexema &&(pilha[i].tipo == "sinteiro" || pilha[i].tipo == "sbooleano")){
            return true;
        }

    }
    return false;
}

bool Tabela_simbolos::pesquisa(string lexema){
  int max = pilha.size() - 1;

    for(int i = max; i >= 0 ; i--){

        if(pilha[i].lexema == lexema){
            return true;
        }

    }
    return false;
}

string Tabela_simbolos::pesquisa_tipovar_tabela(string lexema){ //essa é pra qnd for ver o tipo da variavel
    int max = pilha.size() - 1;

    for(int i = max; i >= 0 ; i--){

        if(pilha[i].lexema == lexema && (pilha[i].tipo == "sinteiro" || pilha[i].tipo == "sbooleano")){
            return pilha[i].tipo;
        }

    }
    return " ";
}

void Tabela_simbolos::coloca_tipo_func(string lexema, string tipo){ //
    int max = pilha.size() - 1;

    for(int i = max; i >= 0 ; i--){

        if(pilha[i].lexema == lexema && pilha[i].tipo == "funcao "){
            pilha[i].tipo = pilha[i].tipo + tipo;
        }

    }
}

string Tabela_simbolos::pesquisa_tipo_proc_func_tabela(string lexema){//essa é pra qnd for chamar a func/proc pra ver se ela existe
    int max = pilha.size() - 1;
    string eu;
    for(int i = max; i >= 0 ; i--){

        if(pilha[i].lexema == lexema && pilha[i].tipo[0] != 's'){ //olha se o tipo n começa com s se começar é variavel
            if(pilha[i].tipo == "procedimento"){
                return pilha[i].tipo;
            }
            else if(pilha[i].tipo == "funcao sbooleano"){
                return "sbooleano";
            }
            else{
                return "sinteiro";
            }
            
        }

    }
    return "error";
}

bool Tabela_simbolos::pesquisa_decl_proc_func_tabela(string lexema){//essa é pra qnd for chamar a func/proc pra ver se ela existe
    int max = pilha.size() - 1;

    for(int i = max; i >= 0 ; i--){

        if(pilha[i].lexema == lexema && pilha[i].tipo[0] != 's'){ //olha se o tipo n começa com s se começar é variavel
            return true;
        }

    }
    return false;
}

void Tabela_simbolos::coloca_tipo_tabela_var(string tipo){
    int max = pilha.size() - 1;

    while(pilha[max].tipo == "var"){
        pilha[max].tipo = tipo;
        max--;
    }
    
}

void Tabela_simbolos::desempilha(){
     
    int max = pilha.size() - 1;

    while(pilha[max].escopo != '*'){
        pilha.pop_back();  
        max = pilha.size() - 1;   
    }

    pilha[max].escopo = ' ';
}




