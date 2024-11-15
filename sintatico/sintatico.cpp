// sintatico.cpp
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip> // Necessário para std::setw
#include <iostream>
#include <sstream>
#include <stdexcept> // Para runtime_error
#include <string>
#include <unordered_map>
#include <vector>

// Inclua os headers necessários
#include "sintatico.hpp"
#include "lexical.hpp"
#include "tabela_simbolos.hpp"
#include "geracao_codigo.hpp"

// Definição do lexer global
LexicalAnalyzer lexer;

// Definição do gerador de codigo
GeracaoCodigo gerador;

struct Args {
    int arg1;
    int arg2;
};

int END = 0;
int counter=0;
int ex_counter = 0;
int rot = 1;
vector<Args> argumentos;


std::string rotulo(){
    std::string rotul = "L" + to_string(rot);
    rot++;
    return rotul;
}

// Definição da tabela de simbolos
Tabela_simbolos tabela;


vector<string> posfix;


// Implementação da função Lexico()

bool guardar = false; //determina se vai guardar ou n a expressão para conferir coisas do semantico

bool unit = false;

std::vector<std::string> lista_expressao;

#include <stack>
#include <vector>
#include <string>
#include <cstdlib> // Para exit(EXIT_FAILURE)
#include <iostream> // Para std::cerr


using namespace std;
// Função única para converter a expressão infixa para pós-fixa

void limpa_expresao(){
    while(!lista_expressao.empty()){
        lista_expressao.pop_back();
    }
}

std::vector<std::string> infixToPostfix() {

    // Função interna para determinar a precedência dos operadores
    auto precedencia = [](const std::string& op) -> int {
        if (op == "nao" || op == "-u" || op == "+u")
            return 4;
        else if (op == "*" || op == "div")
            return 3;
        else if (op == "+" || op == "-")
            return 2;
        else if (op == "e")
            return 1;
        else if (op == "ou")
            return 0;
        else if (op == "=" || op == "!=" || op == "<" || op == "<=" || op == ">" || op == ">=")
            return -1;
        else
            return -2; // Para outros casos
    };

    // Função interna para verificar se o token é um operador
    auto ehOperador = [](const std::string& token) -> bool {
        return token == "+" || token == "-" || token == "*" || token == "div" ||
               token == "e" || token == "ou" || token == "nao" || token == "-u" || token == "+u" ||
               token == "=" || token == "!=" || token == "<" || token == "<=" || token == ">" || token == ">=";
    };

    // Função interna para verificar se o token é um operador unário
    auto ehUnaryOperator = [](const std::string& op) -> bool {
        return op == "nao" || op == "-u" || op == "+u";
    };

    // Função interna para verificar se o token é um operando
    auto ehOperando = [&](const std::string& token) -> bool {
        // Aqui você pode adicionar verificações adicionais se necessário
        return !ehOperador(token) && token != "(" && token != ")";
    };

    std::vector<std::string> saida;
    std::stack<std::string> pilha;

    for (size_t i = 0; i < lista_expressao.size(); ++i) {
        std::string token = lista_expressao[i];

        if (ehOperando(token)) {
            // Se for um operando, adiciona à saída
            saida.push_back(token);

            // Verificar se o topo da pilha é um operador unário
            while (!pilha.empty() && ehUnaryOperator(pilha.top())) {
                saida.push_back(pilha.top());
                pilha.pop();
            }
        }
        else if (token == "(") {
            pilha.push(token);
        }
        else if (token == ")") {
            while (!pilha.empty() && pilha.top() != "(") {
                saida.push_back(pilha.top());
                pilha.pop();
            }
            if (!pilha.empty() && pilha.top() == "(") {
                pilha.pop();
            }
            else {
                std::cerr << "Erro: Parênteses não balanceados." << std::endl;
                cout << "LINHA: " << lexer.getLinhaAtual() << endl;
                exit(EXIT_FAILURE);
            }

            // Após fechar parênteses, verificar por operadores unários
            while (!pilha.empty() && ehUnaryOperator(pilha.top())) {
                saida.push_back(pilha.top());
                pilha.pop();
            }
        }
        else if (ehOperador(token)) {
            if (ehUnaryOperator(token)) {
                // Operadores unários são empilhados imediatamente
                pilha.push(token);
            }
            else {
                while (!pilha.empty() && precedencia(pilha.top()) >= precedencia(token)) {
                    saida.push_back(pilha.top());
                    pilha.pop();
                }
                pilha.push(token);
            }
        }
        else {
            std::cerr << "Erro: Token desconhecido '" << token << "'." << std::endl;
            cout << "LINHA: " << lexer.getLinhaAtual() << endl;
            exit(EXIT_FAILURE);
        }
    }

    // Após processar todos os tokens, desempilha os operadores restantes
    while (!pilha.empty()) {
        if (pilha.top() == "(" || pilha.top() == ")") {
            std::cerr << "Erro: Parênteses não balanceados." << std::endl;
            cout << "LINHA: " << lexer.getLinhaAtual() << endl;
            exit(EXIT_FAILURE);
        }
        saida.push_back(pilha.top());
        pilha.pop();
    }

    limpa_expresao();
    return saida;
}

#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include <cstdlib> // Para exit()
#include <cctype>  // Para isdigit()

