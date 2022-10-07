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

//---------------------VARIAVEIS GLOBAIS-------------------------------------//
Instancia *instancia;
vector<float> array_distancias_ordenado;
vector<float> array_distancias_ordenado_backup;
vector<string> array_pares_ids_ordenado;
vector<string> array_pares_ids_ordenado_backup;
vector<string> clusters_ids;
vector<vector<string>> clusters;
float droneRange = 1000.00; // EM METROS -> VALOR PROVISORIO
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
    cout << endl << endl << "ARRAY DE IDS DEPOIS DA ORDENACAO: " << endl;
    for(int i = 0; i < array_pares_ids_ordenado.size(); i++){
        cout << array_pares_ids_ordenado[i] << "  /  ";
    }
}

void firstClusterInsertion()
{
    vector<float> yCoordinates;
    vector<float> xCoordinates;
    string pointA, pointB;
    int specialCharIndex, locationIndex;
    float centroidX, centroidY;
    vector<string> aux_vector;

    // A PRIMEIRA POSICAO DEVE SER ANALISADA ANTES DO FOR
    specialCharIndex = array_pares_ids_ordenado[0].find("-");
    pointA = array_pares_ids_ordenado[0].substr(0, specialCharIndex);                               // ID DO PONTO A
    pointB = array_pares_ids_ordenado[0].substr(specialCharIndex + 1, array_pares_ids_ordenado.size()); // ID DO PONTO B
    locationIndex = findLocationById(pointA);
    xCoordinates.push_back(instancia->locaisDefinidos[locationIndex]->getx());
    yCoordinates.push_back(instancia->locaisDefinidos[locationIndex]->gety());
    // cout << endl << pointA << " - coordX: " << instancia->locaisDefinidos[locationIndex]->getx() << " - coordY: " << instancia->locaisDefinidos[locationIndex]->gety();
    locationIndex = findLocationById(pointB);
    xCoordinates.push_back(instancia->locaisDefinidos[locationIndex]->getx());
    yCoordinates.push_back(instancia->locaisDefinidos[locationIndex]->gety());
    // cout << endl << pointB << " - coordX: " << instancia->locaisDefinidos[locationIndex]->getx() << " - coordY: " << instancia->locaisDefinidos[locationIndex]->gety();
    // cout << endl << "x do centroide: "<< calculateCentroidCoordinate(xCoordinates);
    // cout << endl << "y do centroide: "<< calculateCentroidCoordinate(yCoordinates);
    centroidX = calculateCentroidCoordinate(xCoordinates);
    centroidY = calculateCentroidCoordinate(yCoordinates);

    if ((droneRange >= 2 * calculateDistanceBetweenPoints(centroidX, centroidY, xCoordinates[0], yCoordinates[0])) &&
        (droneRange >= 2 * calculateDistanceBetweenPoints(centroidX, centroidY, xCoordinates[1], yCoordinates[1])))
    {
        aux_vector.push_back(pointA);
        aux_vector.push_back(pointB);
        clusters.push_back(aux_vector);
        aux_vector.erase(aux_vector.begin()); // limpando a variavel auxiliar
        aux_vector.erase(aux_vector.begin()); // limpando a variavel auxiliar
    }
    else
    {
        aux_vector.push_back(pointA);
        clusters.push_back(aux_vector);
        aux_vector.erase(aux_vector.begin()); // limpando a variavel auxiliar
        aux_vector.push_back(pointB);
        clusters.push_back(aux_vector);
        aux_vector.erase(aux_vector.begin()); // limpando a variavel auxiliar
    }

    array_pares_ids_ordenado.erase(array_pares_ids_ordenado.begin()); // deletando o primeiro par do vetor
}

