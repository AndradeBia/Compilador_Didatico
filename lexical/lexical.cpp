// lexical.cpp

#include "lexical.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <iomanip>

// Definição do mapa de símbolos
const std::unordered_map<std::string, std::string> LexicalAnalyzer::symbolMap = {
    {"programa", "sprograma"},
    {"inicio", "sinicio"},
    {"fim", "sfim"},
    {"procedimento", "sprocedimento"},
    {"funcao", "sfuncao"},
    {"se", "sse"},
    {"entao", "sentao"},
    {"senao", "ssenao"},
    {"enquanto", "senquanto"},
    {"faca", "sfaca"},
    {":=", "satribuicao"},
    {"escreva", "sescreva"},
    {"leia", "sleia"},
    {"var", "svar"},
    {"inteiro", "sinteiro"},
    {"booleano", "sbooleano"},
    {"identificador", "sidentificador"},
    {"numero", "snumero"},
    {".", "sponto"},
    {";", "sponto_virgula"},
    {",", "svirgula"},
    {"(", "sabre_parenteses"},
    {")", "sfecha_parenteses"},
    {">", "smaior"},
    {">=", "smaiorig"},
    {"=", "sig"},
    {"<", "smenor"},
    {"<=", "smenorig"},
    {"!=", "sdif"},
    {"+", "smais"},
    {"-", "smenos"},
    {"*", "smult"},
    {"div", "sdiv"},
    {"e", "se"},
    {"ou", "sou"},
    {"nao", "snao"},
    {":", "sdoispontos"},
    {"verdadeiro", "sverdadeiro"},
    {"falso", "sfalso"}
};

void LexicalAnalyzer::lerArquivo(const std::string &path) {
    std::ifstream arquivo_entrada(path, std::ios::binary | std::ios::ate);

    if (!arquivo_entrada.is_open()) {
        throw std::runtime_error("Não foi possível abrir o arquivo.");
    }

    // Obter o tamanho do arquivo
    std::streamsize tamanho = arquivo_entrada.tellg();
    arquivo_entrada.seekg(0, std::ios::beg);

    std::stringstream buffer;
    buffer << arquivo_entrada.rdbuf();

    // Armazenar o conteúdo do arquivo no atributo interno
    this->arquivo = buffer.str();

    // NOVO: Reiniciar o contador de linhas ao ler um novo arquivo
    this->linhaAtual = 1;
    this->i = 0;
    this->tokens.clear();
}

int LexicalAnalyzer::trata_digito(int i) {
    Token token;
    std::string num;

    while (i < arquivo.size() && std::isdigit(arquivo[i])) {
        if (arquivo[i] == '\n') {
            linhaAtual++; // NOVO: Incrementa se encontrar uma nova linha
        }
        num += arquivo[i];
        i++;
    }

    token.lexema = num;
    token.simbolo = "snumero";
    tokens.push_back(token);
    this->setTokenAtual(token);

    return i;
}

int LexicalAnalyzer::trata_letra(int i) {
    Token token;
    std::string id;

    while (i < arquivo.size() && (std::isalpha(arquivo[i]) || arquivo[i] == '_' || std::isdigit(arquivo[i]))) {
        if (arquivo[i] == '\n') {
            linhaAtual++; // NOVO: Incrementa se encontrar uma nova linha
        }
        id += arquivo[i];
        i++;
    }

    token.lexema = id;
    auto it = symbolMap.find(token.lexema);
    if (it != symbolMap.end()) {
        token.simbolo = it->second;
    } else {
        token.simbolo = "sidentificador";
    }

    tokens.push_back(token);
    this->setTokenAtual(token);
    return i;
}

int LexicalAnalyzer::atribuicao(int i) {
    Token token;
    if ((i + 1) < arquivo.size() && arquivo[i + 1] == '=') {
        token.lexema = ":=";
        token.simbolo = "satribuicao";
        i += 2;
    } else {
        token.lexema = ":";
        token.simbolo = "sdoispontos";
        if (arquivo[i] == '\n') {
            linhaAtual++; // NOVO: Incrementa se ':' for seguido por '\n'
        }
        i += 1;
    }
    tokens.push_back(token);
    this->setTokenAtual(token);
    return i;
}

int LexicalAnalyzer::operador_aritmetico(int i) {
    Token token;
    std::string current(1, arquivo[i]);
    token.lexema = current;
    token.simbolo = symbolMap.at(current);
    if (arquivo[i] == '\n') {
        linhaAtual++; // NOVO: Incrementa se operador aritmético for '\n' (incomum)
    }
    i++;
    tokens.push_back(token);
    this->setTokenAtual(token);
    return i;
}

