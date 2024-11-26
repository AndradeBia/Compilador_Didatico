#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

using namespace std;

struct instrucao {
    string rotulo;
    string instrucao;
    string arg1;
    string arg2;
};

vector<int> MemoriaDados(300, 0); // Inicializa com 300 elementos iguais a 0
vector<instrucao> instrucoes;

int i = 0; // instruções
int s = -1;  // dados

void output_state(int instruction_pointer, const vector<int>& MemoriaDados, const vector<instrucao>& instrucoes) {
    // Output the event type
    cout << "{ \"event\": \"STATE_UPDATE\", ";

    // Output the instruction pointer
    cout << "\"instruction_pointer\": " << instruction_pointer << ", ";

    // Output the memory data
    cout << "\"memoria_dados\": [";
    for (size_t idx = 0; idx < MemoriaDados.size(); ++idx) {
        if (idx > 0) cout << ", ";
        cout << MemoriaDados[idx];
    }
    cout << "], ";

    // Output the instruction list
    cout << "\"instrucoes\": [";
    for (size_t idx = 0; idx < instrucoes.size(); ++idx) {
        if (idx > 0) cout << ", ";
        cout << "{";
        cout << "\"rotulo\": \"" << instrucoes[idx].rotulo << "\", ";
        cout << "\"instrucao\": \"" << instrucoes[idx].instrucao << "\", ";
        cout << "\"arg1\": \"" << instrucoes[idx].arg1 << "\", ";
        cout << "\"arg2\": \"" << instrucoes[idx].arg2 << "\"";
        cout << "}";
    }
    cout << "]";

    cout << "}" << endl;
    cout.flush();
}