void finishClusterInsertion(){
    vector<float> yCoordinates;
    vector<float> xCoordinates;
    string pointA, pointB;
    int specialCharIndex, locationIndex;
    float centroidX, centroidY;
    bool aux = false;
    vector<string> aux_vector;
    bool foundA = false, foundB = false;
    int indexClusterA, indexClusterB;

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
                    if ((droneRange <= 2 * calculateDistanceBetweenPoints(centroidX, centroidY, xCoordinates[k], yCoordinates[k])))
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
                // somente o pointA esta em um cluster.Entao tentamos inserir o pointB nesse cluster e ver se continua viavel.
                // se continuar, ok. Se nao, o pointB vira um cluster individual;

                if (clusters[indexClusterA].size() >= instancia->getQMAX())
                {
                    // nao cabe mais ninguem no cluster, eh criado um cluster individual
                    aux_vector.push_back(pointB);
                    clusters.push_back(aux_vector);
                    aux_vector.erase(aux_vector.begin()); // limpando a variavel auxiliar
                    break;
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
                        if ((droneRange <= 2 * calculateDistanceBetweenPoints(centroidX, centroidY, xCoordinates[k], yCoordinates[k])))
                        {
                            aux = true;
                        }
                    }
                    if (aux)
                    {
                        // algum ponto nao alcanca o centroide, logo nao eh viavel
                        aux_vector.push_back(pointB);
                        clusters.push_back(aux_vector);
                        aux_vector.erase(aux_vector.begin()); // limpando a variavel auxiliar
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
                    // somente o pointB esta em um cluster. Entao tentamos inserir o pointB nesse cluster e ver se continua viavel.
                    // se continuar, ok. Se nao, o pointA vira um cluster individual;

                    if (clusters[indexClusterB].size() >= instancia->getQMAX())
                    {
                        // nao cabe mais ninguem no cluster, eh criado um cluster individual
                        aux_vector.push_back(pointA);
                        clusters.push_back(aux_vector);
                        aux_vector.erase(aux_vector.begin()); // limpando a variavel auxiliar
                        break;
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
                            if ((droneRange <= 2 * calculateDistanceBetweenPoints(centroidX, centroidY, xCoordinates[k], yCoordinates[k])))
                            {
                                aux = true;
                            }
                        }
                        if (aux)
                        {
                            // algum ponto nao alcanca o centroide, logo nao eh viavel
                            aux_vector.push_back(pointA);
                            clusters.push_back(aux_vector);
                            aux_vector.erase(aux_vector.begin()); // limpando a variavel auxiliar
                            aux = false;
                        }
                        else
                        {
                            // o cluster original adicionado do pointA eh viavel
                            clusters[indexClusterA].push_back(pointA);
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
                        locationIndex = findLocationById(pointA);
                        xCoordinates.erase(xCoordinates.begin(), xCoordinates.end());
                        yCoordinates.erase(yCoordinates.begin(), yCoordinates.end());
                        xCoordinates.push_back(instancia->locaisDefinidos[locationIndex]->getx());
                        yCoordinates.push_back(instancia->locaisDefinidos[locationIndex]->gety());
                        locationIndex = findLocationById(pointB);
                        xCoordinates.push_back(instancia->locaisDefinidos[locationIndex]->getx());
                        yCoordinates.push_back(instancia->locaisDefinidos[locationIndex]->gety());
                        // calcula o centroide do cluster
                        centroidX = calculateCentroidCoordinate(xCoordinates);
                        centroidY = calculateCentroidCoordinate(yCoordinates);
                        // verifica se o cluster eh viavel
                        if ((droneRange >= 2 * calculateDistanceBetweenPoints(centroidX, centroidY, xCoordinates[0], yCoordinates[0])) &&
                            (droneRange >= 2 * calculateDistanceBetweenPoints(centroidX, centroidY, xCoordinates[1], yCoordinates[1])))
                        {
                            // cria um cluster com os dois pontos
                            aux_vector.push_back(pointA);
                            aux_vector.push_back(pointB);
                            clusters.push_back(aux_vector);
                            aux_vector.erase(aux_vector.begin(), aux_vector.end()); // limpando a variavel auxiliar
                        }
                        else
                        {
                            // cria um cluster para cada ponto
                            aux_vector.push_back(pointA);
                            clusters.push_back(aux_vector);
                            aux_vector.erase(aux_vector.begin()); // limpando a variavel auxiliar
                            aux_vector.push_back(pointB);
                            clusters.push_back(aux_vector);
                            aux_vector.erase(aux_vector.begin()); // limpando a variavel auxiliar
                        }
                    }
                }
            }
        }

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
//-------------------------------------------------------------------------//

int main()
{
    // DEFININDO OS ARQUIVOS DE ENTRADA E CRIANDO A INSTANCIA
    string arquivoInstancias = "inst4000_cli30_dfc140_dc27.txt";
    string arquivoConfiguracoes = "configuracoes_1.txt";
    instancia = leituraDosArquivosDeEntrada(arquivoInstancias, arquivoConfiguracoes);

    // CALCULANDO OS ARRAYS DE PARES DE DISTANCIAS E IDS
    // SALVANDO O CONTEUDO DOS ARRAYS EM UMA VARIAVEL DE BACKUP POIS VOU MODIFICAR OS ARRAYS DURANTE A CLUSTERIZACAO
    calculateSortedDistancesAndIdsArrays();
    array_distancias_ordenado_backup = array_distancias_ordenado;
    array_pares_ids_ordenado_backup = array_pares_ids_ordenado;

    // INICIANDO O PROCESSO DE CLUSTERIZACAO
    firstClusterInsertion(); // CALCULA O PRIMEIRO CLUSTER
    finishClusterInsertion(); //PERCORRE TODA A LISTA E TERMINA A CLUSTERIZACAO

    cout << endl << "\nEncerrando programa ..." << endl;
    return 0;
}