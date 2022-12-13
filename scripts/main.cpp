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
#include <chrono>
#include <bits/stdc++.h>
#include "Localidade.h"
#include "Instancia.h"
#include "Colony.h"
using namespace std;
using namespace std::chrono;

//---------------------VARIÁVEIS GLOBAIS-------------------------------------//
Instancia *instancia;
vector<float> array_distancias_ordenado;
vector<string> array_pares_ids_ordenado;
vector<string> clusters_ids;
vector<vector<string>> clusters;
vector<float> centroidsX;
vector<float> centroidsY;
vector<int> selected_depots;
vector<float> depotsCoordX;
vector<float> depotsCoordY;
vector<Localidade*> openDepotsCONSTRUTIVO;
vector<vector<string>> solutionCONSTRUTIVO;//TEM O FORMATO DE UM ARRAY, COM UMA POSIÇÃO PRA CADA DEPÓSITO ABERTO. CADA POSIÇÃO É UM ARRAY COM OS IDS DOS PARKING SPOTS VINCULADOS AQUELE DEPÓSITO
float droneRange = 2000.00; // EM METROS
string arquivoInstancias = "inst4000_cli300_dfc356_dc170";
string arquivoConfiguracoes = "configuracoes_1.txt";
int TAMANHO_INSTANCIA = 4; //VARIÁVEL PARA DEFINIR O TAMANHO DA INSTANCIA PARA GERAR O CUSTO ALEATORIO DE CADA DEPÓSITO (4, 7, 11)
//---------------------------------------------------------------------------//

//--------------------MÉTODOS AUXILIARES-------------------------------------//
float calculateCentroidCoordinate(vector<float> coordinates)
{
    float sum = 0;
    for (int i = 0; i < coordinates.size(); i++)
    {
        sum += coordinates[i];
    }

    return sum / coordinates.size();
}

int findLocationById(string id)
{
    int index;
    for (int i = 0; i < instancia->locaisDefinidos.size(); i++)
    {
        if (instancia->locaisDefinidos[i]->getId() == id)
        {
            index = i;
            break;
        }
    }
    return index;
}

float calculateDistanceBetweenPoints(float xa, float ya, float xb, float yb)
{
    return sqrt(pow((xa - xb), 2) + pow((ya - yb), 2));
}

Instancia *leituraDosArquivosDeEntrada(string arquivoEntradaInstancia, string arquivoEntradaConfig)
{
    // PRIMEIRO LENDO O ARQUIVO REFERENTE À INSTANCIA
    ifstream arquivo(arquivoEntradaInstancia);
    if (!arquivo.is_open())
    {
        cout << "Erro ao abrir aquivo: " << arquivoEntradaInstancia << "  --> Encerrando programa." << endl;
        exit(1);
    }

    Instancia *novaInstancia = new Instancia();
    string lixo, id, tipo, x, y;

    getline(arquivo, lixo, '\n'); // Lendo e descartando a primeira linha do arquivo
    while (arquivo.good())
    { // Lendo o restante do arquivo, cada linha vira um cliente ou depósito
        arquivo >> id;
        if (id[0] == 'D' || id[0] == 'C')
        {
            if (id[0] == 'D')
                novaInstancia->somaDepositos();
            else
                novaInstancia->somaClientes();

            arquivo >> tipo;
            arquivo >> x;
            arquivo >> y;
            Localidade *novoLocal = new Localidade(id, tipo[0], stof(x), stof(y));
            novaInstancia->insereLocal(novoLocal);
        }
        else
        {
            if (id[0] == 'T')
            {
                getline(arquivo, lixo, '/');
                getline(arquivo, lixo, '/');
                getline(arquivo, lixo, '/');
                getline(arquivo, lixo, '/');
                getline(arquivo, lixo, '/');
                getline(arquivo, lixo, '/');
                break;
            }
        }
    }
    novaInstancia->atualizaTamanho();
    novaInstancia->criaMatriz();
    arquivo.close();
    cout << endl
         << "A matriz de distancias foi calculada e exportada no arquivo 'Matriz.txt'." << endl;
    novaInstancia->getMatrizDistancias();

    // AGORA LENDO O ARQUIVO REFERENTE ÀS CONFIGURAÇÕES
    ifstream arquivoConfig(arquivoEntradaConfig);
    if (!arquivoConfig.is_open())
    {
        cout << "Erro ao abrir aquivo: " << arquivoEntradaConfig << "  --> Encerrando programa." << endl;
        exit(1);
    }

    while (arquivoConfig.good())
    { // Lendo o arquivo, cada linha vira um dado da instancia
        string aux;
        getline(arquivoConfig, lixo, '/');
        getline(arquivoConfig, aux, '/');
        novaInstancia->setFDRONE(stod(aux));

        getline(arquivoConfig, lixo, '/');
        getline(arquivoConfig, aux, '/');
        novaInstancia->setFTRUCK(stod(aux));

        getline(arquivoConfig, lixo, '/');
        getline(arquivoConfig, aux, '/');
        novaInstancia->setCD(stod(aux));

        getline(arquivoConfig, lixo, '/');
        getline(arquivoConfig, aux, '/');
        novaInstancia->setCT(stod(aux));

        getline(arquivoConfig, lixo, '/');
        getline(arquivoConfig, aux, '/');
        novaInstancia->setCTW(stod(aux));

        getline(arquivoConfig, lixo, '/');
        getline(arquivoConfig, aux, '/');
        novaInstancia->setDronesPorCaminhao(stoi(aux));

        getline(arquivoConfig, lixo, '/');
        getline(arquivoConfig, aux, '/');
        novaInstancia->setVD(stoi(aux));

        getline(arquivoConfig, lixo, '/');
        getline(arquivoConfig, aux, '/');
        novaInstancia->setVT(stoi(aux));

        getline(arquivoConfig, lixo, '/');
        getline(arquivoConfig, aux, '/');
        novaInstancia->setSI(stoi(aux));

        getline(arquivoConfig, lixo, '/');
        getline(arquivoConfig, aux, '/');
        novaInstancia->setTUNLOAD(stoi(aux));

        getline(arquivoConfig, lixo, '/');
        getline(arquivoConfig, aux, '/');
        novaInstancia->setTLOAD(stoi(aux));

        getline(arquivoConfig, lixo, '/');
        getline(arquivoConfig, aux, '/');
        novaInstancia->setKDRONE(stod(aux));

        getline(arquivoConfig, lixo, '/');
        getline(arquivoConfig, aux, '/');
        novaInstancia->setKTRUCK(stod(aux));

        break;
    }
    arquivoConfig.close();
    // IMPRIMINDO DADOS DE TESTE NA TELA
    //  cout << endl << "DADOS DO ARQUIVO DE INSTANCIA:";
    //  cout << endl << "Numero de depositos: " << novaInstancia->getDepositos();
    //  cout << endl << "Numero de clientes: " << novaInstancia->getClientes();
    //  cout << endl << "Capacidade dos caminhoes: " << novaInstancia->getQMAX() << " parcelas";
    //  cout << endl << endl << "DADOS DO ARQUIVO DE CONFIGURACAO:";
    //  cout << endl << "FDRONE: " << novaInstancia->getFDRONE();
    //  cout << endl << "FTRUCK: " << novaInstancia->getFTRUCK();
    //  cout << endl << "CD: " << novaInstancia->getCD();
    //  cout << endl << "CT: " << novaInstancia->getCT();
    //  cout << endl << "CTW: " << novaInstancia->getCTW();
    //  cout << endl << "H: " << novaInstancia->getDronesPorCaminhao();
    //  cout << endl << "VD: " << novaInstancia->getVD();
    //  cout << endl << "VT: " << novaInstancia->getVT();
    //  cout << endl << "SI: " << novaInstancia->getSI();
    //  cout << endl << "TUNLOAD: " << novaInstancia->getTUNLOAD();
    //  cout << endl << "TLOAD: " << novaInstancia->getTLOAD();
    //  cout << endl << "KDRONE: " << novaInstancia->getKDRONE();
    //  cout << endl << "KTRUCK: " << novaInstancia->getKTRUCK();

    novaInstancia->criaArraysDeDepositosEclientes();//PREENCHENDO OS VETORES DE CLIENTES E DEPOSITOS NA INSTANCIA
    novaInstancia->calculaCustosEcapacidadesDosDepositosEcaminhoes(TAMANHO_INSTANCIA); //CALCULANDO CUSTOS E CAPACIDADES DE CADA DEPÓSITO E DOS CAMINHÕES
    return novaInstancia;
}

