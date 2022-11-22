#include "Colony.h"
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <string>
#include <string.h>
#include <stdio.h>
#include <cstdlib>
#include <ctime>
#include <bits/stdc++.h>
#include "Localidade.h"
#include "Instancia.h"

//CONSTRUTOR / DESTRUTOR
Colony::Colony(int nAnts, Instancia* instancia, int iteracoes, double initialPheromoneLevel){
	this->nAnts = nAnts;
    this->instancia = instancia;
    this->iteracoes = iteracoes;
    this->initialPheromoneLevel = initialPheromoneLevel;

    // INICIANDO VARIAVEIS PARA O CALCULO DE VALORES RANDOMICOS
    unsigned seed = time(0);   
    srand(seed);
}

Colony::~Colony() {}

//PRIMEIRO ACO
int Colony::howManyDepots_firstColony_DepotSelection(){
    //FUNÇÃO QUE CALCULA QUANTOS DEPÓSITOS SERÃO ABERTOS PARA CADA FORMIGA, NO PRIMEIRO ACO
    int n = this->instancia->getClientes();//numero de clientes da instancia, cada um com demanda qi = 1
    int w = this->instancia->getQDEPOT(); //capacidade dos depositos
    int u = rand() % 3 + 1; //randomico entre 1 e r, sendo que r = 3
    int m = this->instancia->getDepositos(); //número de depósitos
    int aux = (n / w) + u;

    if(aux > m)
        return m;
    return aux;
}

int Colony::roletaPrimeiroACO(vector<double> vetorDeHeuristicas, vector<int> depositosJaSelecionados){
    vector<float> probabilidades;
    vector<int> depositosRoleta;
    double somaDasHeuristicas = 0;

    for(int i = 0; i < vetorDeHeuristicas.size(); i++){
        if(std::find(depositosJaSelecionados.begin(), depositosJaSelecionados.end(), i) != depositosJaSelecionados.end()){
            //depósito já selecionado, desconsiderar
        } else{
            somaDasHeuristicas += vetorDeHeuristicas[i];
            probabilidades.push_back(vetorDeHeuristicas[i]);
            depositosRoleta.push_back(i);
        }
    }

    //VAMOS GIRAR A ROLETA
    float aleat = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/somaDasHeuristicas)); //recebe um aleatório de 0 até somaDasHeuristicas
    float sumAux = 0;
    for(int i = 0; i < probabilidades.size(); i++){
        sumAux += probabilidades[i];
        if(sumAux >= aleat){
            return depositosRoleta[i]; //RETORNA O INDICE DO DEPÓSITO SELECIONADO NO VETOR ORIGINAL (COM TODOS OS DEPÓSITOS)
        }
    }
}

