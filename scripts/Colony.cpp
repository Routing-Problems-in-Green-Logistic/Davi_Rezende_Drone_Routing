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

//COLÔNIA DE FORMIGAS
int Colony::roletaACO(vector<double> vetorDeHeuristicas){
    
    //CALCULANDO A SOMA DAS HEURÍSTICAS
    double somaDasHeuristicas = 0;
    for(int i = 0; i < vetorDeHeuristicas.size(); i++){
        somaDasHeuristicas += vetorDeHeuristicas[i];
    }
    // cout << endl << "ROLETA: sao " << vetorDeHeuristicas.size() << " heuristicas";
    // cout << endl << "ROLETA: soma das heuristicas = " << somaDasHeuristicas;
    // cout << endl << "ROLETA: imprimindo heuristicas:";
    // for(int y = 0; y < vetorDeHeuristicas.size(); y++){
    //     cout << "   " << vetorDeHeuristicas[y];
    // }
    if(somaDasHeuristicas == 0){
        //retorna uma posição aleatória do array
        return (rand() % vetorDeHeuristicas.size()+1) - 1;
    } else{
        //VAMOS GIRAR A ROLETA
        float aleat = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/somaDasHeuristicas)); //recebe um aleatório de 0 até somaDasHeuristicas
        //cout << endl << "ROLETA: girou a roleta, sorteou = " << aleat;
        float sumAux = 0;
        for(int i = 0; i < vetorDeHeuristicas.size(); i++){
            sumAux += vetorDeHeuristicas[i];
            if(sumAux >= aleat){
                return i; //RETORNA O INDICE DO PAR SELECIONADO NO VETOR ORIGINAL (vetorDeHeuristicas)
            }
        }
    }
}