void calculateSortedDistancesAndIdsArrays()
{
    vector<float> array_distancias;
    vector<string> array_pares_ids;
    string aux;

    // TRANSFORMANDO A MATRIZ EM DOIS ARRAYS, COM TODOS OS PARES DE IDS E TODAS AS DISTANCIAS (EXCETO OS QUE ENVOLVEM DEPOSITOS)
    for (int i = 0; i < instancia->locaisDefinidos.size(); i++)
    {
        for (int j = 0; j < instancia->locaisDefinidos.size(); j++)
        {
            if (instancia->locaisDefinidos[i]->gettipo() != 'd' && instancia->locaisDefinidos[j]->gettipo() != 'd')
            {
                array_distancias.push_back(instancia->matriz_distancias[i][j]);
                aux = instancia->locaisDefinidos[i]->getId() + "-" + instancia->locaisDefinidos[j]->getId();
                array_pares_ids.push_back(aux);
            }
        }
    }

    // ORDENANDO E LIMPANDO O ARRAY DE DISTANCIAS
    array_distancias_ordenado = array_distancias;
    std::sort(array_distancias_ordenado.begin(), array_distancias_ordenado.end());
    // AQUI TEMOS UM PONTO IMPORTANTE, VAI ESTAR ERRADO CASO TENHAMOS DISTANCIAS IGUAIS ENTRE PARES DIFERENTES, O QUE EH MUITO IMPROVAVEL
    // TAMBEM VAI ESTAR ERRADO CASO TENHAMOS UM CLIENTE COM DUAS ENTREGAS A SEREM FEITAS (CLIENTE REPETIDO)
    // ELIMINANDO DISTANCIAS DUPLICADAS (PARES) E O PRIMEIRO ELEMENTO (0)
    array_distancias_ordenado.erase(unique(array_distancias_ordenado.begin(), array_distancias_ordenado.end()), array_distancias_ordenado.end());
    array_distancias_ordenado.erase(array_distancias_ordenado.begin());

    // cout << "ARRAY DE DISTANCIAS ANTES DE ORDENAR: " << endl;
    // for(int i = 0; i < array_distancias.size(); i++){
    //     cout << array_distancias[i] << "  -  ";
    // }
    // cout << endl << endl << "ARRAY DE DISTANCIAS DEPOIS DE ORDENAR E DE LIMPAR: " << endl;
    // for(int i = 0; i < array_distancias_ordenado.size(); i++){
    //     cout << array_distancias_ordenado[i] << "  -  ";
    // }

    // EU PERCORRO O ARRAY DE DISTANCIAS ORDENADO E "LIMPO" E ENCONTRO OS ELEMENTOS NO ARRAY DE DISTANCIAS ORIGINAL
    // O INDICE CORRESPONDE COM O INDICE DO ARRAY DE IDS ORIGINAL
    // COM BASE NISSO EU CRIO O ARRAY DE IDS ORDENADO, CORRESPONDENDO O ARRAY DE DISTANCIAS ORDENADO

    vector<float>::iterator it;
    for (int i = 0; i < array_distancias_ordenado.size(); i++)
    {
        it = std::find(array_distancias.begin(), array_distancias.end(), array_distancias_ordenado[i]);
        array_pares_ids_ordenado.push_back(array_pares_ids[(it - array_distancias.begin())]);
    }

    // cout << endl << endl << "ARRAY DE IDS ANTES DA ORDENACAO: " << endl;
    // for(int i = 0; i < array_pares_ids.size(); i++){
    //     cout << array_pares_ids[i] << "  /  ";
    // }
    array_pares_ids_ordenado.erase(unique(array_pares_ids_ordenado.begin(), array_pares_ids_ordenado.end()), array_pares_ids_ordenado.end());
    // cout << endl << endl << "ARRAY DE IDS DEPOIS DA ORDENACAO: " << endl;
    // for(int i = 0; i < array_pares_ids_ordenado.size(); i++){
    //     cout << array_pares_ids_ordenado[i] << "  /  ";
    // }
}