using namespace std;

string analisa_tipo_expressao_semantico() {

    posfix = infixToPostfix();
    // Imprimir a expressão pós-fixa para depuração

    /*cout << "Expressao posfixa: ";
    for (const string& t : posfix) {
        cout << t << " ";
    }
    cout << endl;*/
    
    
    stack<string> pilha;
    
    for (size_t i = 0; i < posfix.size(); ++i) {
        string token = posfix[i];
        
        if (token == "+" || token == "-" || token == "*" || token == "div" ||
            token == "=" || token == "!=" || token == "<" || token == "<=" || token == ">" || token == ">=" ||
            token == "e" || token == "ou") {
            
            // Operadores binários
            if (pilha.size() < 2) {
                cout << "Erro: Operador '" << token << "' requer dois operandos." << endl;
                exit(EXIT_FAILURE);
            }
            string tipoOperando2 = pilha.top(); pilha.pop();
            string tipoOperando1 = pilha.top(); pilha.pop();
            
            string tipoResultado;
            
            if (token == "+" || token == "-" || token == "*" || token == "div") {
                if (tipoOperando1 == "sinteiro" && tipoOperando2 == "sinteiro") {
                    tipoResultado = "sinteiro";
                } else {
                    cout << "Erro semântico: Operador '" << token << "' requer operandos inteiros." << endl;
                    exit(EXIT_FAILURE);
                }
            } else if (token == "=" || token == "!=" || token == "<" || token == "<=" || token == ">" || token == ">=") {
                if (tipoOperando1 == "sinteiro" && tipoOperando2 == "sinteiro") {
                    tipoResultado = "sbooleano";
                } else {
                    cout << "Erro semântico: Operador '" << token << "' requer operandos inteiros." << endl;
                    exit(EXIT_FAILURE);
                }
            } else if (token == "e" || token == "ou") {
                if (tipoOperando1 == "sbooleano" && tipoOperando2 == "sbooleano") {
                    tipoResultado = "sbooleano";
                } else {
                    cout << "Erro semântico: Operador '" << token << "' requer operandos booleanos." << endl;
                    exit(EXIT_FAILURE);
                }
            } else {
                cout << "Erro: Operador desconhecido '" << token << "'." << endl;
                exit(EXIT_FAILURE);
            }
            
            pilha.push(tipoResultado);
        } else if (token == "nao" || token == "-u" || token == "+u") {
            // Operadores unários
            if (pilha.empty()) {
                cout << "Erro: Operador '" << token << "' requer um operando." << endl;
                exit(EXIT_FAILURE);
            }
            string tipoOperando = pilha.top(); pilha.pop();
            
            string tipoResultado;
            
            if (token == "-u" || token == "+u") {
                if (tipoOperando == "sinteiro") {
                    tipoResultado = "sinteiro";
                } else {
                    cout << "Erro semântico: Operador '" << token << "' requer operando inteiro." << endl;
                    exit(EXIT_FAILURE);
                }
            } else if (token == "nao") {
                if (tipoOperando == "sbooleano") {
                    tipoResultado = "sbooleano";
                } else {
                    cout << "Erro semântico: Operador '" << token << "' requer operando booleano." << endl;
                    exit(EXIT_FAILURE);
                }
            } else {
                cout << "Erro: Operador desconhecido '" << token << "'." << endl;
                exit(EXIT_FAILURE);
            }
            
            pilha.push(tipoResultado);
        } else if (token == "verdadeiro" || token == "falso") {
            // Constantes booleanas
            pilha.push("sbooleano");
        } else if (isdigit(token[0])) {
            // Números inteiros
            pilha.push("sinteiro");
        } else {
            // Deve ser um identificador (variável ou função)
            if (tabela.pesquisa_declvar_tabela(token)) {
                // Variável declarada
                string tipo = tabela.pesquisa_tipovar_tabela(token);
                pilha.push(tipo);
                
            } else if (tabela.pesquisa_decl_proc_func_tabela(token)) {
                string tipo = tabela.pesquisa_tipo_proc_func_tabela(token);
                if(tipo == "procedimento"){
                    cout << "Erro semantico: Identificador '" << token << "' utilizado como variavel, mas e um procedimento." << endl;
                    exit(EXIT_FAILURE);
                }
                else{
                    pilha.push(tipo);
                }
                
            } else {
                // Identificador não declarado
                cout << "Erro semântico: Variável '" << token << "' não declarada." << endl;
                exit(EXIT_FAILURE);
            }
        }
    }
    
    if (pilha.size() != 1) {
        cout << "Erro: Expressão inválida, pilha de tipos inconsistente." << endl;
        exit(EXIT_FAILURE);
    }
    
    string tipoExpressao = pilha.top();

    return tipoExpressao;
}


Token Lexico() {
    Token token = lexer.Lexic();
    if(guardar){
        if(unit == true){
            string holer= lista_expressao.back();
            lista_expressao.pop_back();
            lista_expressao.push_back(holer + 'u');
            unit = false;
        }
        //cout<< "Lexema addna lista: " << token.lexema << endl;
        lista_expressao.push_back(token.lexema);
    }
    
    return token;
}