//PRIMEIRO ACO
int Colony::howManyDepots_firstColony_DepotSelection(){
    //FUNÇÃO QUE CALCULA QUANTOS DEPÓSITOS SERÃO ABERTOS PARA CADA FORMIGA, NO PRIMEIRO ACO
    int n = this->instancia->getClientes();//numero de clientes da instancia, cada um com demanda qi = 1
    int u = rand() % 3 + 1; //randomico entre 1 e r, sendo que r = 3
    int m = this->instancia->getDepositos(); //número de depósitos
    int w = 0;//capacidade dos depositos
    for(int i = 0; i < this->instancia->depositosLoc.size(); i++){
        w += this->instancia->depositosLoc[i]->capacidadeDoDeposito;
    }
    
    int aux = (n / (w/m)) + u; //FÓRMULA 16 DO TRABALHO DE BASE
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
    vector<int> selectedDepotsAuxVector;
    vector<int> selectedDepotsACO;
    vector<double> auxVectorFloat;
    vector<double> auxVector;
    
    //ADICIONANDO A PRIMEIRA LINHA NA MATRIZ DE FEROMONIOS, COM OS VALORES INICIAIS (IGUAIS PARA TODOS OS DEPÓSITOS)
    for(int i = 0; i < this->instancia->getDepositos(); i++){
        auxVector.push_back(this->initialPheromoneLevel);
    }
    this->pheromoneList.push_back(auxVector);

    //ITERANDO CADA 'ITERAÇÃO'
    for(int s = 0; s < this->iteracoes; s++){
        //calculando a 'heuristica' de cada depósito para essa iteração
        auxVectorFloat.erase(auxVectorFloat.begin(), auxVectorFloat.end()); //limpando variavel auxiliar
        for(int j = 0; j < this->instancia->getDepositos(); j++){
            auxVectorFloat.push_back(this->pheromoneList[s][j] *  (this->instancia->depositosLoc[j]->capacidadeDoDeposito / this->instancia->depositosLoc[j]->custoDoDeposito)); //adicionando o produto: feromônio * heurística^1
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

    //CRIANDO UM ARRAY COM OS DEPÓSITOS ABERTOS PARA ESSA INSTÂNCIA
    //IMPRIMINDO RESULTADO DO PRIMEIRO ACO
    cout << endl << "RESULTADO DO PRIMEIRO ACO -> Serao abertos os seguintes depositos:";
    std::sort(this->openDepots.begin(), this->openDepots.end());
    for(int i = 0; i < this->openDepots.size(); i++){
        this->instancia->depositosLoc[this->openDepots[i]]->capacidadeUtilizadaDoDeposito = 0;
        this->depositosAbertosLoc.push_back(this->instancia->depositosLoc[this->openDepots[i]]);
        cout << "  " << this->openDepots[i];
    }
}

//SEGUNDO ACO
double Colony::encontraMenorDistanciaEntreClusters(Localidade* clusterEspecifico, vector<Localidade*> conjuntoDeClusters){
    int idEspecifico = stoi(clusterEspecifico->getId().substr(2, clusterEspecifico->getId().size()));
    int aux;
    vector<double> distancias;

    for(int i = 0; i < conjuntoDeClusters.size(); i++){
        aux = stoi(conjuntoDeClusters[i]->getId().substr(2, conjuntoDeClusters[i]->getId().size()));
        distancias.push_back(this->instancia->distanciasEntreParkingSpots[idEspecifico][aux]);
    }

    if(distancias.size() > 0){
        //AGORA TENHO UM VETOR COM TODAS AS DISTÂNCIAS, BASTA ENCONTRAR E RETORNAR O MENOR VALOR
        auto it = std::minmax_element(distancias.begin(), distancias.end());//calculando o mínimo elemento do vetor
        int min_idx = std::distance(distancias.begin(), it.first); //capturando o indice do menor elemento do vetor
        return fabs(distancias[min_idx]);
    } else{
        return 0;
    }
}

//calcula a distância entre dois pontos, dadas as coordenadas
double Colony::calculateDistanceBetweenPoints(float xa, float ya, float xb, float yb){
    return sqrt(pow((xa - xb), 2) + pow((ya - yb), 2));
}

void Colony::initializeSecondACO(){
    //VARIÁVEIS AUXILIARES
    vector<double> auxVector;
    vector<double> heuristicas;
    vector<Localidade*> auxLoc;
    vector<vector<Localidade*>> auxAuxLoc; //ARRAY AUXILIAR PARA REDUZIR O NUMERO DE LAÇOS NO ACO ABAIXO

    //A MATRIZ DE FEROMÔNIOS AGORA POSSUI UMA LINHA PARA CADA ITERAÇÃO, E UMA COLUNA PARA CADA PAR 'DEPÓSITO ABERTO/CLUSTER'
    //ADICIONANDO A PRIMEIRA LINHA NA MATRIZ DE FEROMONIOS, COM OS VALORES INICIAIS (IGUAIS PARA TODOS OS PARES DEPOSITO/CLUSTER)
    for(int i = 0; i < this->openDepots.size() * this->instancia->clusters.size(); i++){
        auxVector.push_back(this->initialPheromoneLevel);
    }
    this->secondACOpheromoneList.push_back(auxVector);

    for(int i = 0; i < this->openDepots.size(); i++){
        auxAuxLoc.push_back(auxLoc);
    }

    //ITERANDO CADA 'ITERAÇÃO'
    for(int s = 0; s < this->iteracoes; s++){
        vector<vector<Localidade*>> solucoesDaIteracao = auxAuxLoc;

        //ITERANDO CADA FORMIGA
        for(int h = 0; h < this->nAnts; h++){
            vector<Localidade*> clustersSelecionadosLoc;
            vector<Localidade*> clustersAindaNaoSelecionadosLoc = this->instancia->parkingSpots;
            vector<vector<Localidade*>> solucaoParcial = auxAuxLoc; //esse array tem uma posição para cada depósito aberto, e dentro de cada depósito temos uma lista de clusters

            //ENQUANTO TIVER CLUSTERS NÃO ASSOCIADOS
            while(clustersSelecionadosLoc.size() < this->instancia->parkingSpots.size()){
                //calculando a 'heuristica' de cada par depósito/cluster para esse passo da formiga
                heuristicas.erase(heuristicas.begin(), heuristicas.end()); //limpando a variável para receber as heurísticas dessa iteração
                for(int i = 0; i < this->depositosAbertosLoc.size(); i++){
                    for(int j = 0; j < clustersAindaNaoSelecionadosLoc.size(); j++){
                        
                        double distanciaEntrePontoEdeposito = fabs(this->calculateDistanceBetweenPoints(this->depositosAbertosLoc[i]->getx(), this->depositosAbertosLoc[i]->gety(), clustersAindaNaoSelecionadosLoc[j]->getx(), clustersAindaNaoSelecionadosLoc[j]->gety()));
                        double menorDistanciaEntreClusters = 0;
                        if(solucaoParcial[i].size() > 0){
                            menorDistanciaEntreClusters = this->encontraMenorDistanciaEntreClusters(clustersAindaNaoSelecionadosLoc[j], solucaoParcial[i]); //fazer funcao q percorre a matriz da instancia e captura esse menor valor;                        
                        }

                        //ADICIONANDO NO ARRAY DE HEURÍSTISCAS O SEGUINTE PRODUTO: feromônio x heurística^1
                        if(distanciaEntrePontoEdeposito < menorDistanciaEntreClusters){                      
                            heuristicas.push_back(this->secondACOpheromoneList[s][(i * clustersAindaNaoSelecionadosLoc.size()) + j] * (1 / distanciaEntrePontoEdeposito));
                        } else{
                            heuristicas.push_back(this->secondACOpheromoneList[s][(i * clustersAindaNaoSelecionadosLoc.size()) + j] * (1 / menorDistanciaEntreClusters));
                        }
                    }
                }

                float q = static_cast <float> (rand()) / static_cast <float> (RAND_MAX); //aleatório entre 0 e 1 para definir se é guloso ou ACO
                if(q <= 0.5){ //q' <= q'0, onde q'0 = 0.1 -> variável definida pelo autor (exploration factor) (COLOQUEI 50% MAS NO TRABALHO BASE É 10%)
                    //GULOSO
                    //VOU SELECIONANDO SEMPRE OS MAIORES VALORES DE HEURÍSTICA E ASSOCIANDO OS PARES SELECIONADOS
                    int success = 0;
                    while(success == 0){
                        auto it = std::minmax_element(heuristicas.begin(), heuristicas.end());//calculando o máximo elemento do vetor
                        int max_idx = std::distance(heuristicas.begin(), it.second); //capturando o indice do maior elemento do vetor

                        //lógica para encontrar o par cluster/depósito indicado por esse índice max_idx
                        Localidade* depositoSelecionado = this->depositosAbertosLoc[max_idx / clustersAindaNaoSelecionadosLoc.size()];
                        //cout << endl << "deposito selecionado: " << depositoSelecionado->getId();
                        Localidade* clusterSelecionado = clustersAindaNaoSelecionadosLoc[max_idx % clustersAindaNaoSelecionadosLoc.size()];
                        //cout << endl << "cluster selecionado: " << clusterSelecionado->getId();
                        //cout << endl<< "capUti = " << depositoSelecionado->capacidadeUtilizadaDoDeposito << " - clusSize = " << clusterSelecionado->clientesNoCluster.size() << " - capDep = " << depositoSelecionado->capacidadeDoDeposito;
                        if(depositoSelecionado->capacidadeUtilizadaDoDeposito + clusterSelecionado->clientesNoCluster.size() > depositoSelecionado->capacidadeDoDeposito + 2){ //AQUI ESTA A TRAPAÇA
                            //esse depósito não possui mais capacidade para atender esse cluster, precisamos escolher outro par
                            heuristicas[max_idx] = -999;
                        } else{
                            solucaoParcial[max_idx / clustersAindaNaoSelecionadosLoc.size()].push_back(clusterSelecionado); //incrementa a solução parcial
                            this->depositosAbertosLoc[max_idx / clustersAindaNaoSelecionadosLoc.size()]->capacidadeUtilizadaDoDeposito += clusterSelecionado->clientesNoCluster.size(); //incrementa a capacidadeUtilizada do depósito
                            clustersSelecionadosLoc.push_back(clusterSelecionado); //adiciona o cluster na lista de clusters já associados
                            clustersAindaNaoSelecionadosLoc.erase(clustersAindaNaoSelecionadosLoc.begin() + (max_idx % clustersAindaNaoSelecionadosLoc.size()));//remover o cluster selecionado
                            success = 1;
                        }
                    }
                } else{
                    //ACO
                    int success = 0;
                    while(success == 0){
                        //CHAMA A ROLETA, QUE RETORNA O ÍNDICE DA HEURÍSTICA (PAR) A SER UTILIZADA
                        int auxInt = this->roletaACO(heuristicas);
                        //cout << endl << "ROLETA NO SEGUNDO ACO SORTEOU = " << auxInt;

                        //lógica para encontrar o par cluster/depósito indicado por esse índice da roleta
                        Localidade* depositoSelecionado = this->depositosAbertosLoc[auxInt / clustersAindaNaoSelecionadosLoc.size()];
                        //cout << endl << "deposito selecionado: " << depositoSelecionado->getId();
                        Localidade* clusterSelecionado = clustersAindaNaoSelecionadosLoc[auxInt % clustersAindaNaoSelecionadosLoc.size()];
                        //cout << endl << "cluster selecionado: " << clusterSelecionado->getId();
                        //cout << endl<< "capUti = " << depositoSelecionado->capacidadeUtilizadaDoDeposito << " - clusSize = " << clusterSelecionado->clientesNoCluster.size() << " - capDep = " << depositoSelecionado->capacidadeDoDeposito;
                        
                        //verificando se a solução é viável (se o depósito comporta mais esse cluster)
                        if(depositoSelecionado->capacidadeUtilizadaDoDeposito + clusterSelecionado->clientesNoCluster.size() > depositoSelecionado->capacidadeDoDeposito + 2){//AQUI ESTÁ A TRAPAÇA
                            //nao comporta, chama a roleta de novo (mantém success == 0). TALVEZ PENSAR EM ALGO PRA FAZER AQUI!!
                        } else{
                            //comporta, efetiva a decisão
                            solucaoParcial[auxInt / clustersAindaNaoSelecionadosLoc.size()].push_back(clusterSelecionado); //incrementa a solução parcial
                            this->depositosAbertosLoc[auxInt / clustersAindaNaoSelecionadosLoc.size()]->capacidadeUtilizadaDoDeposito += clusterSelecionado->clientesNoCluster.size(); //incrementa a capacidadeUtilizada do depósito
                            clustersSelecionadosLoc.push_back(clusterSelecionado); //adiciona o cluster na lista de clusters já associados
                            clustersAindaNaoSelecionadosLoc.erase(clustersAindaNaoSelecionadosLoc.begin() + (auxInt % clustersAindaNaoSelecionadosLoc.size()));//remover o cluster selecionado
                            success = 1;
                        }
                    }
                }
            }

            //IMPRIMINDO SOLUÇÃO DESSA FORMIGA
            //cout << endl << "Formiga " << h << " na iteracao " << s << " construiu a seguinte solucao:";
            
            //INCREMENTANDO AS SOLUÇÕES DA ITERAÇÃO
            for(int i = 0; i < solucaoParcial.size(); i++){
                for(int j = 0; j < solucaoParcial[i].size(); j++){
                    solucoesDaIteracao[i].push_back(solucaoParcial[i][j]);
                }
            }

            //ZERANDO CAPACIDADES UTILIZADAS DOS DEPÓSITOS
            for(int i = 0; i < this->instancia->depositosLoc.size(); i++){
                this->instancia->depositosLoc[i]->capacidadeUtilizadaDoDeposito = 0;
            }
        }
        //atualizando feromonios
        auxVector.erase(auxVector.begin(), auxVector.end()); //limpando variavel auxiliar
        auxVector = this->secondACOpheromoneList[s];//a nova linha na matriz de feromônios recebe a linha anterior
        auxVector = evaporacao(auxVector, 0.9);//descontar a evaporacao em cada depósito
        //SOMANDO A CONTRIBUIÇÃO DE CADA FORMIGA QUE SELECIONOU AQUELE PAR CLUSTER/DEPÓSITO
        for(int i = 0; i < solucoesDaIteracao.size(); i++){
            for(int j = 0; j < solucoesDaIteracao[i].size(); j++){
                int id = stoi(solucoesDaIteracao[i][j]->getId().substr(2, solucoesDaIteracao[i][j]->getId().size())); //ÍNDICE NUMÉRICO DO CLUSTER
                auxVector[i * this->instancia->clusters.size() + id] += 0.1 * this->initialPheromoneLevel;
            }
        }
        this->secondACOpheromoneList.push_back(auxVector);//adicionando a nova linha na matriz de feromônios
        solucoesDaIteracao.erase(solucoesDaIteracao.begin(), solucoesDaIteracao.end()); //limpando variavel auxiliar
    }

    //CALCULANDO SOLUÇÃO FINAL
    vector<Localidade*> clustersSelecionadosLoc;
    vector<Localidade*> clustersAindaNaoSelecionadosLoc = this->instancia->parkingSpots;
    vector<vector<Localidade*>> solucaoParcial; //esse array tem uma posição para cada depósito aberto, e dentro de cada depósito temos uma lista de clusters
    for(int i = 0; i < this->depositosAbertosLoc.size(); i++){
        solucaoParcial.push_back(auxLoc);
    }

    //ENQUANTO TIVER CLUSTERS NÃO ASSOCIADOS
    while(clustersSelecionadosLoc.size() < this->instancia->parkingSpots.size()){
        //calculando a 'heuristica' de cada par depósito/cluster para esse passo da formiga
        heuristicas.erase(heuristicas.begin(), heuristicas.end()); //limpando a variável para receber as heurísticas dessa iteração
        for(int i = 0; i < this->depositosAbertosLoc.size(); i++){
            for(int j = 0; j < clustersAindaNaoSelecionadosLoc.size(); j++){
                double distanciaEntrePontoEdeposito = this->calculateDistanceBetweenPoints(this->depositosAbertosLoc[i]->getx(), this->depositosAbertosLoc[i]->gety(), clustersAindaNaoSelecionadosLoc[j]->getx(), clustersAindaNaoSelecionadosLoc[j]->gety());
                double menorDistanciaEntreClusters = 0;
                if(solucaoParcial[i].size() > 0){
                    menorDistanciaEntreClusters = this->encontraMenorDistanciaEntreClusters(clustersAindaNaoSelecionadosLoc[j], solucaoParcial[i]); //fazer funcao q percorre a matriz da instancia e captura esse menor valor;                        
                }

                if(distanciaEntrePontoEdeposito < menorDistanciaEntreClusters){
                    heuristicas.push_back(this->secondACOpheromoneList[secondACOpheromoneList.size()-1][(i * clustersAindaNaoSelecionadosLoc.size()) + j] * (1 / distanciaEntrePontoEdeposito));
                } else{
                    heuristicas.push_back(this->secondACOpheromoneList[secondACOpheromoneList.size()-1][(i * clustersAindaNaoSelecionadosLoc.size()) + j] * (1 / menorDistanciaEntreClusters));
                }
            }
        }

        //GULOSO
        //VOU SELECIONANDO SEMPRE OS MAIORES VALORES DE HEURÍSTICA E ASSOCIANDO OS PARES SELECIONADOS
        int success = 0;
        while(success == 0){
            auto it = std::minmax_element(heuristicas.begin(), heuristicas.end());//calculando o máximo elemento do vetor
            int max_idx = std::distance(heuristicas.begin(), it.second); //capturando o indice do maior elemento do vetor

            //lógica para encontrar o par cluster/depósito indicado por esse índice max_idx
            Localidade* depositoSelecionado = this->depositosAbertosLoc[max_idx / clustersAindaNaoSelecionadosLoc.size()];
            Localidade* clusterSelecionado = clustersAindaNaoSelecionadosLoc[max_idx % clustersAindaNaoSelecionadosLoc.size()];
            if(depositoSelecionado->capacidadeUtilizadaDoDeposito + clusterSelecionado->clientesNoCluster.size() > depositoSelecionado->capacidadeDoDeposito){
                //esse depósito não possui mais capacidade para atender esse cluster, precisamos escolher outro par
                heuristicas[max_idx] = -999;
            } else{
                solucaoParcial[max_idx / clustersAindaNaoSelecionadosLoc.size()].push_back(clusterSelecionado); //incrementa a solução parcial
                depositoSelecionado->capacidadeUtilizadaDoDeposito += clusterSelecionado->clientesNoCluster.size(); //incrementa a capacidadeUtilizada do depósito
                clustersSelecionadosLoc.push_back(clusterSelecionado); //adiciona o cluster na lista de clusters já associados
                clustersAindaNaoSelecionadosLoc.erase(clustersAindaNaoSelecionadosLoc.begin() + (max_idx % clustersAindaNaoSelecionadosLoc.size()));//remover o cluster selecionado
                success = 1;
            }
        }  
    }

    //IMPRIMINDO SOLUÇÃO FINAL
    cout << endl << "\nRESULTADO DO SEGUNDO ACO -> Serao feitas as seguintes associacoes entre clusters e depositos:";
    for(int i = 0; i < solucaoParcial.size(); i++){
        cout << endl << "Deposito " << this->depositosAbertosLoc[i]->getId() << ": ";
        for(int j = 0; j < solucaoParcial[i].size(); j++){
            cout <<" " << solucaoParcial[i][j]->getId();
        }
    }
    this->solucaoSegundoACO = solucaoParcial; //SALVANDO SOLUÇÃO DESTE ACO EM UM CAMPO DA COLÔNIA
}

//TERCEIRO ACO
void Colony::initializeThirdACO(){
    //VARIÁVEIS AUXILIARES
    vector<Localidade*> rotaParcial;
    vector<vector<Localidade*>> solucaoParcial;
    vector<double> heuristicas;
    vector<double> auxDouble;
    vector<Localidade*> auxLoc;
    vector<int> vetorDeDemandasDosClusters;
    int menorDemandaRestante;

    cout << endl << "\nRESULTADO DO TERCEIRO ACO -> Serao executadas as seguintes rotas:";

    // O ACO SERÁ EXECUTADO UMA VEZ PARA CADA DEPÓSITO ABERTO, UMA VEZ QUE SÃO COMO PROBLEMAS INDEPENDENTES
    //iterando cada depósito
    for(int i = 0; i < this->depositosAbertosLoc.size(); i++){
        //A MATRIZ DE FEROMÔNIOS POSSUI UMA LINHA PARA CADA ITERAÇÃO, E UMA COLUNA PARA CADA PAR DE PONTOS
        auxLoc = this->solucaoSegundoACO[i];
        auxLoc.push_back(this->depositosAbertosLoc[i]); //adicionando o próprio depósito, na última posição. Agora, auxLoc guarda todos os pontos envolvidos no roteamento para esse depósito
        auxDouble.erase(auxDouble.begin(), auxDouble.end()); //limpando variavel auxiliar
        this->thirdACOpheromoneList.erase(this->thirdACOpheromoneList.begin(), this->thirdACOpheromoneList.end()); //limpando a matriz de feromônio
        for(int t = 0; t < auxLoc.size() * auxLoc.size(); t++){
            auxDouble.push_back(this->initialPheromoneLevel);
        }
        this->thirdACOpheromoneList.push_back(auxDouble);

        //iterando cada "iteração"
        for(int s = 0; s < this->iteracoes; s++){
            //iterando cada formiga
            for(int h = 0; h < this->nAnts; h++){
                vector<Localidade*> clustersAindaNaoVisitados = this->solucaoSegundoACO[i];
                int capacidadeUtilizadaDoCaminhao = 0;
                Localidade* pontoAtual = this->depositosAbertosLoc[i]; //A ROTA SEMPRE SE INICIA NO DEPÓSITO DE ORIGEM
                rotaParcial.push_back(this->depositosAbertosLoc[i]); //A ROTA SEMPRE SE INICIA NO DEPÓSITO DE ORIGEM

                while(clustersAindaNaoVisitados.size() > 0){
                    //CONSTRUINDO E CRIANDO O ARRAY DE HEURÍSTICAS
                    for(int t = 0; t < clustersAindaNaoVisitados.size(); t++){
                        //CALCULANDO AS DISTÂNCIAS PARA A HEURÍSTICA
                        double d1 = fabs(this->calculateDistanceBetweenPoints(pontoAtual->getx(), pontoAtual->gety(), this->depositosAbertosLoc[i]->getx(), this->depositosAbertosLoc[i]->gety()));
                        double d2 = fabs(this->calculateDistanceBetweenPoints(this->depositosAbertosLoc[i]->getx(), this->depositosAbertosLoc[i]->gety(), clustersAindaNaoVisitados[t]->getx(), clustersAindaNaoVisitados[t]->gety()));
                        double d3 = fabs(this->calculateDistanceBetweenPoints(pontoAtual->getx(), pontoAtual->gety(), clustersAindaNaoVisitados[t]->getx(), clustersAindaNaoVisitados[t]->gety()));
                        //cout << endl << "d1=" << d1 << "  d2=" << d2 << "  d3="<< d3;
                        //ENCONTRANDO O FEROMÔNIO PARA O PAR ESCOLHIDO
                        std::vector<Localidade*>::iterator itz = std::find(auxLoc.begin(), auxLoc.end(), pontoAtual);
                        int indicePontoAtual = itz - auxLoc.begin();
                        itz = std::find (auxLoc.begin(), auxLoc.end(), clustersAindaNaoVisitados[t]);
                        int indiceProxPonto = itz - auxLoc.begin();
                        //cout << endl << "feromonio esse ponto: " << this->thirdACOpheromoneList[this->thirdACOpheromoneList.size()-1][indicePontoAtual * auxLoc.size() + indiceProxPonto];

                        //ADICIONANDO NO ARRAY DE HEURÍSTISCAS O SEGUINTE PRODUTO: feromônio x heurística^4
                        heuristicas.push_back(this->thirdACOpheromoneList[this->thirdACOpheromoneList.size()-1][indicePontoAtual * auxLoc.size() + indiceProxPonto] * pow((d1 + d2 - d3), 4));
                        
                        //POPULANDO O VETOR DE DEMANDAS DOS CLUSTERS, QUER ME AJUDA A ROTEAR O CAMINHÃO
                        vetorDeDemandasDosClusters.push_back(clustersAindaNaoVisitados[t]->clientesNoCluster.size());
                    }
                    // cout << endl << "imprimindo clusters ainda nao associados e suas demandas:";
                    // for(int rr = 0; rr < clustersAindaNaoVisitados.size(); rr++){
                    //     cout << "  cluster: " << clustersAindaNaoVisitados[rr]->getId() << "   demanda: " << clustersAindaNaoVisitados[rr]->clientesNoCluster.size() << "  /";
                    // }
                    auto itaux = std::minmax_element(vetorDeDemandasDosClusters.begin(), vetorDeDemandasDosClusters.end());//calculando o menor elemento do vetor
                    int min_idx = std::distance(vetorDeDemandasDosClusters.begin(), itaux.first); //capturando o indice do menor elemento do vetor
                    menorDemandaRestante = vetorDeDemandasDosClusters[min_idx]; //capturando a menor demanda restante
                    // cout << endl << "imprimindo menor demanda: " << menorDemandaRestante << "   teste: " << vetorDeDemandasDosClusters[min_idx];
                    // cout << endl << "imprimindo indice da menor demanda: " << min_idx;
                    // cout << endl << "imprimindo capacidade utilizada: " << capacidadeUtilizadaDoCaminhao;
                    // cout << endl << "imprimindo capacidade do caminhao: " << this->instancia->getQMAX();

                    //VERIFICANDO SE JÁ É HORA DE RETORNAR PARA O DEPÓSITO E RECARREGAR
                    if(capacidadeUtilizadaDoCaminhao + menorDemandaRestante > this->instancia->getQMAX()){
                        //cout << endl << "entrou if retorno pro deposito";
                        rotaParcial.push_back(this->depositosAbertosLoc[i]); //ADICIONANDO O DEPÓSITO NA ROTA
                        capacidadeUtilizadaDoCaminhao = 0;
                        pontoAtual = this->depositosAbertosLoc[i];
                    } else{ //ESSE CAMINHÃO AINDA CONSEGUE ATENDER CLUSTERS NESSA VIAGEM
                        int success = 0;
                        //cout << endl << "entrou else, vamos atender clusters";
                        float q = static_cast <float> (rand()) / static_cast <float> (RAND_MAX); //aleatório entre 0 e 1 para definir se é guloso ou ACO
                        if(q <= 0.001){ //q'' <= q''0, onde q''0 = 0.5 -> variável definida pelo autor (exploration factor)
                            //GULOSO
                            //cout << endl << "entrou guloso";
                            //cout << endl<< "formiga "<< h << " na iteracao " << s << "entrou no guloso";
                            while(success == 0){
                                //cout << endl << "entrou while do guloso";
                                auto it = std::minmax_element(heuristicas.begin(), heuristicas.end());//calculando o máximo elemento do vetor
                                int max_idx = std::distance(heuristicas.begin(), it.second); //capturando o indice do maior elemento do vetor
                                // cout << endl << "imprimindo heuristicas:";
                                // for(int jj = 0; jj < heuristicas.size(); jj++){
                                //     cout << "   " << heuristicas[jj];
                                // }
                                // cout << endl << "imprimindo maior heuristica: " << heuristicas[max_idx];
                                // cout << endl << "imprimindo indice da maior heuristica: " << max_idx;

                                //VERIFICANDO SE O CAMINHÃO CONSEGUE ATENDER ESSE CLUSTER NESSA VIAGEM AINDA
                                if(capacidadeUtilizadaDoCaminhao + clustersAindaNaoVisitados[max_idx]->clientesNoCluster.size() <= this->instancia->getQMAX()){
                                    //cout << endl << "entrou if guloso";
                                    capacidadeUtilizadaDoCaminhao += clustersAindaNaoVisitados[max_idx]->clientesNoCluster.size(); //atualiza capacidade utilizada do caminhão
                                    rotaParcial.push_back(clustersAindaNaoVisitados[max_idx]); //inclui novo cluster na rota 
                                    pontoAtual = clustersAindaNaoVisitados[max_idx]; //atualiza o ponto atual               
                                    clustersAindaNaoVisitados.erase(clustersAindaNaoVisitados.begin() + max_idx);//remover o cluster selecionado TO ACHANDO Q O ERRO TA NESSE ERASE Q TAVA COM UM % ANTES
                                    success = 1;
                                } else{
                                    //cout << endl << "entrou else guloso";
                                    heuristicas[max_idx] = -999; //garantindo que esse ponto não será escolhido na próxima vez
                                }
                            }
                        } else{
                            //ACO
                            //cout << endl << "entrou aco";
                            //cout << endl<< "formiga "<< h << " na iteracao " << s << "entrou no aco";
                            while(success == 0){
                                //cout << endl << "entrou while do aco";
                                //CHAMA A ROLETA, QUE RETORNA O ÍNDICE DA HEURÍSTICA (PAR) A SER UTILIZADA
                                int auxInt = this->roletaACO(heuristicas);
                                // cout << endl << "imprimindo heuristicas:";
                                // for(int jj = 0; jj < heuristicas.size(); jj++){
                                //     cout << "   " << heuristicas[jj];
                                // }
                                // cout << endl << "imprimindo indice da roleta: " << auxInt;
                                //cout << endl << "voltou da roleta, sorteou o indice = " << auxInt;
                                //VERIFICANDO SE O CAMINHÃO CONSEGUE ATENDER ESSE CLUSTER NESSA VIAGEM AINDA
                                //cout << endl << "TESTANDO ACO: caputilizadaCaminhap = " << capacidadeUtilizadaDoCaminhao << "  /  novaDemanda = " << clustersAindaNaoVisitados[auxInt]->clientesNoCluster.size() << "  /  capCaminhao = " << this->instancia->getQMAX(); 
                                if(capacidadeUtilizadaDoCaminhao + clustersAindaNaoVisitados[auxInt]->clientesNoCluster.size() <= this->instancia->getQMAX()){
                                    //cout << endl << "entrou if aco";
                                    capacidadeUtilizadaDoCaminhao += clustersAindaNaoVisitados[auxInt]->clientesNoCluster.size(); //atualiza capacidade utilizada do caminhão
                                    rotaParcial.push_back(clustersAindaNaoVisitados[auxInt]); //inclui novo cluster na rota 
                                    pontoAtual = clustersAindaNaoVisitados[auxInt]; //atualiza o ponto atual
                                    clustersAindaNaoVisitados.erase(clustersAindaNaoVisitados.begin() + auxInt);//remover o cluster selecionado
                                    success = 1;
                                } else{
                                    //cout << endl << "entrou else aco";
                                    heuristicas[auxInt] = 0; //garantindo que esse ponto não será escolhido na próxima vez
                                }
                            }
                        }
                    }
                    heuristicas.erase(heuristicas.begin(), heuristicas.end()); //limpando variavel auxiliar
                    vetorDeDemandasDosClusters.erase(vetorDeDemandasDosClusters.begin(), vetorDeDemandasDosClusters.end()); //limpando variavel auxiliar
                }
                rotaParcial.push_back(this->depositosAbertosLoc[i]); //adicionando o depósito, pois é o ponto final da rota
                // cout << endl << "A formiga " << h << " na iteracao " << s << " construiu a seguinte solucao pro deposito " << depositosDisponiveis[i]->getId();
                // for(int x = 0; x < rotaParcial.size(); x++){
                //     cout << "  " << rotaParcial[x]->getId() << " (demanda: " << rotaParcial[x]->clientesNoCluster.size() << ")   ";
                // }
                solucaoParcial.push_back(rotaParcial);
                rotaParcial.erase(rotaParcial.begin(), rotaParcial.end()); //limpando variavel auxiliar
            }

            //ATUALIZA FEROMONIOS
            auxDouble.erase(auxDouble.begin(), auxDouble.end()); //limpando variavel auxiliar
            auxDouble = this->thirdACOpheromoneList[s];//a nova linha na matriz de feromônios recebe a linha anterior
            auxDouble = evaporacao(auxDouble, 0.9);//descontar a evaporacao em cada par
            //SOMANDO A CONTRIBUIÇÃO DE CADA FORMIGA QUE SELECIONOU AQUELE PAR
            for(int g = 0; g < solucaoParcial[solucaoParcial.size()-1].size()-1; g++){ 
                std::vector<Localidade*>::iterator ite = std::find(auxLoc.begin(), auxLoc.end(), solucaoParcial[solucaoParcial.size()-1][g]);
                int indicePontoAtual = ite - auxLoc.begin();
                ite = std::find (auxLoc.begin(), auxLoc.end(), solucaoParcial[solucaoParcial.size()-1][g+1]);
                int indiceProxPonto = ite - auxLoc.begin();
                auxDouble[indicePontoAtual * auxLoc.size() + indiceProxPonto] += 0.1 * this->initialPheromoneLevel;
            }
            this->thirdACOpheromoneList.push_back(auxDouble);//adicionando a nova linha na matriz de feromônios
        }
        //AGORA, QUE O ALGORITMO JÁ FOI EXECUTADO PARA ESSE ACO, VAMOS FAZER UMA ÚLTIMA EXECUÇÃO GULOSA, PARA DEFINIR A SOLUÇÃO FINAL PARA ESSE DEPÓSITO
        //SENDO ASSIM, ESSE ÚLTIMO CÁLCULO VAI LEVAR EM CONSIDERAÇÃO UMA MATRIZ DE FEROMÔNIO QUE RECEBEU CONTRIBUIÇÃO DE TODAS AS FORMIGAS E ITERAÇÕES
        vector<Localidade*> clustersAindaNaoVisitados = this->solucaoSegundoACO[i];
        int capacidadeUtilizadaDoCaminhao = 0;
        Localidade* pontoAtual = this->depositosAbertosLoc[i]; //A ROTA SEMPRE SE INICIA NO DEPÓSITO DE ORIGEM
        rotaParcial.push_back(this->depositosAbertosLoc[i]); //A ROTA SEMPRE SE INICIA NO DEPÓSITO DE ORIGEM
        heuristicas.erase(heuristicas.begin(), heuristicas.end()); //limpando variavel auxiliar

        while(clustersAindaNaoVisitados.size() > 0){
            //CONSTRUINDO E CRIANDO O ARRAY DE HEURÍSTICAS
            for(int t = 0; t < clustersAindaNaoVisitados.size(); t++){
                //CALCULANDO AS DISTÂNCIAS PARA A HEURÍSTICA
                double d1 = fabs(this->calculateDistanceBetweenPoints(pontoAtual->getx(), pontoAtual->gety(), this->depositosAbertosLoc[i]->getx(), this->depositosAbertosLoc[i]->gety()));
                double d2 = fabs(this->calculateDistanceBetweenPoints(this->depositosAbertosLoc[i]->getx(), this->depositosAbertosLoc[i]->gety(), clustersAindaNaoVisitados[t]->getx(), clustersAindaNaoVisitados[t]->gety()));
                double d3 = fabs(this->calculateDistanceBetweenPoints(pontoAtual->getx(), pontoAtual->gety(), clustersAindaNaoVisitados[t]->getx(), clustersAindaNaoVisitados[t]->gety()));
                
                //ENCONTRANDO O FEROMÔNIO PARA O PAR ESCOLHIDO
                std::vector<Localidade*>::iterator itz = std::find(auxLoc.begin(), auxLoc.end(), pontoAtual);
                int indicePontoAtual = itz - auxLoc.begin();
                itz = std::find (auxLoc.begin(), auxLoc.end(), clustersAindaNaoVisitados[t]);
                int indiceProxPonto = itz - auxLoc.begin();

                //ADICIONANDO NO ARRAY DE HEURÍSTISCAS O SEGUINTE PRODUTO: feromônio x heurística^4
                heuristicas.push_back(this->thirdACOpheromoneList[this->thirdACOpheromoneList.size()-1][indicePontoAtual * auxLoc.size() + indiceProxPonto] * pow((d1 + d2 - d3), 4));
                
                //POPULANDO O VETOR DE DEMANDAS DOS CLUSTERS, QUER ME AJUDA A ROTEAR O CAMINHÃO
                vetorDeDemandasDosClusters.push_back(clustersAindaNaoVisitados[t]->clientesNoCluster.size());
            }

            // cout << endl << "imprimindo clusters ainda nao associados e suas demandas:";
            // for(int rr = 0; rr < clustersAindaNaoVisitados.size(); rr++){
            //     cout << "  cluster: " << clustersAindaNaoVisitados[rr]->getId() << "   demanda: " << clustersAindaNaoVisitados[rr]->clientesNoCluster.size() << "  /";
            // }
            auto itaux = std::minmax_element(vetorDeDemandasDosClusters.begin(), vetorDeDemandasDosClusters.end());//calculando o menor elemento do vetor
            int min_idx = std::distance(vetorDeDemandasDosClusters.begin(), itaux.first); //capturando o indice do menor elemento do vetor
            menorDemandaRestante = vetorDeDemandasDosClusters[min_idx]; //capturando a menor demanda restante
            // cout << endl << "imprimindo menor demanda: " << menorDemandaRestante << "   teste: " << vetorDeDemandasDosClusters[min_idx];
            // cout << endl << "imprimindo indice da menor demanda: " << min_idx;
            // cout << endl << "imprimindo capacidade utilizada: " << capacidadeUtilizadaDoCaminhao;
            // cout << endl << "imprimindo capacidade do caminhao: " << this->instancia->getQMAX();

            //VERIFICANDO SE JÁ É HORA DE RETORNAR PARA O DEPÓSITO E RECARREGAR
            if(capacidadeUtilizadaDoCaminhao + menorDemandaRestante > this->instancia->getQMAX()){
                rotaParcial.push_back(this->depositosAbertosLoc[i]); //ADICIONANDO O DEPÓSITO NA ROTA
                capacidadeUtilizadaDoCaminhao = 0;
                pontoAtual = this->depositosAbertosLoc[i];
            } else{ //ESSE CAMINHÃO AINDA CONSEGUE ATENDER CLUSTERS NESSA VIAGEM
                int success = 0;
                while(success == 0){
                    // cout << endl << "imprimindo heuristicas:";
                    // for(int jj = 0; jj < heuristicas.size(); jj++){
                    //     cout << "   " << heuristicas[jj];
                    // }
                    auto it = std::minmax_element(heuristicas.begin(), heuristicas.end());//calculando o máximo elemento do vetor
                    int max_idx = std::distance(heuristicas.begin(), it.second); //capturando o indice do maior elemento do vetor
                    // cout << endl << "imprimindo maior heuristica: " << heuristicas[max_idx];
                    // cout << endl << "imprimindo indice da maior heuristica: " << max_idx;
                    // cout << endl << "demanda desse cliente de maior heuristica: " << clustersAindaNaoVisitados[max_idx]->clientesNoCluster.size();

                    //VERIFICANDO SE O CAMINHÃO CONSEGUE ATENDER ESSE CLUSTER NESSA VIAGEM AINDA
                    if(capacidadeUtilizadaDoCaminhao + clustersAindaNaoVisitados[max_idx]->clientesNoCluster.size() <= this->instancia->getQMAX()){
                        capacidadeUtilizadaDoCaminhao += clustersAindaNaoVisitados[max_idx]->clientesNoCluster.size(); //atualiza capacidade utilizada do caminhão
                        rotaParcial.push_back(clustersAindaNaoVisitados[max_idx]); //inclui novo cluster na rota 
                        pontoAtual = clustersAindaNaoVisitados[max_idx]; //atualiza o ponto atual
                        clustersAindaNaoVisitados.erase(clustersAindaNaoVisitados.begin() + max_idx);//remover o cluster selecionado
                        success = 1;
                    } else{
                        heuristicas[max_idx] = -999; //garantindo que esse ponto não será escolhido na próxima vez
                    }
                }
            }
            heuristicas.erase(heuristicas.begin(), heuristicas.end()); //limpando variavel auxiliar
            vetorDeDemandasDosClusters.erase(vetorDeDemandasDosClusters.begin(), vetorDeDemandasDosClusters.end()); //limpando variavel auxiliar
        }
        rotaParcial.push_back(this->depositosAbertosLoc[i]); //adicionando o depósito, pois é o ponto final da rota
        cout << endl << "Rota para o deposito " << this->depositosAbertosLoc[i]->getId() << ": ";
        for(int x = 0; x < rotaParcial.size(); x++){
            if(x == rotaParcial.size()-1){
                cout << " " << rotaParcial[x]->getId();
            } else{
                if(rotaParcial[x]->gettipo() == 'd'){
                    cout << " " << rotaParcial[x]->getId() << " ->";
                } else{
                    cout << " " << rotaParcial[x]->getId() << "(demanda: " << rotaParcial[x]->clientesNoCluster.size() << ") ->";
                }
            }
        }
        this->solucaoTerceiroACO.push_back(rotaParcial);//salvando a rota na própria colônia
        rotaParcial.erase(rotaParcial.begin(), rotaParcial.end()); //limpando variavel auxiliar        
    }
}

//OUTROS MÉTODOS AUXILIARES
void Colony::calculateFinalCosts(){
    cout << endl << endl << "FUNCAO FINAL COSTS";
    double totalCost = 0;
    for(int i = 0; i < this->solucaoTerceiroACO.size(); i++){
        this->finalCosts.push_back(this->solucaoTerceiroACO[i][0]->custoDoDeposito); //SOMANDO O CUSTO DE FUNCIONAMENTO DIÁRIO DO DEPÓSITO
        for(int j = 0; j < this->solucaoTerceiroACO[i].size()-1; j++){
            double dist = this->calculateDistanceBetweenPoints(this->solucaoTerceiroACO[i][j]->getx(), this->solucaoTerceiroACO[i][j]->gety(), this->solucaoTerceiroACO[i][j+1]->getx(), this->solucaoTerceiroACO[i][j+1]->gety());
            dist = dist * 2.46; //CIRCUITY FACTOR DO CAMINHÃO
            this->finalCosts[i] += dist * this->instancia->getCT(); //custo de deslocamento do caminhão nesse trecho

            //SE O PONTO ATUAL FOR UM CLUSTER, SOMA O CUSTO DO CLUSTER
            if(this->solucaoTerceiroACO[i][j]->gettipo() != 'd'){
                this->finalCosts[i] += this->solucaoTerceiroACO[i][j]->custoParaAtenderCluster;
            } else{ //SE O PONTO ATUAL FOR UM DEPÓSITO, É NECESSÁRIO ADICIONAR MAIS UM CUSTO DE USO DE CAMINHÃO
                this->finalCosts[i] += this->instancia->getFTRUCK(); //custo fixo por usar um caminhão
            }
        }
        cout << endl << "Custo total para o deposito " << this->solucaoTerceiroACO[i][0]->getId() << ": " << this->finalCosts[i];
        totalCost += this->finalCosts[i]; 
    }
    cout << endl << "Custo final para a instancia: " << totalCost;
}

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