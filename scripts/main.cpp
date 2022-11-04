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
using namespace std;

//---------------------VARIÁVEIS GLOBAIS-------------------------------------//
Instancia *instancia;
vector<float> array_distancias_ordenado;
vector<string> array_pares_ids_ordenado;
vector<string> clusters_ids;
vector<vector<string>> clusters;
vector<float> centroidsX;
vector<float> centroidsY;
vector<float> clusters_times;
vector<float> clusters_drone_costs;
vector<float> clusters_truck_idle_costs;
vector<int> selected_depots;
vector<float> depotsCoordX;
vector<float> depotsCoordY;
float droneRange = 3000.00; // EM METROS
string arquivoInstancias = "inst4000_cli30_dfc140_dc27";
string arquivoConfiguracoes = "configuracoes_1.txt";
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
                string aux1;
                getline(arquivo, lixo, '/');
                getline(arquivo, aux1, '/');
                novaInstancia->setQMAX(stoi(aux1));

                getline(arquivo, lixo, '/');
                getline(arquivo, aux1, '/');
                novaInstancia->setQDEPOT(stoi(aux1));

                getline(arquivo, lixo, '/');
                getline(arquivo, aux1, '/');
                novaInstancia->setDEPOTFIXCOST(stoi(aux1));

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
    //  cout << endl << "Capacidade do(s) deposito(s): " << novaInstancia->getQDEPOT() << " parcelas";
    //  cout << endl << "Custo fixo do(s) deposito(s): " << novaInstancia->getDEPOTFIXCOST();
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
    // cout << endl << endl << "Numero de clusters: " << clusters.size() << endl;
    // for (int i = 0; i < clusters.size(); i++)
    // {
    //     for (int j = 0; j < clusters[i].size(); j++)
    //     {
    //         cout << clusters[i][j] << "   ";
    //     }
    //     cout << endl;
    // }
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

void calculateClustersDroneTravelCosts(){ 
    //FUNCAO QUE CALCULA, PARA CADA CLUSTER, O CUSTO DE DRONES
    //OS VALORES CALCULADOS SAO ARMAZENADOS NA VARIAVEL GLOBAL CLUSTERS_DRONE_COSTS
    //AS POSICOES NESSA VARIAVEL GLOBAL EQUIVALEM ÀS POSICOES NA VARIAVEL GLOBAL CLUSTERS

    //VARIAVEIS AUXILIARES
    int locationIndex;
    vector<float> yCoordinates;
    vector<float> xCoordinates;
    float costAux = 0;

    for (int i = 0; i < clusters.size(); i++)
    {
        for (int j = 0; j < clusters[i].size(); j++)
        {
            //captura as coordenadas dos clientes do cluster
            locationIndex = findLocationById(clusters[i][j]);
            xCoordinates.push_back(instancia->locaisDefinidos[locationIndex]->getx());
            yCoordinates.push_back(instancia->locaisDefinidos[locationIndex]->gety());
        }
        
        //calculando custos de deslocamento dos drones
        for(int k = 0; k < xCoordinates.size(); k++){
            costAux += float(2 * calculateDistanceBetweenPoints(xCoordinates[k], yCoordinates[k], centroidsX[i], centroidsY[i]));
        }
        costAux = costAux * instancia->getCD();

        //LEMBRANDO QUE AQUI, AINDA NAO FORAM CONSIDERADOS OS CUSTOS DE USO DIARIO DOS DRONES (FDRONE)
        
        //limpando variaveis auxiliares
        costAux = 0.0;
        xCoordinates.erase(xCoordinates.begin(), xCoordinates.end());
        yCoordinates.erase(yCoordinates.begin(), yCoordinates.end());
    }

    //IMPRIMINDO DRONE COSTS DE CADA CLUSTER
    cout << endl << "CUSTOS DE DRONE DE CADA CLUSTER" << endl;
    for(int i = 0; i < clusters_drone_costs.size(); i++){
        cout << "Cluster " << i+1 << ": " << clusters_drone_costs[i] << endl;
    }
}