void gera_expressao(){
    for(int i = 0; i < int(posfix.size()); i++){
        string item = posfix.at(i);
        cout << item << endl;
        if(tabela.pesquisa_declvar_tabela(item)){
            string endereco = tabela.proucura_end(item);
            gerador.gera("","LDV",endereco,"");
        }
        else{
            if(item == "+"){
                 gerador.gera("","ADD","","");
            }
            else if(item == "-"){
                gerador.gera("","SUB","","");
            }
            else if(item == "*"){
                gerador.gera("","MULT","","");
            }
            else if(item == "div"){
                gerador.gera("","DIVI","","");
            }
            else if(tabela.pesquisa_decl_proc_func_tabela(item)){
                //gerador.gera("","CALL","","");             ///////FALTA IMPLEMETAR O ROTULO
                gerador.gera("","LDV","0","");
            }
            else if(item == ">"){
                gerador.gera("","CMA","","");
            }
            else if(item == "<"){
                gerador.gera("","CME","","");
            }
            else if(item == "="){
                gerador.gera("","CEQ","","");
            }
            else if(item == "!="){
                gerador.gera("","CDIF","","");
            }
            else if(item == "<="){
                gerador.gera("","CMEQ","","");
            }
            else if(item == ">="){
                gerador.gera("","CMAQ","","");
            }
            else if(item == "-u"){
                gerador.gera("","INV","","");
            }
            else if(item == "+u"){
                continue;
            }
            else if(item == "verdadeiro"){
                gerador.gera("","LDC","1","");
            }
            else if(item == "falso"){
                gerador.gera("","LDC","0","");
            }
            else if(item == "e"){
                gerador.gera("","AND","","");
            }
            else if(item == "ou"){
                gerador.gera("","OR","","");
            }
            else if(item == "nao"){
                gerador.gera("","NEG","","");
            }
            else{
                gerador.gera("","LDC",item,"");
            }
        }
    }
}



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

Token analisa_tipo(Token token){
    if(token.simbolo != "sinteiro" && token.simbolo != "sbooleano"){
        cout << "tipo invalido --> "<< token.simbolo << " esperado booleano ou inteiro" << endl;
        cout << "LINHA: " << lexer.getLinhaAtual() << endl;
        exit(EXIT_FAILURE);
    }
    else{
        tabela.coloca_tipo_tabela_var(token.simbolo); //SEMANTICO
        return Lexico();
    }
}

Token analisa_variaveis(Token token){
    //certo
    while(token.simbolo != "sdoispontos"){
        if(token.simbolo == "sidentificador"){

            if(tabela.pesquisa_duplicvar_tabela(token.lexema)){                     //SEMANTICO
                cout << "Nome de variavel repetida" << endl; 
                cout << "LINHA: " << lexer.getLinhaAtual() << endl;
                //tabela.imprimirPilha();
                exit(EXIT_FAILURE);
            }

            tabela.insere_tabela(token.lexema, to_string(END), "var", ' ');             //SEMANTICO

            counter++;                                                                  //GERAÇÃO DE CODIGO
            END++;

            token = Lexico();
            //certo
            if(token.simbolo == "svirgula" || token.simbolo == "sdoispontos"){
                if(token.simbolo == "svirgula"){
                    token = Lexico();
                    if(token.simbolo == "sdoispontos"){
                        cout << "comando ',:' nao existe" << endl;
                        cout << "LINHA: " << lexer.getLinhaAtual() << endl;
                        exit(EXIT_FAILURE); 
                    }
                }
            }
            else{
                cout << "faltou virgula ou dois pontos após o nome da variavel "<< token.lexema << endl;
                cout << "LINHA: " << lexer.getLinhaAtual() << endl;
                exit(EXIT_FAILURE);
            }
        }
        else{
            cout << "esperado nome da variavel" << token.simbolo << " " << token.lexema<< endl;
            cout << "LINHA: " << lexer.getLinhaAtual() << endl;
            exit(EXIT_FAILURE);
        }
    }

    token = Lexico();
    //certo
    token = analisa_tipo(token);
    //certo
    return token; 
}

Token analisa_et_variaveis(Token token){ 
    ex_counter = counter;
    counter = 0;
    int backup_end = END;

    if(token.simbolo == "svar"){
        //certo
        
        token = Lexico();
        if(token.simbolo == "sidentificador"){
            //chega aqui
            
            while(token.simbolo == "sidentificador"){
                token = analisa_variaveis(token);

                //certo
                if(token.simbolo == "sponto_virgula")
                    token = Lexico(); 
                else{
                  cout << "esperado ponto e virgula apos o tipo na declaracao de variaveis" << endl; 
                  cout << "LINHA: " << lexer.getLinhaAtual() << endl;
                  exit(EXIT_FAILURE); 
                }
                
            }
            
             //certo
             Args arg;
             arg.arg1 =  backup_end;
             arg.arg2 = counter;
             argumentos.push_back(arg);

             gerador.gera("","ALLOC",to_string(backup_end),to_string(counter));                              //GERAÇÃO DE CODIGO
             return token; 
        }
        else{
            cout << "Esperado nome da variavel" << endl;
            cout << "LINHA: " << lexer.getLinhaAtual() << endl;
            exit(EXIT_FAILURE);
        }
    }
    return token;
}