void Colony::initializeFirstACO(){
    
    //VARIÁVEIS AUXILIARES
    int m = this->instancia->getDepositos(); //número de depósitos
    double w = double(this->instancia->getDEPOTFIXCOST()); //custo fixo para abrir um depósito
    double f = double(this->instancia->getQDEPOT()); //capacidade dos depósitos
    vector<int> selectedDepotsAuxVector;
    vector<int> selectedDepotsACO;
    vector<double> auxVectorFloat;
    vector<double> auxVector;
    
    //ADICIONANDO A PRIMEIRA LINHA NA MATRIZ DE FEROMONIOS, COM OS VALORES INICIAIS (IGUAIS PARA TODOS OS DEPÓSITOS)
    for(int i = 0; i < this->instancia->getDepositos(); i++){
        auxVector.push_back(this->initialPheromoneLevel);
    }
    this->pheromoneList.push_back(auxVector);

    //EXECUTANDO O PRIMEIRO ACO

    //ITERANDO CADA 'ITERAÇÃO'
    for(int s = 0; s < this->iteracoes; s++){
        //calculando a 'heuristica' de cada depósito para essa iteração
        auxVectorFloat.erase(auxVectorFloat.begin(), auxVectorFloat.end()); //limpando variavel auxiliar
        for(int j = 0; j < m; j++){
            auxVectorFloat.push_back(this->pheromoneList[s][j] * (w/f) * 1.0);
        }

        //ITERANDO CADA FORMIGA
        for(int h = 0; h < this->nAnts; h++){
            int p = this->howManyDepots_firstColony_DepotSelection(); //número de depósitos abertos para essa formiga
            //cout<<"\nFormiga: " << h << ", na iteracao: " << s << " escolheu " << p << " depositos." << endl;

            float q = static_cast <float> (rand()) / static_cast <float> (RAND_MAX); //aleatório entre 0 e 1 para definir se é guloso ou ACO
            if(q <= 0.5){ //q <= q0, onde q0 = 0.5 -> variável definida pelo autor (exploration factor)
    
                //GULOSO
                //cout<<"\nFormiga: " << h << ", na iteracao: " << s << " foi pelo GULOSO" << endl;
                //selecionando os 'p' melhores valores (depósitos) e atualizando a matriz de feromônio para esses depósitos
                auxVector = auxVectorFloat;
                for(int j = 0; j < p; j++){
                    auto it = std::minmax_element(auxVector.begin(), auxVector.end());//calculando o máximo elemento do vetor
                    int max_idx = std::distance(auxVector.begin(), it.second); //capturando o indice do maior elemento do vetor
                    selectedDepotsAuxVector.push_back(max_idx); //array que guarda os depósitos selecionados por essa formiga
                    auxVector[max_idx] = -999; //dessa forma, esse depósito não vai ser selecionado na próxima iteração de 'j'
                }
            } else{
                //ACO
                //cout<<"\nFormiga: " << h << ", na iteracao: " << s << " foi pelo ACO" << endl;
                int auxInt;
                for(int i = 0; i < p; i++){
                    auxInt = this->roletaPrimeiroACO(auxVectorFloat, selectedDepotsACO);
                    selectedDepotsACO.push_back(auxInt);
                    selectedDepotsAuxVector.push_back(auxInt);
                }
                selectedDepotsACO.erase(selectedDepotsACO.begin(), selectedDepotsACO.end()); //limpando variavel auxiliar
            }
        }

        // TESTANDO QUAIS DEPOSITOS ESSA ITERACAO ESCOLHEU
        // cout << endl << "depositos selecionados na iteracao " << s << ": " << endl;
        // for(int l = 0; l < selectedDepotsAuxVector.size(); l++){
        //     cout << " " << selectedDepotsAuxVector[l];
        // }

        //atualizando feromonios
        auxVector.erase(auxVector.begin(), auxVector.end()); //limpando variavel auxiliar
        auxVector = this->pheromoneList[s];//a nova linha na matriz de feromônios recebe a linha anterior
        auxVector = evaporacao(auxVector, 0.9);//descontar a evaporacao em cada depósito
        //SOMANDO A CONTRIBUIÇÃO DE CADA FORMIGA QUE UTILIZOU CADA DEPÓSITO
        for(int i = 0; i < selectedDepotsAuxVector.size(); i++){
            auxVector[selectedDepotsAuxVector[i]] += 0.1 * this->initialPheromoneLevel;
        }
        this->pheromoneList.push_back(auxVector);//adicionando a nova linha na matriz de feromônios
        selectedDepotsAuxVector.erase(selectedDepotsAuxVector.begin(), selectedDepotsAuxVector.end()); //limpando variavel auxiliar
    }
    
    //AGORA QUE O ACO TERMINOU, AS FORMIGAS JÁ FIZERAM SUAS CONTRIBUIÇÕES
    //ENTÃO, VOU APENAS CALCULAR QUANTOS E QUAIS DEPÓSITOS SERÃO ABERTOS, UMA ÚLTIMA VEZ
    //TENDO COMO BASE A MATRIZ DE FEROMÔNIOS ATUALIZADA POR TODAS AS FORMIGAS

    int finalDepots = this->howManyDepots_firstColony_DepotSelection(); //número de depósitos abertos para essa formiga
    cout << endl << "escolha final de depositos: " << finalDepots << endl;

    auxVectorFloat.erase(auxVectorFloat.begin(), auxVectorFloat.end()); //limpando variavel auxiliar
    selectedDepotsAuxVector.erase(selectedDepotsAuxVector.begin(), selectedDepotsAuxVector.end()); //limpando variavel auxiliar
    auxVectorFloat = this->pheromoneList[this->pheromoneList.size()-1];//pegando a última linha da matriz de feromônios
    for(int j = 0; j < finalDepots; j++){
        auto it = std::minmax_element(auxVectorFloat.begin(), auxVectorFloat.end());//calculando o máximo elemento do vetor
        int max_idx = std::distance(auxVectorFloat.begin(), it.second); //capturando o indice do maior elemento do vetor
        selectedDepotsAuxVector.push_back(max_idx); //array que guarda os depósitos selecionados por essa formiga
        auxVectorFloat[max_idx] = -999; //dessa forma, esse depósito não vai ser selecionado na próxima iteração de 'j'
    }

    //AGORA selectedDepotsAuxVector GUARDA OS INDICES DOS DEPOSITOS QUE SERAO UTILIZADOS
    this->openDepots = selectedDepotsAuxVector;

    cout << "\nPara essa instancia serao abertos os depositos: ";
    for(int i = 0; i < this->openDepots.size(); i++){
        cout << this->openDepots[i] << "  ";
    }

    cout << endl << "Imprimindo matriz final de feromonios:" << endl;
    for(int i = 0; i < this->iteracoes+1; i++){
        cout << "\n Iteracao " << i << ": ";
        for(int j = 0; j < m; j++){
            cout << "D" << j << ": " << this->pheromoneList[i][j] << " / ";
        }
    }
}