void calculateClustersTimesAndTruckCosts(){
    //FUNCAO QUE CALCULA, PARA CADA CLUSTER, O TEMPO DE ATENDIMENTO E O CUSTO DO CAMINHÃO
    //OS VALORES CALCULADOS SAO ARMAZENADOS NAS VARIAVEIS GLOBAIS CLUSTERS_TRUCK_COSTS E CLUSTERS_TIMES
    //AS POSICOES NESSAS VARIAVEIS GLOBAIS EQUIVALEM ÀS POSICOES NA VARIAVEL GLOBAL CLUSTERS

    //VARIAVEIS AUXILIARES
    int locationIndex;
    vector<float> yCoordinates;
    vector<float> xCoordinates;
    vector<float> distances;
    vector<float> times;
    vector<float> droneTimesAux;

    for (int i = 0; i < clusters.size(); i++)
    {
        for (int j = 0; j < clusters[i].size(); j++)
        {
            //captura as coordenadas dos clientes do cluster
            locationIndex = findLocationById(clusters[i][j]);
            xCoordinates.push_back(instancia->locaisDefinidos[locationIndex]->getx());
            yCoordinates.push_back(instancia->locaisDefinidos[locationIndex]->gety());
        }

        for(int k = 0; k < xCoordinates.size(); k++){
            distances.push_back(2.0 * calculateDistanceBetweenPoints(xCoordinates[k], yCoordinates[k], centroidsX[i], centroidsY[i]));
        }

        for(int h = 0; h < distances.size(); h++){
            times.push_back((distances[h] / float(instancia->getVD())) + instancia->getTUNLOAD() + instancia->getTLOAD());
            //fiz um pouco diferente do autor aqui
        }

        //agora ja temos o tempo que cada cliente demanda, logo podemos fazer uma distribuicao de tarefas entre os drones
        //sao sempre 3 drones por caminhao. Comecamos pelos maiores tempos.

        //CASO TENHAMOS 1 CLIENTE NO CLUSTER
        if(times.size() == 1){
            clusters_times.push_back(times[0]);
        } else{
            std::sort(times.begin(), times.end());
            //CASO TENHAMOS 2 CLIENTES NO CLUSTER
            if(times.size() == 2){
                clusters_times.push_back(times[1]); 
            } else{
                //CASO TENHAMOS 3 OU MAIS CLIENTES NO CLUSTER
                droneTimesAux.push_back(times[times.size()-1]);
                droneTimesAux.push_back(times[times.size()-2]);
                droneTimesAux.push_back(times[times.size()-3]);
                int m = times.size()-4;
                while(m != 0){
                    std::sort(droneTimesAux.begin(), droneTimesAux.end());   
                    droneTimesAux[0] += times[m];
                    m--;
                }
                std::sort(droneTimesAux.begin(), droneTimesAux.end());
                clusters_times.push_back(droneTimesAux[droneTimesAux.size()-1]);
            }
        }

        //ADICIONANDO O TRUCK IDLE WAITING COST PARA ESSE CLUSTER
        clusters_truck_idle_costs.push_back(clusters_times[clusters_times.size()-1] * instancia->getCTW());

        //limpando variaveis auxiliares
        distances.erase(distances.begin(), distances.end());
        xCoordinates.erase(xCoordinates.begin(), xCoordinates.end());
        yCoordinates.erase(yCoordinates.begin(), yCoordinates.end());
        droneTimesAux.erase(droneTimesAux.begin(), droneTimesAux.end());
        times.erase(times.begin(), times.end());
    }

    //IMPRIMINDO TEMPOS DE CADA CLUSTER
    cout << endl << "TEMPOS DE CADA CLUSTER" << endl;
    for(int i = 0; i < clusters_times.size(); i++){
        cout << "Cluster " << i+1 << ": " << clusters_times[i] << endl;
    }

    //IMPRIMINDO CUSTOS DE TRUCK IDLE WAITING COST DE CADA CLUSTER
    cout << endl << "CUSTOS DE TRUCK IDLE WAITING DE CADA CLUSTER" << endl;
    for(int i = 0; i < clusters_truck_idle_costs.size(); i++){
        cout << "Cluster " << i+1 << ": " << clusters_truck_idle_costs[i] << endl;
    }
}

int howManyDepots_firstColony_DepotSelection(){
    //FUNÇÃO QUE CALCULA QUANTOS DEPÓSITOS SERÃO ABERTOS E SALVA O RESULTADO EM UMA VARIAVEL GLOBAL
    int n = instancia->getClientes(); //numero de clientes da instancia, cada um com demanda qi = 1
    int w = instancia->getQDEPOT(); //capacidade dos depositos
    int u = rand() % 3 + 1; //randomico entre 1 e r, sendo que r = 3
    int m = instancia->getDepositos(); //número de depósitos
    int aux = (n / w) + u;

    //COMO EU ESTAVA ENCONTRANDO VALORES MAIORES DO QUE A QUANTIDADE DE DEPÓSITOS, EU FIZ ESSE AJUSTE
    //MOSTRAR PRA LUCIANA!!
    if(aux > m)
        return m;
    return aux;
}