Token analisa_declaracao_proc(Token token){ //retorna ; dps do fim 
    token = Lexico();
    //certo
    if(token.simbolo == "sidentificador"){
        if(tabela.pesquisa_decl_proc_func_tabela(token.lexema)){        //SEMANTICO
            cout << "Nome de procedimento duplicado" << endl;
            cout << "LINHA: " << lexer.getLinhaAtual() << endl;
            exit(EXIT_FAILURE);
        }

        string aux_rot = rotulo(); //vai guardar o valor pq muda qnd chama a FUNC DNV
        tabela.insere_tabela(token.lexema, aux_rot, "procedimento", '*');  //SEMANTICO

        gerador.gera(aux_rot,"NULL","","");                               // GERAÇÃO DE CODIGO  

        token = Lexico();
        if(token.simbolo == "sponto_virgula"){
            
            token = analisa_bloco();

            int desempilhados = tabela.desempilha();                                      //SEMANTICO
            if(desempilhados != 0){
                Args aux;
                aux = argumentos.back();
                argumentos.pop_back();
                gerador.gera("","DALLOC",to_string(aux.arg1),to_string(aux.arg2));
            }
                 

            gerador.gera("","RETURN","","");                             // GERAÇÃO DE CODIGO

            return token;
        }
        else{
            cout << "faltou ; apos o nome do procedimento" << endl;
            cout << "LINHA: " << lexer.getLinhaAtual() << endl;
            exit(EXIT_FAILURE);
        }
    }
    else{
        cout << "esperado nome do procedimento" << endl;
        cout << "LINHA: " << lexer.getLinhaAtual() << endl;
        exit(EXIT_FAILURE);
    }
}

Token analisa_declaracao_func(Token token){ //retorna ; dps do fim
    token = Lexico();
    if(token.simbolo == "sidentificador"){
        if(tabela.pesquisa_decl_proc_func_tabela(token.lexema)){                //SEMANTICO
            cout << "Nome da funcao duplicado" << endl;
            cout << "LINHA: " << lexer.getLinhaAtual() << endl;
            exit(EXIT_FAILURE);
        }
        string nome_da_func = token.lexema;                                     //SEMANTICO
        string aux_rot = rotulo();
        tabela.insere_tabela(token.lexema,aux_rot, "funcao ", '*');                //SEMANTICO

        gerador.gera(aux_rot,"NULL","","");

        token = Lexico();
        
        if(token.simbolo == "sdoispontos"){
            token = Lexico();                                                   //SEMANTICO
            if(token.simbolo != "sinteiro" && token.simbolo != "sbooleano"){
                cout << "tipo invalido --> "<< token.simbolo << " esperado booleano ou inteiro" << endl;
                cout << "LINHA: " << lexer.getLinhaAtual() << endl;
                exit(EXIT_FAILURE);
            }
            else{
                tabela.coloca_tipo_func(nome_da_func, token.simbolo); 
                token = Lexico();
            }                                                                   //SEMANTICO

            if(token.simbolo == "sponto_virgula"){
                token = analisa_bloco();
                int desempilhados = tabela.desempilha(); 
                                                         //SEMANTICO
                if(desempilhados != 0){
                    Args aux;
                    aux = argumentos.back();
                    argumentos.pop_back();
                    gerador.gera("","DALLOC",to_string(aux.arg1),to_string(aux.arg2));
                }
                
                gerador.gera("","RETURN","","");                             // GERAÇÃO DE CODIGO

                return token;
                //retorna coisa dps do fim
            }
            else{
                cout << "faltou o ponto e virgula" << endl;
                cout << "LINHA: " << lexer.getLinhaAtual() << endl;
                exit(EXIT_FAILURE);
            }
        }
        else{
            cout << "esperado dois pontos após o nome da função" << endl;
            cout << "LINHA: " << lexer.getLinhaAtual() << endl;
            exit(EXIT_FAILURE);
        }

    }
    else{
        cout << "esperado o nome da função" << endl;
        cout << "LINHA: " << lexer.getLinhaAtual() << endl;
        exit(EXIT_FAILURE);
    }
}

Token analisa_sub_rotinas(Token token){ //retorn oq tem depois do ; do fim
    int flag = 0;
    string aux_rot;

    if(token.simbolo == "sprocedimento" || token.simbolo == "sfuncao"){                 //GERAÇÃO DE CODIGO
        aux_rot = rotulo();
        gerador.gera("","JMP",aux_rot,"");
        flag = 1;
    }

    while(token.simbolo == "sprocedimento" || token.simbolo == "sfuncao"){

        if(token.simbolo == "sprocedimento"){
            token = analisa_declaracao_proc(token);
        }
        else{
            token = analisa_declaracao_func(token);
        }

        if(token.simbolo == "sponto_virgula"){
            token = Lexico();
        }
        else{
            cout << "Faltou o ; do fim no procedimento:(" << endl; 
            cout << "LINHA: " << lexer.getLinhaAtual() << endl;
            exit(EXIT_FAILURE);
        }
    }


    if(flag){
        gerador.gera(aux_rot,"NULL","","");                                     //GERAÇÃO DE CODIGO
    }

    return token; //Fica de olho prta vcer se ta certo
}

