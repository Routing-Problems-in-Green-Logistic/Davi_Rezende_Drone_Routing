//Classe para representar uma instância do problema
#ifndef INSTANCIA_H_INCLUDED
#define INSTANCIA_H_INCLUDED
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <list>
#include "Localidade.h"
using namespace std;

class Instancia{
    public:
        int depositos; //Número de depósitos
        int clientes; //Número de clientes
        int size; //Tamanho da instância (número de locais)
        double FDRONE, FTRUCK; //Custos fixos para utilização de um drone ou caminhão
        double CD, CT; //Custo, por metro percorido, de drone e de caminhão
        double CTW; //Custo de espera do caminhão no parking spot, por segundo
        int DronesPorCaminhao; //Número de drones por caminhão
        int VD, VT; //Velocidade do drone e do caminhão, em m/s
        int SI; //Tempo para o caminhão estacionar em um parking spot, em segundos
        int TUNLOAD, TLOAD; //Tempo para o drone descarregar uma encomenda, e carregar uma encomenda, em segundos
        int QMAX; //Capacidade dos caminhões, em parcelas
        double KDRONE, KTRUCK; //Circuity fator of drone and truck travel;
        float** matriz_distancias; //Matriz de distâncias
        vector<int> visitado;
        vector<Localidade*> locaisDefinidos;
        vector<Localidade*> depositosLoc;
        vector<Localidade*> clientesLoc;
        vector<Localidade*> parkingSpots;
        vector<vector<Localidade*>> clusters;
        vector<vector<double>> distanciasEntreParkingSpots; //guarda a distância entre todos os parkingSpots
        //cada linha é um parkingSpot, e cada coluna é sua distância para outro parkingSpot.
        
        Instancia();//Construtor padrão
        ~Instancia();//Destrutor

        //getters
        void getMatrizDistancias(); //salva a matriz de distâncias em um arquivo .txt
        float getDistancia(int origem, int destino); //calcula a distância entre dois locais, dado o índice de cada um
        int getClientes();
        int getDepositos();
        int getSize();
        double getFDRONE();
        double getFTRUCK();
        double getCD();
        double getCT();
        double getCTW();
        int getDronesPorCaminhao();
        int getVD();
        int getVT();
        int getSI();
        int getTUNLOAD();
        int getTLOAD();
        int getQMAX();
        double getKDRONE();
        double getKTRUCK();

        //setters
        void setClientes(int tamanho);
        void setDepositos(int tamanho);
        void setFDRONE(double fdrone);
        void setFTRUCK(double ftruck);
        void setCD(double cd);
        void setCT(double ct);
        void setCTW(double ctw);
        void setDronesPorCaminhao(int h);
        void setVD(int vd);
        void setVT(int vt);
        void setSI(int si);
        void setTUNLOAD(int tunload);
        void setTLOAD(int tload);
        void setQMAX(int qmax);
        void setKDRONE(double kdrone);
        void setKTRUCK(double ktruck);

        //Métodos personalizados
        void insereLocal(Localidade* local);
        void somaClientes();
        void somaDepositos();
        void alocaVisitado(int indice, int rota);
        void atualizaTamanho();
        int checaVisitado(int indice);
        void criaMatriz(); //calcula a matriz de distâncias
        void limpaVisitado();
        void criaArraysDeDepositosEclientes();
        void calculaCustosEcapacidadesDosDepositosEcaminhoes(int tamanhoDaInstancia);
};
#endif // INSTANCIA_H_INCLUDED