void firstAntColony_DepotSelection(){
    int h = 10; //número de formigas
    int t = 1; //valor inicial de feromônio
    int m = instancia->getDepositos(); //número de depósitos
    int w = instancia->getDEPOTFIXCOST(); //custo fixo para abrir um depósito
    int f = instancia->getQDEPOT(); //capacidade dos depósitos
    vector<int> selectedDepotsAuxVector;
    vector<float> auxVectorFloat;

    //matriz de feromônios: todos os depósitos começam com o mesmo valor inicial
    //cada posição é um vetor com uma posição para cada depósito. Temos 10 vetores, um para cada formiga
    vector<vector<float>> depotsPheromoneLevels;
    for(int i = 0; i < m; i++){
        auxVectorFloat.push_back(t);
    }
    depotsPheromoneLevels.push_back(auxVectorFloat);
    auxVectorFloat.erase(auxVectorFloat.begin(), auxVectorFloat.end()); //limpando variavel auxiliar

    //iterando cada formiga
    for(int i = 1; i <= h; i++){
        int p = howManyDepots_firstColony_DepotSelection(); //número de depósitos abertos para essa formiga

        //PREENCHENDO A LINHA DA MATRIZ PARA ESSA ITERAÇÃO
        depotsPheromoneLevels.push_back(depotsPheromoneLevels[i-1]);
        
        float q = static_cast <float> (rand()) / static_cast <float> (RAND_MAX); //aleatório entre 0 e 1 para definir se é guloso ou ACO
        if(q <= 0.5){ //q <= q0, onde q0 = 0.5 -> variável definida pelo autor (exploration factor)
            //GULOSO

            //calculando a 'heuristica' de cada depósito
            for(int j = 0; j < m; j++){
                auxVectorFloat.push_back(depotsPheromoneLevels[i][j] * (w/f));
            }

            //selecionando os 'p' melhores valores (depósitos) e atualizando a matriz de feromônio para esses depósitos
            selectedDepotsAuxVector.erase(selectedDepotsAuxVector.begin(), selectedDepotsAuxVector.end()); //limpando variavel auxiliar
            for(int j = 0; j < p; j++){
                auto it = std::minmax_element(auxVectorFloat.begin(), auxVectorFloat.end());//calculando o máximo elemento do vetor
                int max_idx = std::distance(auxVectorFloat.begin(), it.second); //capturando o indice do maior elemento do vetor
                selectedDepotsAuxVector.push_back(max_idx); //array que guarda os depósitos selecionados por essa formiga
                auxVectorFloat[max_idx] = -999; //dessa forma, esse depósito não vai ser selecionado na próxima iteração de 'j'
                depotsPheromoneLevels[i][max_idx] = 0.9 * depotsPheromoneLevels[i-1][max_idx] + 2; //FUNÇÃO DE ATUALIZAÇÃO DO NIVEL DE FEROMONIO
                //ACHO Q ESSA FUNÇÃO NUNCA ALTERA O VALOR DO FEROMONIO!!! POR ISSO VOU COLOCAR UM +2
            }
            auxVectorFloat.erase(auxVectorFloat.begin(), auxVectorFloat.end()); //limpando variavel auxiliar            
        } else{
            //ACO ->POR ENQUANTO ESTA IGUAL AO GULOSO PORQUE A HEURISTICA DE CADA DEPOSITO SE MANTEVE A MESMA (SO ADICIONOU UM MESMO DENOMINADOR EM TODOS)
            //LOGO, OS RESULTADOS ENCONTRADOS CONTINUARIAM DEPENDENDO APENAS DO NUMERADOR, QUE É IGUAL AO DO GULOSO
            //TIRAR ESSA DÚVIDA COM A LUCIANA

            // //calculando a 'heuristica'(probabilidade) de cada depósito
            // for(int j = 0; j < m; j++){
            //     auxVectorFloat.push_back((depotsPheromoneLevels[i][j] * (w/f)) / );
            // }

            // //selecionando os 'p' melhores valores (depósitos) e atualizando a matriz de feromônio para esses depósitos
            // selectedDepotsAuxVector.erase(selectedDepotsAuxVector.begin(), selectedDepotsAuxVector.end()); //limpando variavel auxiliar
            // for(int j = 0; j < p; j++){
            //     auto it = std::minmax_element(auxVectorFloat.begin(), auxVectorFloat.end());//calculando o máximo elemento do vetor
            //     int max_idx = std::distance(auxVectorFloat.begin(), it.second); //capturando o indice do maior elemento do vetor
            //     selectedDepotsAuxVector.push_back(max_idx); //array que guarda os depósitos selecionados por essa formiga
            //     auxVectorFloat[max_idx] = -999; //dessa forma, esse depósito não vai ser selecionado na próxima iteração de 'j'
            //     depotsPheromoneLevels[i][max_idx] = 0.9 * depotsPheromoneLevels[i-1][max_idx] + 2; //FUNÇÃO DE ATUALIZAÇÃO DO NIVEL DE FEROMONIO
            //     //ACHO Q ESSA FUNÇÃO NUNCA ALTERA O VALOR DO FEROMONIO!!! POR ISSO VOU COLOCAR UM +2
            // }
            // auxVectorFloat.erase(auxVectorFloat.begin(), auxVectorFloat.end()); //limpando variavel auxiliar        
        }
    }

    //A configuração final do array 'selectedDepotsAuxVector' guarda quais depósitos serão abertos para essa instância.
    //O resultado desse primeiro ACO é armazenado na variável global 'selected_depots'
    selected_depots = selectedDepotsAuxVector;

    // cout << "\nPara essa instancia serao abertos os depositos: ";
    // for(int i = 0; i < selected_depots.size(); i++){
    //     cout << selected_depots[i] << "  ";
    // }

    // cout << endl << "Imprimindo matriz final de feromonios:" << endl;
    // for(int i = 0; i < h+1; i++){
    //     cout << "\n Iteracao " << i << ": ";
    //     for(int j = 0; j < m; j++){
    //         cout << "D" << j << ": " << depotsPheromoneLevels[i][j] << " / ";
    //     }
    // }
}