void clusterInsertion(){
    vector<float> yCoordinates;
    vector<float> xCoordinates;
    vector<string> aux_vector;
    string pointA, pointB;
    int specialCharIndex, locationIndex, indexClusterA, indexClusterB;
    float centroidX, centroidY;
    bool foundA = false, foundB = false, aux = false;

    // PERCORRENDO A LISTA DE PARES DE IDS
    for (int i = 0; i < array_pares_ids_ordenado.size(); i++)
    {
        // capturando os ids dos pontos do par
        specialCharIndex = array_pares_ids_ordenado[i].find("-");
        pointA = array_pares_ids_ordenado[i].substr(0, specialCharIndex); // ID DO PONTO A
        pointB = array_pares_ids_ordenado[i].substr(specialCharIndex + 1, array_pares_ids_ordenado.size()); // ID DO PONTO B

        // VERIFICANDO SE ESSES PONTOS JA ESTAO EM ALGUM CLUSTER
        for (int j = 0; j < clusters.size(); j++)
        {
            for (int h = 0; h < clusters[j].size(); h++)
            {
                if (!foundA)
                {
                    if (std::find(clusters[j].begin(), clusters[j].end(), pointA) != clusters[j].end())
                    {
                        // pointA encontrado
                        foundA = true;
                        indexClusterA = j;
                    }
                }

                if (!foundB)
                {
                    if (std::find(clusters[j].begin(), clusters[j].end(), pointB) != clusters[j].end())
                    {
                        // pointB encontrado
                        foundB = true;
                        indexClusterB = j;
                    }
                }
            }
            //se ja tiver encontrado os dois pode parar de procurar
            if(foundA && foundB){
                break;
            }
        }

        if (foundA && foundB && indexClusterA != indexClusterB)
        {
            // os pontos estao em clusters diferentes. Vamos tentar unir os clusters.
            // verifica se o numero de pontos no cluster uniao eh aceitavel. Se for, tenta fazer a uniao caso o centroide seja viavel
            if ((clusters[indexClusterA].size() + clusters[indexClusterB].size()) <= instancia->getQMAX())
            {
                // captura as coordenadas de todos os pontos
                xCoordinates.erase(xCoordinates.begin(), xCoordinates.end());
                yCoordinates.erase(yCoordinates.begin(), yCoordinates.end());
                for (int k = 0; k < clusters[indexClusterA].size(); k++)
                {
                    locationIndex = findLocationById(clusters[indexClusterA][k]);
                    xCoordinates.push_back(instancia->locaisDefinidos[locationIndex]->getx());
                    yCoordinates.push_back(instancia->locaisDefinidos[locationIndex]->gety());
                }
                for (int k = 0; k < clusters[indexClusterB].size(); k++)
                {
                    locationIndex = findLocationById(clusters[indexClusterB][k]);
                    xCoordinates.push_back(instancia->locaisDefinidos[locationIndex]->getx());
                    yCoordinates.push_back(instancia->locaisDefinidos[locationIndex]->gety());
                }
                // calcula o novo centroide
                centroidX = calculateCentroidCoordinate(xCoordinates);
                centroidY = calculateCentroidCoordinate(yCoordinates);
                // verifica se o novo centroide eh viavel em relacao aos pontos
                for (int k = 0; k < xCoordinates.size(); k++)
                {
                    if ((droneRange < 2 * calculateDistanceBetweenPoints(centroidX, centroidY, xCoordinates[k], yCoordinates[k])))
                    {
                        aux = true;
                    }
                }
                if (!aux)
                {
                    // Todos os pontos alcancam o centroide, logo eh viavel, vamos unir os clusters
                    for (int k = 0; k < clusters[indexClusterB].size(); k++)
                    {
                        clusters[indexClusterA].push_back(clusters[indexClusterB][k]);
                    }
                    clusters.erase(clusters.begin() + indexClusterB);
                }
                else
                {
                    aux = false;
                }
            }
        }
        else
        {
            if (foundA && !foundB)
            {
                // somente o pointA esta em um cluster. Entao tentamos inserir o pointB nesse cluster e ver se continua viavel.
                // se continuar, ok. Se nao, o pointB vira um cluster individual;

                if (clusters[indexClusterA].size() >= instancia->getQMAX())
                {
                    // nao cabe mais ninguem no cluster, eh criado um cluster individual
                    aux_vector.erase(aux_vector.begin(), aux_vector.end()); // limpando a variavel auxiliar
                    aux_vector.push_back(pointB);
                    clusters.push_back(aux_vector);
                    aux_vector.erase(aux_vector.begin(), aux_vector.end()); // limpando a variavel auxiliar
                }
                else
                {
                    // captura as coordenadas do pointB
                    locationIndex = findLocationById(pointB);
                    xCoordinates.erase(xCoordinates.begin(), xCoordinates.end());
                    yCoordinates.erase(yCoordinates.begin(), yCoordinates.end());
                    xCoordinates.push_back(instancia->locaisDefinidos[locationIndex]->getx());
                    yCoordinates.push_back(instancia->locaisDefinidos[locationIndex]->gety());
                    // captura as coordenadas dos pontos ja presentes no cluster
                    for (int k = 0; k < clusters[indexClusterA].size(); k++)
                    {
                        locationIndex = findLocationById(clusters[indexClusterA][k]);
                        xCoordinates.push_back(instancia->locaisDefinidos[locationIndex]->getx());
                        yCoordinates.push_back(instancia->locaisDefinidos[locationIndex]->gety());
                    }
                    // calcula o novo centroide
                    centroidX = calculateCentroidCoordinate(xCoordinates);
                    centroidY = calculateCentroidCoordinate(yCoordinates);
                    // verifica se o novo centroide eh viavel em relacao aos pontos
                    for (int k = 0; k < xCoordinates.size(); k++)
                    {
                        if ((droneRange < 2 * calculateDistanceBetweenPoints(centroidX, centroidY, xCoordinates[k], yCoordinates[k])))
                        {
                            aux = true;
                        }
                    }
                    if (aux)
                    {
                        // algum ponto nao alcanca o centroide, logo nao eh viavel
                        aux_vector.erase(aux_vector.begin(), aux_vector.end()); // limpando a variavel auxiliar
                        aux_vector.push_back(pointB);
                        clusters.push_back(aux_vector);
                        aux_vector.erase(aux_vector.begin(), aux_vector.end()); // limpando a variavel auxiliar
                        aux = false;
                    }
                    else
                    {
                        // o cluster original adicionado do pointB eh viavel
                        clusters[indexClusterA].push_back(pointB);
                    }
                }
            }
            else
            {
                if (!foundA && foundB)
                {
                    // somente o pointB esta em um cluster. Entao tentamos inserir o pointA nesse cluster e ver se continua viavel.
                    // se continuar, ok. Se nao, o pointA vira um cluster individual;

                    if (clusters[indexClusterB].size() >= instancia->getQMAX())
                    {
                        // nao cabe mais ninguem no cluster, eh criado um cluster individual
                        aux_vector.erase(aux_vector.begin(), aux_vector.end()); // limpando a variavel auxiliar
                        aux_vector.push_back(pointA);
                        clusters.push_back(aux_vector);
                        aux_vector.erase(aux_vector.begin(), aux_vector.end()); // limpando a variavel auxiliar
                    }
                    else
                    {
                        // captura as coordenadas do pointA
                        locationIndex = findLocationById(pointA);
                        xCoordinates.erase(xCoordinates.begin(), xCoordinates.end());
                        yCoordinates.erase(yCoordinates.begin(), yCoordinates.end());
                        xCoordinates.push_back(instancia->locaisDefinidos[locationIndex]->getx());
                        yCoordinates.push_back(instancia->locaisDefinidos[locationIndex]->gety());
                        // captura as coordenadas dos pontos ja presentes no cluster
                        for (int k = 0; k < clusters[indexClusterB].size(); k++)
                        {
                            locationIndex = findLocationById(clusters[indexClusterB][k]);
                            xCoordinates.push_back(instancia->locaisDefinidos[locationIndex]->getx());
                            yCoordinates.push_back(instancia->locaisDefinidos[locationIndex]->gety());
                        }
                        // calcula o novo centroide
                        centroidX = calculateCentroidCoordinate(xCoordinates);
                        centroidY = calculateCentroidCoordinate(yCoordinates);
                        // verifica se o novo centroide eh viavel em relacao aos pontos
                        for (int k = 0; k < xCoordinates.size(); k++)
                        {
                            if ((droneRange < 2 * calculateDistanceBetweenPoints(centroidX, centroidY, xCoordinates[k], yCoordinates[k])))
                            {
                                aux = true;
                            }
                        }
                        if (aux)
                        {
                            // algum ponto nao alcanca o centroide, logo nao eh viavel
                            aux_vector.erase(aux_vector.begin(), aux_vector.end()); // limpando a variavel auxiliar
                            aux_vector.push_back(pointA);
                            clusters.push_back(aux_vector);
                            aux_vector.erase(aux_vector.begin(), aux_vector.end()); // limpando a variavel auxiliar
                            aux = false;
                        }
                        else
                        {
                            // o cluster original adicionado do pointA eh viavel
                            clusters[indexClusterB].push_back(pointA);
                        }
                    }
                }
                else
                {
                    if (!foundA && !foundB)
                    {
                        // nenhum dos pontos esta em cluster. Cria um cluster com os dois pontos, se for viavel.
                        // Se nao, cria um cluster para cada ponto.

                        // captura as coordenadas dos dois pontos
                        xCoordinates.erase(xCoordinates.begin(), xCoordinates.end());
                        yCoordinates.erase(yCoordinates.begin(), yCoordinates.end());
                        locationIndex = findLocationById(pointA);
                        xCoordinates.push_back(instancia->locaisDefinidos[locationIndex]->getx());
                        yCoordinates.push_back(instancia->locaisDefinidos[locationIndex]->gety());
                        locationIndex = findLocationById(pointB);
                        xCoordinates.push_back(instancia->locaisDefinidos[locationIndex]->getx());
                        yCoordinates.push_back(instancia->locaisDefinidos[locationIndex]->gety());
                        // calcula o centroide do cluster
                        centroidX = calculateCentroidCoordinate(xCoordinates);
                        centroidY = calculateCentroidCoordinate(yCoordinates);
                        // verifica se o cluster eh viavel
                        if ((droneRange >= 2.0 * calculateDistanceBetweenPoints(centroidX, centroidY, xCoordinates[0], yCoordinates[0])) &&
                            (droneRange >= 2.0 * calculateDistanceBetweenPoints(centroidX, centroidY, xCoordinates[1], yCoordinates[1])))
                        {
                            // cria um cluster com os dois pontos
                            aux_vector.erase(aux_vector.begin(), aux_vector.end()); // limpando a variavel auxiliar
                            aux_vector.push_back(pointA);
                            aux_vector.push_back(pointB);
                            clusters.push_back(aux_vector);
                            aux_vector.erase(aux_vector.begin(), aux_vector.end()); // limpando a variavel auxiliar
                        }
                        else
                        {
                            // cria um cluster para cada ponto
                            aux_vector.erase(aux_vector.begin(), aux_vector.end());
                            aux_vector.push_back(pointA);
                            clusters.push_back(aux_vector);
                            aux_vector.erase(aux_vector.begin(), aux_vector.end()); // limpando a variavel auxiliar
                            aux_vector.push_back(pointB);
                            clusters.push_back(aux_vector);
                            aux_vector.erase(aux_vector.begin(), aux_vector.end()); // limpando a variavel auxiliar
                        }
                    }
                }
            }
        }

        aux = false;
        foundA = false;
        foundB = false;
   }

    // IMPRIMINDO CLUSTERS
    cout << endl << endl << "Numero de clusters: " << clusters.size() << endl;
    for (int i = 0; i < clusters.size(); i++)
    {
        for (int j = 0; j < clusters[i].size(); j++)
        {
            cout << clusters[i][j] << "   ";
        }
        cout << endl;
    }
}