Token analisa_atribuicao(Token token, string variavel){
    token = Lexico();

    if(tabela.pesquisa_declvar_tabela(variavel)){
        //////////////////////////////////////////////////////////////////////SEMANTICO 
            guardar = true;
            lista_expressao.push_back(token.lexema);                //SEMANTICO

            token = analisa_expressao(token);

            guardar=false;
            lista_expressao.pop_back();                             //SEMANTICO

            string tipo = analisa_tipo_expressao_semantico();

            string tipo_var = tabela.pesquisa_tipovar_tabela(variavel);

            if(tipo != tipo_var){
                cout << "Tentou atribuir expressao de tipo diferente do que a variavel suporta" << endl;
                cout << "LINHA: " << lexer.getLinhaAtual() << endl;
                exit(EXIT_FAILURE);
            }

        //////////////////////////////////////////////////////////////////////SEMANTICO
//soma := a+c
            
            gera_expressao();                                                                   //GERAÇÃO DE CODIGO
            gerador.gera("","STR",tabela.proucura_end(variavel),"");
            return token;
    }
    else{ // é função
            guardar = true;
            lista_expressao.push_back(token.lexema);                //SEMANTICO

            token = analisa_expressao(token);

            guardar=false;
            lista_expressao.pop_back();                             //SEMANTICO

            //lista_expressao
            
          /*  for(int i= 0; i < int(lista_expressao.size()) ; i++){
                //cout << "entrei aqui?? " << token.lexema << endl;
                if(tabela.pesquisa_declvar_tabela(lista_expressao.at(i))){
                    if(tabela.pesquisa_duplicvar_tabela(lista_expressao.at(i))){
                        continue;
                    }
                    else{
                         cout << "funcao retorna variavel alem do escopo" << endl;
                         cout << "LINHA: " << lexer.getLinhaAtual() << endl;
                         exit(EXIT_FAILURE);
                    }
                }
            }*/

            string tipo = analisa_tipo_expressao_semantico();

            string tipo_func = tabela.pesquisa_tipo_proc_func_tabela(variavel);

            if(tipo != tipo_func){
                cout << "Tentou atribuir expressao de tipo diferente do que a funcao suporta" << endl;
                cout << "LINHA: " << lexer.getLinhaAtual() << endl;
                exit(EXIT_FAILURE);
            }

            //GERAÇÃO DE CODIGO
            gera_expressao(); 
            gerador.gera("","STR","0","");
            //GERAÇÃO DE CODIGO

            return token;

        //////////////////////////////////////////////////////////////////////SEMANTICO


    }
            
}

Token chamada_procedimento(Token token, string variavel){

    string aux_rot = tabela.pesquisa_end_proc_func(variavel);           //GERAÇÃO DE CODIGO
    gerador.gera("","CALL",aux_rot,"");

    return token;
}

Token analisa_chamada_funcao(Token token){

    string aux_rot = tabela.pesquisa_end_proc_func(token.lexema);           //GERAÇÃO DE CODIGO
    gerador.gera("","CALL",aux_rot,"");

    token = Lexico();
    return token;
}

Token analisa_atrib_chprocedimento(Token token, string variavel){
    //certo 
    if(token.simbolo == "satribuicao"){

        token = analisa_atribuicao(token, variavel); 
        //retorna quem está dps da exrpessão (normalmente ;)
    }
    else{
        token = chamada_procedimento(token, variavel);
        //retorna quem está dps do indentificador (normalmente ;)
    }
    return token;
}