void secondAntColony_DepotClusterAssigning(){
    //em um primeiro momemento imaginei essa função salvando em uma variavel global da seguinte estrutura: um vector com d posições, onde d
    //é o número de depósitos a serem abertos naquele dia. Cada uma dessas d posições é um vector de inteiros, que guarda os index dos clusters que 
    //esse depósito atende. Esses index são relativos à variável global 'clusters'

    int h = 10; //número de formigas
    int t = 1; //valor inicial de feromônio
    
    //matriz de feromônios: todos os pares parkingSpot/depósito começam com o mesmo valor inicial
    //cada posição é um vetor com uma posição para cada par parkingSpot/depósito. Temos 10 vetores, um para cada formiga
    //só é necessário considerar os depósitos que serão abertos (resultado do primeiro ACO)
    vector<vector<float>> pairsPheromoneLevels;
    vector<float> auxVectorFloat;
    for(int i = 0; i < selected_depots.size() * clusters.size(); i++){
        auxVectorFloat.push_back(t);
    }
    pairsPheromoneLevels.push_back(auxVectorFloat);
    auxVectorFloat.erase(auxVectorFloat.begin(), auxVectorFloat.end()); //limpando variavel auxiliar

    //variavel auxiliar que guarda as distâncias entre os clusters e os depósitos selecionados
    vector<vector<float>> pairsDistances;
    for(int i = 0; i < selected_depots.size(); i++){
        for(int j = 0; j < clusters.size(); j++){
            auxVectorFloat.push_back(calculateDistanceBetweenPoints(centroidsX[j], centroidsY[j], depotsCoordX[i], depotsCoordY[i]));
        }
        pairsDistances.push_back(auxVectorFloat);
        auxVectorFloat.erase(auxVectorFloat.begin(), auxVectorFloat.end()); //limpando variavel auxiliar
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

    // GERANDO ARQUIVO PARA PLOTAR A CLUSTERIZACAO
    //generateClusterPlotFile();

    // FUNCOES QUE CALCULAMOS CUSTOS RELACIONADOS A DRONES E CAMINHOES NOS CLUSTERS, E O TEMPO QUE CADA CLUSTER DEMANDA
    // OS VALORES SAO ARMAZENADOS EM VARIAVEIS GLOBAIS
    //calculateClustersDroneTravelCosts();
    //calculateClustersTimesAndTruckCosts();

    // EXECUTANDO OS TRÊS ALGORITMOS COLÔNIA DE FORMIGAS
    //firstAntColony_DepotSelection();



    cout << endl << "\nEncerrando programa ..." << endl;
    return 0;
}