//SEGUNDO ACO
/*
void Colony::initializeSecondACO(vector<vector<int>> clusters, vector<vector<float> pairsDistances){
    
    //VARIÁVEIS AUXILIARES
    int m = this->instancia->getDepositos(); //número de depósitos
    int w = this->instancia->getDEPOTFIXCOST(); //custo fixo para abrir um depósito
    int f = this->instancia->getQDEPOT(); //capacidade dos depósitos
    vector<double> auxVector;
    vector<float> auxVectorFloat;
    //vector<int> selectedDepotsAuxVector;
    
    //A MATRIZ DE FEROMÔNIOS AGORA POSSUI UMA LINHA PARA CADA ITERAÇÃO, E UMA COLUNA PARA CADA PAR 'DEPÓSITO ABERTO/CLUSTER'
    //LIMPANDO A MATRIZ DE FEROMÔNIOS
    this->pheromoneList.erase(this->pheromoneList.begin(), this->pheromoneList.end());
    //ADICIONANDO A PRIMEIRA LINHA NA MATRIZ DE FEROMONIOS, COM OS VALORES INICIAIS (IGUAIS PARA TODOS OS PARES DEPOSITO/CLUSTER)
    for(int i = 0; i < this->openDepots.size() * clusters.size(); i++){
        auxVector.push_back(this->initialPheromoneLevel);
    }
    this->pheromoneList.push_back(auxVector);

    //vector<vector<int>> depotsClusteresAssociations;
    //em um primeiro momento imaginei essa função salvando em uma variavel da colônia a seguinte estrutura: um vector com d posições, onde d
    //é o número de depósitos a serem abertos naquele dia. Cada uma dessas d posições é um vector de inteiros, que guarda os index dos clusters que 
    //esse depósito atende. Esses index são relativos à variável global 'clusters'

    //ITERANDO CADA 'ITERAÇÃO'
    for(int s = 0; s < this->iteracoes; s++){

        //ITERANDO CADA FORMIGA
        for(int h = 0; h < this->nAnts; h++){

            float q = static_cast <float> (rand()) / static_cast <float> (RAND_MAX); //aleatório entre 0 e 1 para definir se é guloso ou ACO
            if(q <= 0.1){ //q' <= q'0, onde q'0 = 0.1 -> variável definida pelo autor (exploration factor)
                //GULOSO

                //calculando a 'heuristica' de cada par parkingSpot/depósito
                for(int j = 0; j < selected_depots.size() * clusters.size(); j++){
                    //NÃO ESTOU CONSEGUINDO PROGREDIR AQUI
                    //auxVectorFloat.push_back(pairsPheromoneLevels[i][j] * (1 / )); //B(BETA) = 1 -> influência do feromônio do segundo ACO
                }
            
            } else{
                //ACO      
            }
                    }

        //TESTANTO QUAIS DEPOSITOS ESSA ITERACAO ESCOLHEU
        // cout << endl << "depositos selecionados: "<< endl;
        // for(int l = 0; l < selectedDepotsAuxVector.size(); l++){
        //     cout<<" " << selectedDepotsAuxVector[l];
        // }

        //atualizando feromonios
        auxVector.erase(auxVector.begin(), auxVector.end()); //limpando variavel auxiliar
        auxVector = this->pheromoneList[s];//a nova linha na matriz de feromônios recebe a linha anterior
        for(int i = 0; i < selectedDepotsAuxVector.size(); i++){
            //para cada uso de um depósito, aquele depósito tem seu nivel de feromônio atualizado
            auxVector[selectedDepotsAuxVector[i]] = 0.9 * auxVector[selectedDepotsAuxVector[i]] + 2; //FUNÇÃO DE ATUALIZAÇÃO DO NIVEL DE FEROMONIO
        }
        this->pheromoneList.push_back(auxVector);//adicionando a nova linha na matriz de feromônios
        selectedDepotsAuxVector.erase(selectedDepotsAuxVector.begin(), selectedDepotsAuxVector.end()); //limpando variavel auxiliar
    }





    //iterando cada formiga
    for(int i = 1; i <= h; i++){

        //PREENCHENDO A LINHA DA MATRIZ PARA ESSA ITERAÇÃO
        pairsPheromoneLevels.push_back(pairsPheromoneLevels[i-1]);
        
        float q = static_cast <float> (rand()) / static_cast <float> (RAND_MAX); //aleatório entre 0 e 1 para definir se é guloso ou ACO
        if(q <= 0.1){ //q' <= q'0, onde q'0 = 0.1 -> variável definida pelo autor (exploration factor)
            //GULOSO

            //calculando a 'heuristica' de cada par parkingSpot/depósito
            for(int j = 0; j < selected_depots.size() * clusters.size(); j++){
                //NÃO ESTOU CONSEGUINDO PROGREDIR AQUI
                //auxVectorFloat.push_back(pairsPheromoneLevels[i][j] * (1 / )); //B(BETA) = 1 -> influência do feromônio do segundo ACO
            }
        
        } else{
            //ACO      
        }
    }
}
*/

//TERCEIRO ACO

//OUTROS MÉTODOS AUXILIARES
vector<double> Colony::evaporacao(vector<double> auxVector, double taxaDeEvaporacao){
    for(int i = 0; i < auxVector.size(); i++){
        auxVector[i] = taxaDeEvaporacao * auxVector[i];
    }
    return auxVector;
}

double Colony::somatorioVetorDouble(vector<double> vec){
    float soma = 0;
    for(int i = 0; i < vec.size(); i++){
        soma += vec[i];
    }
    return soma;
}