void calculateCentroidsCoordinates(){
    //APOS A CLUSTERIZACAO, ESSA FUNCAO PERMITE CALCULAR OS CENTROIDES DA CONFIGURACAO FINAL DE CLUSTERS
    //UTILIZEI ESSA FUNCAO PARA CALCULAR OS CENTROIDES FINAIS E IMPRIMIR EM UM ARQUIVO QUE SERA UTILIZADO PARA
    //A PLOTAGEM DOS CLUSTERS

    //VARIAVEIS AUXILIARES
    int locationIndex;
    vector<float> yCoordinates;
    vector<float> xCoordinates;
    float centroidCoordAux;

    for (int i = 0; i < clusters.size(); i++)
    {
        for (int j = 0; j < clusters[i].size(); j++)
        {
            //captura as coordenadas dos clientes do cluster
            locationIndex = findLocationById(clusters[i][j]);
            xCoordinates.push_back(instancia->locaisDefinidos[locationIndex]->getx());
            yCoordinates.push_back(instancia->locaisDefinidos[locationIndex]->gety());
        }

            // calcula o centroide do cluster
            centroidCoordAux = calculateCentroidCoordinate(xCoordinates);
            centroidsX.push_back(centroidCoordAux);
            centroidCoordAux = calculateCentroidCoordinate(yCoordinates);
            centroidsY.push_back(centroidCoordAux);
            xCoordinates.erase(xCoordinates.begin(), xCoordinates.end());
            yCoordinates.erase(yCoordinates.begin(), yCoordinates.end());
    }

    // //IMPRESSOES DE TESTE
    // cout << endl << "TESTANDO VETOR DE CENTROIDES DOS CLUSTERS" << endl;
    // for(int i = 0; i < centroidsX.size(); i++){
    //     cout << "CLUSTER " << i << ":   X-> " <<  centroidsX[i] << "   Y-> " << centroidsY[i] << endl;
    // }
}