Token analisa_se(Token token){
    //id
    bool senao = false;                                     //ISSO SERVE PARA VERIFICAR SE PRECISA CRIAR UMA LABEL EXTRA PARA O SENÃO, ou seja se tem ou n o senao
    string aux_rot = rotulo();                              //GERAÇÃO DE CODIGO
    string aux_rot2;

    guardar = true;
    lista_expressao.push_back(token.lexema);                //SEMANTICO

    token = analisa_expressao(token);

    guardar=false;
    lista_expressao.pop_back();                             //SEMANTICO

    string tipo = analisa_tipo_expressao_semantico();

    if(tipo != "sbooleano"){
         cout << "Expressao do se nao eh booleano" << endl;
         cout << "LINHA: " << lexer.getLinhaAtual() << endl;
         exit(EXIT_FAILURE);
    }

    gera_expressao();
    gerador.gera("","JMPF",aux_rot,"");                              //GERAÇÃO DE CODIGO

    //GERAÇÃO PRECISA VER ONDE VOU COLOCAR O GERA EXPRESSAO  gera_expressao(); 

    if(token.simbolo == "sentao"){
        //certo
        token = Lexico();
        //certo
        if(token.simbolo == "sinicio"){
            token = analisa_comandos(token); // e ver oq faz com o freitas
            //gerador.gera("","JMP",aux_rot2,"");
            cout << "entrei aqui no comando composto do se__ parei pq n ta implementado" << token.lexema << endl; // dps tira essa bosta pra funfar isso n é erro n
            if(token.simbolo == "sfim"){ //eu que coloquei isso aqui
                return token;
            }
        }

        //aqui chega um senao
        if(token.simbolo != "ssenao"){
            cout << "OQ SAI AQUI1: " << token.lexema << endl;
            token = analisa_comando_simples(token);
            cout << "OQ SAI AQUI2: " << token.lexema << endl;
           // gerador.gera("","JMP",aux_rot2,"");
        }
            

        if(token.simbolo != "ssenao"){ //Eu que fiz para acomodar os comandos compostos
            if(token.simbolo == "sponto_virgula")
                token = Lexico();
        }
        
        if(token.simbolo == "ssenao"){
            senao = true;
            aux_rot2 = rotulo();
            gerador.gera("","JMP",aux_rot2,""); 
            gerador.gera(aux_rot,"NULL","","");                              //GERAÇÃO DE CODIGO
            //aux_rot = rotulo();
   
            //cout << "ENTREI NO SENAO E SE DER ALGUM ERRO FUTURO SINTATICO VOLTA AQUI PRA RESOLVER" << endl;
            token = Lexico();

            if(token.simbolo == "sinicio"){
                token = analisa_comandos(token); // e ver oq faz com o freitas
                if(token.simbolo == "sfim"){ //eu que coloquei isso aqui
                    //gerador.gera(aux_rot2,"NULL","","");
                    return token;
                }
            }
            else{
                token = analisa_comando_simples(token);
                
                if(token.simbolo == "sfim"){
                    gerador.gera(aux_rot2,"NULL","","");
                    return token;
                }

                token = Lexico();
                //gerador.gera(aux_rot2,"NULL","","");
            }
            

            //if(token.simbolo != "sfim"){ //eu que coloquei para funcionar quando tem comandos depois de um inicio e fim do enquanto 
              //  token = analisa_comando_simples(token);
            //}

        }

        //string aux_rot2 = rotulo();
        if(!senao){
            gerador.gera(aux_rot,"NULL","","");   //GERAÇÃO DE CODIGO
        }
        else{
            gerador.gera(aux_rot2,"NULL","","");
        }
        return token;
    }

    else{
        cout << "faltou o então" << endl;
        cout << "LINHA: " << lexer.getLinhaAtual() << endl;
        exit(EXIT_FAILURE);
    }
}
    
    

Token analisa_fator(Token token){
    if (token.simbolo == "sidentificador") {
        // Variável ou Função
/////////////////////////////////////////////////////////////////////SEMANTICO

        if(tabela.pesquisa(token.lexema)){
            if(tabela.pesquisa_decl_proc_func_tabela(token.lexema)){
                                                                       //SEMANTICO
                token = analisa_chamada_funcao(token);
            }

            else{
                token = Lexico();
            }

        }

        else{
             cout << "identificador ou func n declarada" << endl;
             cout << "LINHA: " << lexer.getLinhaAtual() << endl;
             exit(EXIT_FAILURE);
        }       
    }

/////////////////////////////////////////////////////////////////////SEMANTICO 

    else if (token.simbolo == "snumero") {
        // Número
        token = Lexico();
    }
    else if (token.simbolo == "snao") {
        // Operador NÃO
        token = Lexico();
        token = analisa_fator(token);
    }
    else if (token.simbolo == "sabre_parenteses") {
        // Expressão entre parênteses
        token = Lexico();
        token = analisa_expressao(token);
        if (token.simbolo == "sfecha_parenteses") {
            token = Lexico();
        }
        else {
            cout << "ERRO: Esperado ')' após expressão" << endl; 
            cout << "LINHA: " << lexer.getLinhaAtual() << endl;
            exit(EXIT_FAILURE);
        }
    }
    else if (token.simbolo == "sverdadeiro" || token.simbolo == "sfalso") {
        // Constante booleana
        token = Lexico();
    }
    else {
        cout << "ERRO: Fator inválido" << endl;
        cout << "LINHA: " << lexer.getLinhaAtual() << endl;
        exit(EXIT_FAILURE);

    }
    return token;  
}

Token analisa_termo(Token token){

    token = analisa_fator(token);
    while(token.simbolo == "smult" || token.simbolo == "sdiv" || token.simbolo == "se"){
        token = Lexico();
        token = analisa_fator(token);
    }
    return token;
}

Token analisa_expressao_simples(Token token){ // AQUI ELE TA FLANDO QUE PRECISA COMEÇAR SEMPRE COM + OU - ? SE NÃO FALTA UM RETURN PARA QND N COMEÇAR 
    
    if(token.simbolo == "smais" || token.simbolo == "smenos"){          //
        unit = true;
        token = Lexico();
        unit = false;
    }

    token = analisa_termo(token);
    //certo
    while(token.simbolo == "smais" || token.simbolo == "smenos" || token.simbolo == "sou"){
        token = Lexico();
        token = analisa_termo(token);
    }
    return token;

}

Token analisa_expressao(Token token){ //retorna o prox dps da expressão

    //lista_expressao.push_back(token.lexema);        //insere o primeiro lexema na lista 
                                                    //Todos os Lexema() vão guardar os lexemas


    token = analisa_expressao_simples(token);
    if(token.simbolo == "smaior" || token.simbolo == "smaiorig" || token.simbolo == "sig" ||
     token.simbolo == "smenor" || token.simbolo == "smenorig"|| token.simbolo == "sdif"){

        token = Lexico();
        token = analisa_expressao_simples(token);
    }

                                                   // Paramos de guardar a cada Lexema()
    
    return token;
}