int main(int argc, char* argv[])
{
    bool step_by_step = false;

    if (argc > 1) {
        std::string mode = argv[1];
        if (mode == "step") {
            step_by_step = true;
        }
    }

    // Abre o arquivo programa.obj para leitura
    ifstream arquivo("programa.obj");

    // Verifica se o arquivo foi aberto corretamente
    if (!arquivo.is_open()) {
        cerr << "Erro ao abrir o arquivo programa.obj" << endl;
        return 1;
    }

    string linha;
    // Lê o arquivo linha por linha
    while (getline(arquivo, linha)) {
        // Verifica se a linha tem o tamanho mínimo esperado
        if (linha.length() < 20) { // 4 + 8 + 4 + 4 = 20 caracteres
            cerr << "Linha inválida: " << linha << endl;
            continue;
        }

        // Cria uma nova instrução
        instrucao inst;

        // Extrai os campos da linha com base nas posições fixas
        inst.rotulo = linha.substr(0, 4);
        inst.instrucao = linha.substr(4, 8);
        inst.arg1 = linha.substr(12, 4);
        inst.arg2 = linha.substr(16, 4);

        // Remove espaços extras nos campos
        inst.rotulo.erase(inst.rotulo.find_last_not_of(' ') + 1);
        inst.instrucao.erase(inst.instrucao.find_last_not_of(' ') + 1);
        inst.arg1.erase(inst.arg1.find_last_not_of(' ') + 1);
        inst.arg2.erase(inst.arg2.find_last_not_of(' ') + 1);

        // Adiciona a instrução ao vetor
        instrucoes.push_back(inst);
    }

    // Fecha o arquivo
    arquivo.close();

    // Mapeia rotulos para números de linha
    map<string, int> rotulo_para_linha;
    for (size_t idx = 0; idx < instrucoes.size(); ++idx) {
        const auto& inst = instrucoes[idx];
        if (!inst.rotulo.empty()) {
            rotulo_para_linha[inst.rotulo] = idx;
        }
    }

    // Substitui rotulos em arg1 pelo número da linha
    for (auto& inst : instrucoes) {
        auto it = rotulo_para_linha.find(inst.arg1);
        if (it != rotulo_para_linha.end()) {
            inst.arg1 = to_string(it->second);
        }
    }

    // Send initial state before execution
    output_state(i, MemoriaDados, instrucoes);

    while (1) {
        std::string instrucao = instrucoes.at(i).instrucao;

        if (instrucao == "LDC") {
            s++;
            MemoriaDados.at(s) = stoi(instrucoes.at(i).arg1);
            i++;
        } else if (instrucao == "LDV") {
            s++;
            MemoriaDados.at(s) = MemoriaDados.at(stoi(instrucoes.at(i).arg1));
            i++;
        } else if (instrucao == "ADD") {
            int aux =  MemoriaDados.at(s);
            s--;
            MemoriaDados.at(s) += aux;
            i++;
        } else if (instrucao == "STR") {
            int aux = stoi(instrucoes.at(i).arg1);
            MemoriaDados.at(aux) = MemoriaDados.at(s);
            s--;
            i++;
        } else if (instrucao == "SUB") {
            int aux =  MemoriaDados.at(s);
            s--;
            MemoriaDados.at(s) -= aux;
            i++;
        } else if (instrucao == "MULT") {
            int aux =  MemoriaDados.at(s);
            s--;
            MemoriaDados.at(s) = MemoriaDados.at(s) * aux;
            i++;
        } else if (instrucao == "DIVI") {
            int aux =  MemoriaDados.at(s);
            s--;
            MemoriaDados.at(s) = MemoriaDados.at(s) / aux;
            i++;
        } else if (instrucao == "INV") {
            MemoriaDados.at(s) = MemoriaDados.at(s) * -1;
            i++;
        } else if (instrucao == "AND") {
            int aux =  MemoriaDados.at(s);
            s--;
            MemoriaDados.at(s) = (aux && MemoriaDados.at(s)) ? 1 : 0;
            i++;
        } else if (instrucao == "OR") {
            int aux =  MemoriaDados.at(s);
            s--;
            MemoriaDados.at(s) = (aux || MemoriaDados.at(s)) ? 1 : 0;
            i++;
        } else if (instrucao == "NEG") {
            MemoriaDados.at(s) = 1 - MemoriaDados.at(s);
            i++;
        } else if (instrucao == "CME") {
            int aux =  MemoriaDados.at(s);
            s--;
            MemoriaDados.at(s) = (MemoriaDados.at(s) < aux) ? 1 : 0;
            i++;
        } else if (instrucao == "CMA") {
            int aux =  MemoriaDados.at(s);
            s--;
            MemoriaDados.at(s) = (MemoriaDados.at(s) > aux) ? 1 : 0;
            i++;
        } else if (instrucao == "CEQ") {
            int aux =  MemoriaDados.at(s);
            s--;
            MemoriaDados.at(s) = (MemoriaDados.at(s) == aux) ? 1 : 0;
            i++;
        } else if (instrucao == "CDIF") {
            int aux =  MemoriaDados.at(s);
            s--;
            MemoriaDados.at(s) = (MemoriaDados.at(s) != aux) ? 1 : 0;
            i++;
        } else if (instrucao == "CMEQ") {
            int aux =  MemoriaDados.at(s);
            s--;
            MemoriaDados.at(s) = (MemoriaDados.at(s) <= aux) ? 1 : 0;
            i++;
        } else if (instrucao == "CMAQ") {
            int aux =  MemoriaDados.at(s);
            s--;
            MemoriaDados.at(s) = (MemoriaDados.at(s) >= aux) ? 1 : 0;
            i++;
        } else if (instrucao == "ALLOC") {
            int m = stoi(instrucoes.at(i).arg1);
            int n = stoi(instrucoes.at(i).arg2);
            for (int k = 0; k <= n-1; ++k) {
                s++;
                MemoriaDados[s] = MemoriaDados[m + k];
            }
            i++;
        } else if (instrucao == "DALLOC") {
            int m = stoi(instrucoes.at(i).arg1);
            int n = stoi(instrucoes.at(i).arg2);
            for (int k = n - 1; k >= 0; --k) {
                MemoriaDados[m + k] = MemoriaDados[s];
                s--;
            }
            i++;
        } else if (instrucao == "START") {
            s = -1;
            i++;
        } else if (instrucao == "PRN") {
            // Output an "OUTPUT" event
            cout << "{ \"event\": \"OUTPUT\", \"value\": " << MemoriaDados.at(s) << " }" << endl;
            cout.flush();
            s--;
            i++;
        } else if (instrucao == "RD") {
            // Output an "INPUT_REQUEST" event
            cout << "{ \"event\": \"INPUT_REQUEST\" }" << endl;
            cout.flush();
            // Read input from stdin
            std::string input_line;
            std::getline(std::cin, input_line);
            int numero = std::stoi(input_line);
            s++;
            MemoriaDados.at(s) = numero;
            i++;
        } else if (instrucao == "JMP") {
            i = stoi(instrucoes.at(i).arg1);
        } else if (instrucao == "JMPF") {
            if (MemoriaDados.at(s) == 0) {
                i = stoi(instrucoes.at(i).arg1);
            } else {
                i++;
            }
            s--;
        } else if (instrucao == "NULL") {
            i++;
        } else if (instrucao == "CALL") {
            s++;
            MemoriaDados.at(s) = i + 1;
            i = stoi(instrucoes.at(i).arg1);
        } else if (instrucao == "HLT") {
            // Output execution finished event
            cout << "{ \"event\": \"EXECUTION_FINISHED\" }" << endl;
            cout.flush();
            break;
        } else if (instrucao == "RETURN") {
            i = MemoriaDados.at(s);
            s--;
        } else {
            cout << "Instrução desconhecida: " << instrucao << endl;
        }

        // Output the current state
        output_state(i, MemoriaDados, instrucoes);

        // If in step_by_step mode, wait for command
        if (step_by_step) {
            std::string command;
            std::getline(std::cin, command);
            if (command == "NEXT") {
                // Proceed to next instruction
            } else if (command == "QUIT") {
                break;
            } else {
                // Handle invalid commands if necessary
            }
        }
    }

    return 0;
}