void calculateDepotsCoordinates(){
    //ESSA FUNÇÃO CALCULA AS COORDENADAS DOS DEPÓSITOS E GUARDA NAS VARIÁVEIS GLOBAIS 'depotsCoordX' E 'depotsCoordY'
    
    //VARIAVEIS AUXILIARES
    int locationIndex;
    int d = instancia->getDepositos(); //NÚMERO DE DEPÓSITOS

    vector<string> auxDepotsIds;
    auxDepotsIds.push_back("D0");
    auxDepotsIds.push_back("D1");
    auxDepotsIds.push_back("D2");
    if(d == 4){
        auxDepotsIds.push_back("D3");
    } else{
        if(d == 5){
            auxDepotsIds.push_back("D3");
            auxDepotsIds.push_back("D4");
        }
    }

    for(int i = 0; i < d; i++){
        locationIndex = findLocationById(auxDepotsIds[i]);
        depotsCoordX.push_back(instancia->locaisDefinidos[locationIndex]->getx());
        depotsCoordY.push_back(instancia->locaisDefinidos[locationIndex]->gety());
    }

    //IMPRESSOES DE TESTE
    // cout << endl << "TESTANDO VETOR DE COORDENADAS DOS DEPOSITOS" << endl;
    // for(int i = 0; i < depotsCoordX.size(); i++){
    //     cout << "DEPOSITO " << i << ":   X-> " <<  depotsCoordX[i] << "   Y-> " << depotsCoordY[i] << endl;
    // }
}

