#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <string.h>
#include <stdio.h>
#include <cstdlib>
#include <ctime>
#include "Localidade.h"
#include "Instancia.h"
using namespace std;


//--------------------MÉTODOS AUXILIARES-------------------------------------//
Instancia* leituraDosArquivosDeEntrada(string arquivoEntradaInstancia, string arquivoEntradaConfig){
    //PRIMEIRO LENDO O ARQUIVO REFERENTE À INSTANCIA
    ifstream arquivo(arquivoEntradaInstancia);
    if(!arquivo.is_open()){
        cout<< "Erro ao abrir aquivo: " << arquivoEntradaInstancia << "  --> Encerrando programa." << endl;
        exit(1);
    }

    Instancia* novaInstancia = new Instancia();
    string lixo, id, tipo, x, y;

    getline(arquivo, lixo, '\n');//Lendo e descartando a primeira linha do arquivo
    while(arquivo.good()){//Lendo o restante do arquivo, cada linha vira um cliente ou depósito
        arquivo >> id;
        if(id[0] == 'D' || id[0] == 'C'){
            if(id[0] == 'D')
                novaInstancia->somaDepositos();    
            else
                novaInstancia->somaClientes();
            
            arquivo >> tipo;
            arquivo >> x;
            arquivo >> y;
            Localidade* novoLocal = new Localidade(id, tipo[0] ,stof(x), stof(y));
            novaInstancia->insereLocal(novoLocal);
        } else{
            if(id[0] == 'X' ){
                string aux1;
                getline(arquivo, lixo, '/');
                getline(arquivo, aux1, '/');
                novaInstancia->setQDEPOT(stoi(aux1));

                getline(arquivo, lixo, '/');
                getline(arquivo, aux1, '/');
                novaInstancia->setDEPOTFIXCOST(stoi(aux1));

                getline(arquivo, lixo, '/');
                getline(arquivo, aux1, '/');
                novaInstancia->setQMAX(stoi(aux1));
                break;
            }
        }
    }
    novaInstancia->atualizaTamanho();
    novaInstancia->criaMatriz();
    arquivo.close();
    cout << endl <<"A matriz de distancias foi calculada e exportada no arquivo 'Matriz.txt'." << endl;
    novaInstancia->getMatrizDistancias();

    //AGORA LENDO O ARQUIVO REFERENTE ÀS CONFIGURAÇÕES
    ifstream arquivoConfig(arquivoEntradaConfig);
    if(!arquivoConfig.is_open()){
        cout<< "Erro ao abrir aquivo: " << arquivoEntradaConfig << "  --> Encerrando programa." << endl;
        exit(1);
    }

    while(arquivoConfig.good()){//Lendo o arquivo, cada linha vira um dado da instancia
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
    //IMPRIMINDO DADOS DE TESTE NA TELA
    cout << endl << "DADOS DO ARQUIVO DE INSTANCIA:";
    cout << endl << "Numero de depositos: " << novaInstancia->getDepositos();
    cout << endl << "Numero de clientes: " << novaInstancia->getClientes();
    cout << endl << "Capacidade do(s) deposito(s): " << novaInstancia->getQDEPOT() << " parcelas";
    cout << endl << "Custo fixo do(s) deposito(s): " << novaInstancia->getDEPOTFIXCOST();
    cout << endl << "Capacidade dos caminhoes: " << novaInstancia->getQMAX() << " parcelas";
    cout << endl << endl << "DADOS DO ARQUIVO DE CONFIGURACAO:";
    cout << endl << "FDRONE: " << novaInstancia->getFDRONE();
    cout << endl << "FTRUCK: " << novaInstancia->getFTRUCK();
    cout << endl << "CD: " << novaInstancia->getCD();
    cout << endl << "CT: " << novaInstancia->getCT();
    cout << endl << "CTW: " << novaInstancia->getCTW();
    cout << endl << "H: " << novaInstancia->getDronesPorCaminhao();
    cout << endl << "VD: " << novaInstancia->getVD();
    cout << endl << "VT: " << novaInstancia->getVT();
    cout << endl << "SI: " << novaInstancia->getSI();
    cout << endl << "TUNLOAD: " << novaInstancia->getTUNLOAD();
    cout << endl << "TLOAD: " << novaInstancia->getTLOAD();
    cout << endl << "KDRONE: " << novaInstancia->getKDRONE();
    cout << endl << "KTRUCK: " << novaInstancia->getKTRUCK();
    return novaInstancia;
}
//-------------------------------------------------------------------------//

int main(){
    string arquivoInstancias = "tam4000_cli40_config2.txt";
    string arquivoConfiguracoes = "configuracoes_1.txt";
    Instancia* instancia = leituraDosArquivosDeEntrada(arquivoInstancias, arquivoConfiguracoes);
    cout << endl << "\nEncerrando programa ..." << endl;
    return 0;
}