Token analisa_enquanto(Token token){

    string aux_rot1, aux_rot2;

    aux_rot1 = rotulo();
    gerador.gera(aux_rot1,"NULL","","");                    //GERAÇÃO DE CODIGO

    guardar = true;
    lista_expressao.push_back(token.lexema);                //SEMANTICO

    token = analisa_expressao(token);

    guardar=false;
    lista_expressao.pop_back();                             //SEMANTICO

    string tipo = analisa_tipo_expressao_semantico();

    if(tipo != "sbooleano"){
         cout << "esperado expressao booleana no enquanto" << endl;
         cout << "LINHA: " << lexer.getLinhaAtual() << endl;
         exit(EXIT_FAILURE);
    }

    //GERAÇÃO PRECISA VER ONDE VOU COLOCAR O GERA EXPRESSAO  gera_expressao(); 
    gera_expressao();
    
    if(token.simbolo == "sfaca"){

        aux_rot2 = rotulo();

        gerador.gera("","JUMPF",aux_rot2,"");                    //GERAÇÃO DE CODIGO

        token = Lexico();
        token = analisa_comando_simples(token);

        gerador.gera("","JUMP",aux_rot1,"");                    //GERAÇÃO DE CODIGO
        gerador.gera(aux_rot2,"NULL","","");

        return token;
    }
    else{
        cout << "esperado o faca" << endl;
        cout << "LINHA: " << lexer.getLinhaAtual() << endl;
        exit(EXIT_FAILURE);
    }

}

Token analisa_leia(Token token){
    
    string aux;                                                             //GERAÇÃO DE CODIGO
    
    if(token.simbolo == "sabre_parenteses"){
        token = Lexico();
        //certo
        
        if(token.simbolo == "sidentificador"){                          //SEMANTICO
            if(!tabela.pesquisa_declvar_tabela(token.lexema)){
                 cout << "No leia a variavel nao existe" << endl;
                 cout << "LINHA: " << lexer.getLinhaAtual() << endl;
                 exit(EXIT_FAILURE);
            }
            else{
                if(tabela.pesquisa_tipovar_tabela(token.lexema) != "sinteiro"){      //SEMANTICO
                    cout << "No leia a variavel deve ser inteira" << endl;
                    cout << "LINHA: " << lexer.getLinhaAtual() << endl;
                    exit(EXIT_FAILURE);
                }

                 aux = token.lexema;                                                //GERAÇÃO DE CODIGO
            
            }

            token = Lexico();
            //certo
            if(token.simbolo == "sfecha_parenteses"){

                gerador.gera("","RD","","");
                gerador.gera("","STR",tabela.proucura_end(aux),"");                 //GERAÇÃO DE CODIGO

                return Lexico();
                //certo
            }
            else{
                cout << "Precisa fechar o parenteses" << endl;
                cout << "LINHA: " << lexer.getLinhaAtual() << endl;
                exit(EXIT_FAILURE);
            }
        }
        else{
            cout << "esperado variavel a ser lida" << endl;
            cout << "LINHA: " << lexer.getLinhaAtual() << endl;
            exit(EXIT_FAILURE);
        }
    }
    else{
        cout << "faltou abrir o parenteses" << endl;
        cout << "LINHA: " << lexer.getLinhaAtual() << endl;
        exit(EXIT_FAILURE);
    }
}

Token analisa_escreva(Token token){
    //certo
     string aux;       //GERAÇÃO DE CODIGO

    if(token.simbolo == "sabre_parenteses"){
        token = Lexico();
        //certo
        if(token.simbolo == "sidentificador"){

            if(!tabela.pesquisa_declvar_tabela(token.lexema)){          //SEMANTICO
                 cout << "No escreva a variavel nao existe" << endl;  
                 cout << "LINHA: " << lexer.getLinhaAtual() << endl;
                 exit(EXIT_FAILURE);
            }
            else{
                if(tabela.pesquisa_tipovar_tabela(token.lexema) != "sinteiro"){      //SEMANTICO
                    cout << "No escreva a variavel deve ser inteira" << endl;
                    cout << "LINHA: " << lexer.getLinhaAtual() << endl;
                    exit(EXIT_FAILURE);
                }

                aux = token.lexema;                       //GERAÇÃO DE CODIGO
                
            }

            token = Lexico();
            //certo
            if(token.simbolo == "sfecha_parenteses"){
                token = Lexico();
                //retorna
                //certo


                gerador.gera("","LDV",tabela.proucura_end(aux),"");
                gerador.gera("","PRN","","");                 //GERAÇÃO DE CODIGO


                return token;
            }
            else{
                cout << "Precisa fechar o parenteses" << endl;
                cout << "LINHA: " << lexer.getLinhaAtual() << endl;
                exit(EXIT_FAILURE);
            }
        }
        else{
            cout << "esperado variavel a ser lida" << endl;
            cout << "LINHA: " << lexer.getLinhaAtual() << endl;
            exit(EXIT_FAILURE);
        } 
    }
    else{
        cout << "faltou abrir o parenteses" << endl;
        cout << "LINHA: " << lexer.getLinhaAtual() << endl;
        exit(EXIT_FAILURE);
    }
}