void generateClusterPlotFile(){
    //CRIANDO ARQUIVO
    ofstream arq(arquivoInstancias+"_CLUSTER_PLOT_FILE.txt", ios::out);
    arq << "Cluster  Point   CentroidX       CentroidY"; //CABECALHOS DO ARQUIVO
    arq << endl;

    //IMPRIMINDO PONTOS NO ARQUIVO
    for (int i = 0; i < clusters.size(); i++)
    {
        for (int j = 0; j < clusters[i].size(); j++)
        {
            arq << i+1 << "         " << clusters[i][j] << "       " << centroidsX[i] << "          " << centroidsY[i] << endl;
        }
    }

    cout << endl << "Arquivo com dados dos clusters foi gerado." << endl;
}

void calculateClustersArrayLoc(){
    //ESSE MÉTODO CRIA UM ARRAY ONDE CADA POSIÇÃO É UM ARRAY QUE INDICA UM CLUSTER. A DIFERENÇA É QUE O TIPO É LOCALIDADE.
    int locationIndex;
    vector<Localidade*> auxVector;

    for(int i = 0; i < clusters.size(); i++){
        for(int j = 0; j < clusters[i].size(); j++){
            //captura as coordenadas dos clientes do cluster
            locationIndex = findLocationById(clusters[i][j]);
            auxVector.push_back(instancia->locaisDefinidos[locationIndex]);
        }
        instancia->clusters.push_back(auxVector);
        auxVector.erase(auxVector.begin(), auxVector.end());
    }
}

void calculateClustersTimesAndCosts(){
    //FUNCAO QUE CALCULA, PARA CADA CLUSTER, O TEMPO DE ATENDIMENTO E O CUSTO DE ATENDIMENTO
    //OS VALORES CALCULADOS SAO ARMAZENADOS EM ATRIBUTOS DA CLASSE LOCALIDADE

    //VARIAVEIS AUXILIARES
    vector<float> times;
    vector<float> droneTimesAux;
    float costAux;

    for (int i = 0; i < instancia->clusters.size(); i++)
    {
        costAux = 0.0;
        for(int j = 0; j < instancia->clusters[i].size(); j++){
            float distance = 2.0 * calculateDistanceBetweenPoints(instancia->clusters[i][j]->getx(), instancia->clusters[i][j]->gety(), centroidsX[i], centroidsY[i]);
            times.push_back((distance / float(instancia->getVD())) + instancia->getTUNLOAD() + instancia->getTLOAD());
            costAux += distance;
        }
        instancia->parkingSpots[i]->custoParaAtenderCluster = costAux * instancia->getCD();

        //agora ja temos o tempo que cada cliente demanda, logo podemos fazer uma distribuicao de tarefas entre os drones
        //sao sempre 3 drones por caminhao. Comecamos pelos maiores tempos.
        //ADICIONANDO O TRUCK IDLE WAITING COST PARA ESSE CLUSTER E CUSTO DE USO DIÁRIO DE DRONES
        //CASO TENHAMOS 1 CLIENTE NO CLUSTER
        if(times.size() == 1){
            instancia->parkingSpots[i]->custoParaAtenderCluster += (times[0] * instancia->getCTW()) + instancia->getFDRONE(); //CUSTO DE USO DIÁRIO DE 1 DRONE
        } else{
            std::sort(times.begin(), times.end());
            //CASO TENHAMOS 2 CLIENTES NO CLUSTER
            if(times.size() == 2){
                instancia->parkingSpots[i]->custoParaAtenderCluster += (times[1] * instancia->getCTW()) + (2 * instancia->getFDRONE()); //CUSTO DE USO DIÁRIO DE 2 DRONES
            } else{
                // //CASO TENHAMOS 3 OU MAIS CLIENTES NO CLUSTER
                droneTimesAux.push_back(times[times.size()-1]);
                droneTimesAux.push_back(times[times.size()-2]);
                droneTimesAux.push_back(times[times.size()-3]);
                int m = times.size()-4;
                while(m > 0){
                    std::sort(droneTimesAux.begin(), droneTimesAux.end());   
                    droneTimesAux[0] += times[m];
                    m--;
                }
                std::sort(droneTimesAux.begin(), droneTimesAux.end());
                instancia->parkingSpots[i]->custoParaAtenderCluster += (droneTimesAux[droneTimesAux.size()-1] * instancia->getCTW()) + (3 * instancia->getFDRONE()); //CUSTO DE USO DIÁRIO DE 3 DRONES
                droneTimesAux.erase(droneTimesAux.begin(), droneTimesAux.end());
            }
        }
        times.erase(times.begin(), times.end()); //limpando variável auxiliar
    }

    //IMPRIMINDO CUSTOS DE ATENDIMENTO DE CADA CLUSTER
    // cout << endl << "CUSTOS PARCIAIS DE ATENDIMENTO DOS CLUSTERS" << endl;
    // for(int i = 0; i < instancia->parkingSpots.size(); i++){
    //     cout << "Cluster " << i+1 << ": " << instancia->parkingSpots[i]->custoParaAtenderCluster << endl;
    // }

    //LEMBRANDO QUE ESSES VALORES SÃO PARCIAIS PORQUE AINDA TEMOS OS CUSTOS E TEMPOS RELATIVOS AOS DESLOCAMENTOS DOS CAMINHÕES, QUE
    //SERÃO CALCULADOS NA ETAPA DE ROTEAMENTO. ALÉM DISSO, AINDA TEMOS OS CUSTOS DE USO DIÁRIO DOS CAMINHÕES E OS CUSTOS DE SE ABRIR UM DEPÓSITO.
}

