#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <cstdlib>
#include <ctime>
using namespace std;

void generateInstanceFile(int tam, int nclientes, int numberOfDepots, int truckCapacity, int depotMinCap, int depotMaxCap, int depotFixCostMin, int depotFixCostMax, string fileName){
    
    int dif = depotFixCostMax - depotFixCostMin;
    int depotFixCost = depotFixCostMin + (rand() % (dif + 1));
    dif = depotMaxCap - depotMinCap;
    int depotCapacity = depotMinCap + (rand() % (dif + 1));

    //CRIANDO ARQUIVO
    ofstream arq(fileName, ios::out);
    arq << "StringID  Type       x         y"; //CABECALHOS DA INSTANCIA
    arq << endl;

    //VARIAVEIS PARA A CRIACAO DOS PONTOS
    string id;
    float coordX, coordY;
    
    //CRIANDO DEPOSITOS
    int i = 0;
    while(i < numberOfDepots){
        id = 'D' + to_string(i);
        coordX = rand() % tam;
        coordY = rand() % tam;

        //A SEGUINTE LOGICA DE IF/ELSE EH APENAS PARA FORMATACAO DO ARQUIVO
        string spaceC;
        if(coordX < 10)
            spaceC = "         ";
        else{
            if(coordX < 100)
                spaceC = "        ";
            else{
                if(coordX < 1000)
                    spaceC = "       ";
                else{
                    if(coordX < 10000)
                        spaceC = "      ";
                    else
                        spaceC = "     ";
                }
            }
        }

        arq << id << "         d         " << coordX << spaceC << coordY;
        arq << endl;
        i++;
    }

    //CRIANDO CLIENTES
    i = 0;
    while(i < nclientes){
        id = 'C' + to_string(i);
        coordX = rand() % tam;
        coordY = rand() % tam;

        //A SEGUINTE LOGICA DE IF/ELSE EH APENAS PARA FORMATACAO DO ARQUIVO
        string spaceA, spaceB;
        if(i < 10)
            spaceA = "         ";
        else {
            if(i < 100)
                spaceA = "        ";
            else
                spaceA = "       ";
        }
        if(coordX < 10)
            spaceB = "         ";
        else{
            if(coordX < 100)
                spaceB = "        ";
            else{
                if(coordX < 1000)
                    spaceB = "       ";
                else{
                    if(coordX < 10000)
                        spaceB = "      ";
                    else
                        spaceB = "     ";
                }
            }
        }

        arq << id << spaceA << "c         " << coordX << spaceB << coordY;
        arq << endl;
        i++;
    }

    //RESTANTE DO ARQUIVO
    arq << endl;
    arq << "X DEPOTCAPACITY /" << depotCapacity << "/";
    arq << endl;
    arq << "DEPOTFIXCOST /" << depotFixCost << "/";
    arq << endl;
    arq << "TRUCKCAPACITY /" << truckCapacity << "/";
}

int main(){

    /*
    SÃO 4 TAMANHOS DE INSTANCIAS
    PARA CADA TAMANHO, SÃO 5 CONFIGURAÇÕES DE INSTÂNCIAS (30, 40, 50, 200, 300 CLIENTES)

    PARA CADA CONFIGURAÇÃO VOU CRIAR 4 INSTÂNCIAS, POR CONTA DOS VALORES ALEATÓRIOS DE DEPOT CAPACITY E DEPOT FIX COST
    CADA INSTÂNCIA TERÁ UM VALOR FIXO DE DEPOT CAPACITY E DEPOT FIX COST
    AS MARGENS INFERIORES E SUPERIORES PARA O DEPOT CAPACITY E DEPOT FIX COST SÃO CONHECIDAS: 
        -> DEPOT CAPACITY: 20/40 PARA 30,40,50 CLIENTES, 50/100 PARA 200 CLIENTES, 60/100 PARA 300 CLIENTES
        -> DEPOT FIX COST: 100/200 (3 PRIMEIRAS), 300/500 (2 PRÓXIMAS), 400/500 (3 PRÓXIMAS), 600/800 (DUAS PRÓXIMAS), 600/700 (TRêS PRÓXIMAS),
        800/1000 (DUAS ÚLTIMAS)
    
    CAPACIDADES DOS CAMINHÕES:
        ->30 CLIENTES --> 3
        ->40 CLIENTES --> 3
        ->50 CLIENTES --> 4
        ->200 CLIENTES --> 5
        ->300 CLIENTES --> 5

    PORTANTO SERÃO 60 INSTÂNCIAS
    */

    //CALCULANDO VALORES RANDOMICOS DA INSTANCIA
    unsigned seed = time(0);   
    srand(seed);

    //VARIÁVEIS COM OS PARÂMETROS DE CRIAÇÃO DAS INSTÂNCIAS
    int instanceSizes[5] = {4000, 7680, 11000};//4x4, 7.68x7.68, 11x11 Km2
    int numberOfClients[5] = {30, 40, 50, 200, 300};
    int numberOfDepots[5] = {3, 3, 4, 5, 5};
    int truckCapacities[5] = {10, 10, 10, 25, 25};
    int depotCapacitiesMIN[5] = {20, 20, 20, 50, 60};
    int depotCapacitiesMAX[5] = {40, 40, 40, 100, 200};
    int depotFixCostMin[15] = {100, 100, 100, 300, 300, 400, 400, 400, 600, 600, 600, 600, 600, 800, 800};
    int depotFixCostMax[15] = {200, 200, 200, 500, 500, 500, 500, 500, 800, 800, 700, 700, 700, 1000, 1000};

    int i = 0, j = 0, k = 0;
    string fileName;
    while(i < 3){
        while(j < 5){
            for(int z = 0; z < 4; z++){
                fileName = "inst" + to_string(instanceSizes[i]) +  "_cli" + to_string(numberOfClients[j]) + "_sce" + to_string(z) + ".txt"; 
                generateInstanceFile(instanceSizes[i], numberOfClients[j], numberOfDepots[j], truckCapacities[j], depotCapacitiesMIN[j], depotCapacitiesMAX[j], depotFixCostMin[k], depotFixCostMax[k], fileName);
            }
            j++;
            k++;
        }
        j = 0;
        i++;
    }

    cout << endl;
    cout << "\nInstancias criadas, encerrando programa ..." << endl;
    return 0;
}