Token analisa_comando_simples(Token token){ //se for comando unico retorna ; || se for composto devolve oq ta depois do inicio
    
    if(token.simbolo == "sidentificador"){
        //certo
        string variavel = token.lexema;
        token = Lexico();
        token = analisa_atrib_chprocedimento(token, variavel);
        //retorna ;
    }

    else if(token.simbolo == "sse"){
        token = Lexico();
        token = analisa_se(token);

        //retorna certo
    }

    else if(token.simbolo == "senquanto"){
        token = Lexico();
        token = analisa_enquanto(token);

        if(token.simbolo != "sponto_virgula" && token.simbolo != "sfim"){ //eu que coloquei para funcionar quando tem comandos depois de um inicio e fim do enquanto 
            token = analisa_comando_simples(token);
        }

        //retorna certo
    }

    else if(token.simbolo == "sleia"){
        token = Lexico();
        token = analisa_leia(token);
        //retorna ;
    }

    else if(token.simbolo == "sescreva"){
        token = Lexico();
        token = analisa_escreva(token);
        //retorna ;
    }

    else{
        token = analisa_comandos(token);
    }
    //certo
    return token;
}

Token analisa_comandos(Token token){ //retorna oq tem dps do fim
    //cheguei certinho
    if(token.simbolo == "sinicio"){
        token = Lexico();
        //certo
        bool is_se = 0;
        if(token.simbolo == "sse"){
            is_se = 1;
        }

        token = analisa_comando_simples(token);
        //retorna ponta o virgula do comando
        //certo
        while(token.simbolo != "sfim"){
            //cout << "dentro do analisa comando: " << token.lexema << endl;
            if(token.simbolo == "sponto_virgula" || is_se){

                if(!is_se) // eu que fiz essa coisa inteiro do is_se 
                    token = Lexico();

                if(token.simbolo != "sfim"){
                    //certo
                    if(token.simbolo == "sponto"){
                        cout << "faltou colocar o fim do inicio: " << token.lexema << endl;
                        cout << "LINHA: " << lexer.getLinhaAtual() << endl;
                        exit(EXIT_FAILURE);
                    }
                    is_se = 0;
                    token = analisa_comando_simples(token);
                }
            }
            else{
                cout << "faltou ponto e virgula entre comandos: " << token.lexema << endl;
                cout << "LINHA: " << lexer.getLinhaAtual() << endl;
                exit(EXIT_FAILURE);
            }
        }
        token = Lexico();
        //comando retorna oq tem dps do fim
        return token;
    }
    else{
        cout << "faltou o delimitador inicio ou comandos " << token.lexema << endl;
        cout << "LINHA: " << lexer.getLinhaAtual() << endl;
        exit(EXIT_FAILURE);
    }
}

Token analisa_bloco(){
    Token token = Lexico();
    //aqui chega var
    token = analisa_et_variaveis(token);
    //retorna prox dps do ponto e virgula
    //certo
    token = analisa_sub_rotinas(token);
    //chega inicio certo
    token = analisa_comandos(token);
    //chega quem está depois do fim


    return token;
}



void sintatico(){
    Token token = Lexico();
    if(token.simbolo == "sprograma"){
        //certo

        gerador.gera("","START","","");                                     //GERAÇÃO DE CODIGO
        gerador.gera("","ALLOC",to_string(END),"1");  
        END++;

        token = Lexico();
        //certo
        if(token.simbolo == "sidentificador"){
            tabela.insere_tabela(token.lexema, " ", "nomeDePrograma", ' ');             //Insere na tabela de simbolos o programa declada
            token = Lexico();
            //certo
            if(token.simbolo == "sponto_virgula"){
                token = analisa_bloco();
                if(token.simbolo == "sponto"){
                    token = Lexico();
                    if(token.simbolo != ""){
                        cout << "comandos apos o ponto final nao sao aceitos" << endl;
                        cout << "LINHA: " << lexer.getLinhaAtual() << endl;
                        exit(EXIT_FAILURE);
                    }
                    else{
                        cout << "Nao apresenta erros sintaticos" << endl;
                        tabela.imprimirPilha();

                        if(!argumentos.empty()){
                            Args aux;
                            aux = argumentos.back();
                            argumentos.pop_back();
                            gerador.gera("","DALLOC",to_string(aux.arg1),to_string(aux.arg2));
                        }
                        

                        gerador.gera("","DALLOC","0","1");//perguinta
                        gerador.gera("","HLT","",""); 
                        gerador.imprimirCodigo();
                        return ;
                    }
                }
                else{
                    cout << "esperado ponto final" << endl;
                    cout << "LINHA: " << lexer.getLinhaAtual() << endl;
                    exit(EXIT_FAILURE);
                }
            }
            else{
                cout << "esperado ponto e virgula apos o nome do programa" << endl;
                cout << "LINHA: " << lexer.getLinhaAtual() << endl;
                exit(EXIT_FAILURE);
            }
        }
        else{
            cout << "faltou o nome do programa" << endl;
            cout << "LINHA: " << lexer.getLinhaAtual() << endl;
            exit(EXIT_FAILURE);
        }

    }
    else{
        cout << "faltou colocar programa" << endl;
        exit(EXIT_FAILURE);
    }
}