float somatorioDistancias(vector<float> distances){
    float soma = 0;
    for(int i = 0; i < distances.size(); i++){
        soma += distances[i];
    }
    return soma;
}

void createParkinkgSpotLocations(){
    //FUNÇÃO QUE CRIA, PARA CADA PARKING SPOT, UMA LOCALIDADE, E PREENCHE O VETOR DE PARKING SPOTS NA INSTANCIA
    for(int i = 0; i < clusters.size(); i++){
        Localidade *novoLocal = new Localidade("PS"+to_string(i), 'p', centroidsX[i], centroidsY[i]);
        novoLocal->clientesNoCluster = clusters[i];
        instancia->insereLocal(novoLocal);
        instancia->parkingSpots.push_back(novoLocal);
    }
}

void calculateClustersDistances(){
    //calcula as distâncias entre todos os clusters e salva em uma matriz na classe instancia
    vector<double> auxVector;
    for(int i = 0; i < instancia->parkingSpots.size(); i++){
        for(int j = 0; j < instancia->parkingSpots.size(); j++){
            auxVector.push_back(calculateDistanceBetweenPoints(instancia->parkingSpots[i]->getx(), instancia->parkingSpots[i]->gety(), instancia->parkingSpots[j]->getx(), instancia->parkingSpots[j]->gety()));
        }
        instancia->distanciasEntreParkingSpots.push_back(auxVector);
        auxVector.erase(auxVector.begin(), auxVector.end());
    }
}

void algoritmoConstrutivoLocalizacao(){
    //FUNÇÃO QUE IMPLEMENTA O ALGORITMO CONSTRUTIVO QUE SOLUCIONA A PRIMEIRA ETAPA DO PROBLEMA: ASSOCIAR CLUSTERS A DEPÓSITOS
    //vou utilizar o seguinte método: criar uma estrutura que guarda as distancias dos depósitos ate os clusters. Então, selecionar o
    //depósito que seja mais próximo de todos os clusters (somando todas as distâncias), e ocupar esse depósito com os clientes mais próximos,
    //até estourar sua capacidade. A partir dai, gerar novamente a estrutura com as distâncias entre depósitos e clusters, desconsiderando os
    //depósitos e clusters já associados.

    //ARRAYS QUE GUARDAM DEPÓSITOS E PARKING SPOTS AINDA NÃO ASSOCIADOS
    vector<Localidade*> depositosDisponiveis = instancia->depositosLoc;
    vector<Localidade*> parkingSpotsDisponiveis = instancia->parkingSpots;
    
    //ENQUANTO AINDA TIVER CLUSTERS PARA ASSOCIAR
    while(parkingSpotsDisponiveis.size() > 0){
        //CALCULANDO AS DISTÂNCIAS ENTRE OS DEPÓSITOS E PARKING SPOTS AINDA NÃO ASSOCIADOS
        vector<float> auxVector;
        vector<vector<float>> distances;
        for(int i = 0; i < depositosDisponiveis.size(); i++){
            for(int j = 0; j < parkingSpotsDisponiveis.size(); j++){
                auxVector.push_back(calculateDistanceBetweenPoints(depositosDisponiveis[i]->getx(), depositosDisponiveis[i]->gety(), 
                parkingSpotsDisponiveis[j]->getx(), parkingSpotsDisponiveis[j]->gety()));
            }
            distances.push_back(auxVector);
            auxVector.erase(auxVector.begin(), auxVector.end()); //limpando variavel auxiliar
        }

        //ENCONTRANDO O DEPÓSITO MAIS PRÓXIMOS DOS PARKING SPOTS
        float aux;
        float menor = somatorioDistancias(distances[0]);
        int menorIndice = 0;
        for(int i = 1; i < distances.size(); i++){
            aux = somatorioDistancias(distances[i]);
            if(aux < menor){
                menor = aux;
                menorIndice = i; 
            }
        }

        //LOGO, O DEPÓSITO depositosDisponiveis[menorIndice] SERÁ ABERTO
        openDepotsCONSTRUTIVO.push_back(depositosDisponiveis[menorIndice]);

        int capacidade = 0;
        vector<string> auxDeletarParkingSpots;
        while(capacidade <= depositosDisponiveis[menorIndice]->capacidadeDoDeposito){
            auto it = std::minmax_element(distances[menorIndice].begin(), distances[menorIndice].end());//calculando o minimo elemento do vetor
            int min_idx = std::distance(distances[menorIndice].begin(), it.first); //capturando o indice do menor elemento do vetor
            
            //SE JÁ TIVER PASSADO POR ESSE CLUSTER QUER DIZER QUE JÁ OLHOU TODOS OS CLUSTERS
            if(distances[menorIndice][min_idx] > 8887){
                capacidade = 99999; //funciona como um break
            }

            //SE COUBER ESSE PRÓXIMO CLUSTER DENTRO DA CAPACIDADE DO DEPÓSITO
            if(capacidade + parkingSpotsDisponiveis[min_idx]->clientesNoCluster.size() <= depositosDisponiveis[menorIndice]->capacidadeDoDeposito){
                capacidade += parkingSpotsDisponiveis[min_idx]->clientesNoCluster.size();
                distances[menorIndice][min_idx] = 8888; //ESSES EU TENHO Q REMOVER
                auxDeletarParkingSpots.push_back(parkingSpotsDisponiveis[min_idx]->getId());
            } else{
                distances[menorIndice][min_idx] = 9999; 
            }
        }

        //removendo os parking spots associados
        for(int v = 0; v < auxDeletarParkingSpots.size(); v++){
            for(int k = 0; k < parkingSpotsDisponiveis.size(); k++){
                if(parkingSpotsDisponiveis[k]->getId() == auxDeletarParkingSpots[v]){
                    parkingSpotsDisponiveis.erase(parkingSpotsDisponiveis.begin()+k); 
                }
            }
        }
        depositosDisponiveis.erase(depositosDisponiveis.begin()+menorIndice); //removendo o depósito associado
        distances.erase(distances.begin(), distances.end()); //limpando variavel auxiliar
        solutionCONSTRUTIVO.push_back(auxDeletarParkingSpots); //construindo string da solucao
        auxDeletarParkingSpots.erase(auxDeletarParkingSpots.begin(), auxDeletarParkingSpots.end()); //limpando variavel auxiliar
    }

    cout << endl << "TESTANDO SOLUCAO CONSTRUTIVA INICIAL: " << endl;
    cout << "Foram abertos os seguintes depositos:";
    for(int h = 0; h < openDepotsCONSTRUTIVO.size(); h++){
        cout << "  " << openDepotsCONSTRUTIVO[h]->getId();
    }

    cout << endl << "Temos a seguinte solucao:\n";
    for(int g = 0; g < solutionCONSTRUTIVO.size(); g++){
        cout << "Deposito " << g << " -> ";
        for(int d = 0; d < solutionCONSTRUTIVO[g].size(); d++){
            cout << "  " << solutionCONSTRUTIVO[g][d];
        }
        cout << endl; 
    }
}
//-------------------------------------------------------------------------//