int LexicalAnalyzer::operador_relacional(int i) {
    Token token;
    if (arquivo[i] == '!') {
        if ((i + 1) < arquivo.size() && arquivo[i + 1] == '=') {
            token.simbolo = "sdif";
            token.lexema = "!=";
            i += 2;
            // Verificar se há '\n' entre '!' e '='
            if (arquivo[i - 2] == '\n') {
                linhaAtual++;
            }
        } else {
            std::cout << "Erro na linha " << getLinhaAtual() << ": '!' espera '=' em seguida..." << std::endl;
            i++;
        }
    } else if (arquivo[i] == '<') {
        if ((i + 1) < arquivo.size() && arquivo[i + 1] == '=') {
            token.simbolo = "smenorig";
            token.lexema = "<=";
            i += 2;
            if (arquivo[i - 2] == '\n') {
                linhaAtual++;
            }
        } else {
            token.simbolo = "smenor";
            token.lexema = "<";
            i++;
            if (arquivo[i - 1] == '\n') {
                linhaAtual++;
            }
        }
    } else if (arquivo[i] == '>') {
        if ((i + 1) < arquivo.size() && arquivo[i + 1] == '=') {
            token.simbolo = "smaiorig";
            token.lexema = ">=";
            i += 2;
            if (arquivo[i - 2] == '\n') {
                linhaAtual++;
            }
        } else {
            token.simbolo = "smaior";
            token.lexema = ">";
            i++;
            if (arquivo[i - 1] == '\n') {
                linhaAtual++;
            }
        }
    } else {
        token.lexema = "=";
        token.simbolo = "sig";
        if (arquivo[i] == '\n') {
            linhaAtual++; // NOVO: Incrementa se '=' for seguido por '\n'
        }
        i++;
    }
    tokens.push_back(token);
    this->setTokenAtual(token);
    return i;
}

int LexicalAnalyzer::pontuacao(int i) {
    Token token;
    std::string current(1, arquivo[i]);
    token.lexema = current;
    token.simbolo = symbolMap.at(current);
    if (arquivo[i] == '\n') {
        linhaAtual++; // NOVO: Incrementa se pontuação for '\n'
    }
    tokens.push_back(token);
    this->setTokenAtual(token);
    i++;
    return i;
}

int LexicalAnalyzer::pega_token(int index) {
    char carac = arquivo[index];
    if (std::isdigit(carac)) {
        return trata_digito(index);
    } else if (std::isalpha(carac)) {
        return trata_letra(index);
    } else if (carac == ':') {
        return atribuicao(index);
    } else if (carac == '+' || carac == '-' || carac == '*') {
        return operador_aritmetico(index);
    } else if (carac == '!' || carac == '<' || carac == '>' || carac == '=') {
        return operador_relacional(index);
    } else if (carac == ';' || carac == ',' || carac == '(' || carac == ')' ||
               carac == '.') {
        return pontuacao(index);
    } else {
        if (carac == '\n') {
            linhaAtual++; // NOVO: Incrementa se caractere inválido for '\n'
        }
        std::cout << "Caractere inválido na linha " << getLinhaAtual() << ": " << carac << std::endl;
        return index + 1;
    }
}

void LexicalAnalyzer::lexical() {
    bool comentario = false;
    while (i < arquivo.size()) {
        char current_char = arquivo[i];

        // Verificar início e fim de comentários
        if (current_char == '{') {
            comentario = true;
            i++;
            continue;
        } else if (current_char == '}') {
            comentario = false;
            i++;
            continue;
        }

        if (!comentario) {
            if (std::isspace(current_char)) {
                if (current_char == '\n') {
                    linhaAtual++; // NOVO: Incrementa ao encontrar uma nova linha
                }
                i++;
                continue;
            } else {
                // Processar o próximo token
                i = pega_token(i);
                return; // Retorna após processar um token
            }
        } else {
            if (current_char == '\n') {
                linhaAtual++; // NOVO: Incrementa ao encontrar uma nova linha dentro de um comentário
            }
            i++;
        }
    }

    // Se chegar ao final do arquivo, define token_atual como vazio
    this->token_atual = Token{"", ""};
}

void LexicalAnalyzer::visualizarTokens() const {
    const int larguraLexema = 20;
    const int larguraSimbolo = 20;

    std::cout << std::left << std::setw(larguraLexema) << "Lexema"
              << std::setw(larguraSimbolo) << "Símbolo" << std::endl;
    std::cout << std::setfill('-') << std::setw(larguraLexema + larguraSimbolo)
              << "" << std::setfill(' ') << std::endl;

    for (const auto &token : tokens) {
        std::cout << std::left << std::setw(larguraLexema) << token.lexema
                  << std::setw(larguraSimbolo) << token.simbolo << std::endl;
    }
}

Token LexicalAnalyzer::Lexic() {
    this->lexical(); // Método que avança para o próximo token
    return this->getTokenAtual(); // Retorna o token atual
}

Token LexicalAnalyzer::getTokenAtual() const {
    return this->token_atual;
}

void LexicalAnalyzer::setTokenAtual(const Token &token) {
    this->token_atual = token;
}

// Implementação do método getLinhaAtual
int LexicalAnalyzer::getLinhaAtual() const {
    return this->linhaAtual;
}