int main()
{
    // INICIANDO VARIAVEIS PARA O CALCULO DE VALORES RANDOMICOS
    unsigned seed = time(0);   
    srand(seed);

    // LENDO E CRIANDO A INSTANCIA
    instancia = leituraDosArquivosDeEntrada(arquivoInstancias+".txt", arquivoConfiguracoes);

    // CALCULANDO OS ARRAYS DE PARES DE DISTANCIAS E IDS
    calculateSortedDistancesAndIdsArrays();

    // PROCESSO DE CLUSTERIZACAO
    clusterInsertion();

    // FUNCAO QUE CALCULA OS CENTROIDES DOS CLUSTERS E SALVA EM VARIAVEIS GLOBAIS
    calculateCentroidsCoordinates();

    // FUNCAO QUE CALCULA AS COORDENADAS DOS DEPÓSITOS E SALVA EM VARIÁVEIS GLOBAIS
    calculateDepotsCoordinates();

    //FUNÇÃO QUE CRIA LOCALIDADES PARA OS PARKING SPOTS
    createParkinkgSpotLocations();

    //FUNÇÃO QUE CRIA OS ARRAYS DOS CLUSTERS, NA INSTÂNCIA, COM O TIPO LOCALIDADE
    calculateClustersArrayLoc();

    // GERANDO ARQUIVO PARA PLOTAR A CLUSTERIZACAO
    //generateClusterPlotFile();

    // FUNCOES QUE CALCULAM CUSTOS E TEMPOS QUE CADA CLUSTER DEMANDA
    calculateClustersTimesAndCosts();

    // SOLUÇÃO ATRAVÉS DO ALGORITMO CONSTRUTIVO + ILS
    //algoritmoConstrutivoLocalizacao();

    // SOLUÇÃO ATRAVÉS DO ALGORITMO COLÔNIA DE FORMIGAS
    // INSTANCIANDO A COLÔNIA DE FORMIGAS
    Colony *colonia = new Colony(10, instancia, 10, 1);

    //iniciando contagem do tempo
    auto start = high_resolution_clock::now();

    //EXECUTANDO O PRIMEIRO ACO
    colonia->initializeFirstACO();

    //EXECUTANDO O SEGUNDO ACO
    calculateClustersDistances();
    //cout << endl << "1";

    colonia->initializeSecondACO();
    //cout << endl << "2";

    //EXECUTANDO O TERCEIRO ACO
    colonia->initializeThirdACO();
    //cout << endl << "3";
    
    //finalizando contagem do tempo
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    //CALCULANDO OS CUSTOS FINAIS DA INSTÂNCIA
    colonia->calculateFinalCosts();
    //cout << endl << "4";
    cout << endl << endl << "TEMPO TOTAL: " << duration.count() << " milissegundos";
    
    cout << endl << "\nEncerrando programa ..." << endl;
    return 0;
}