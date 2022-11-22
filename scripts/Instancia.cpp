#include "Instancia.h"

Instancia::Instancia(){ //Construtor
    this->clientes = 0;
    this->depositos = 0;
}

Instancia::~Instancia(){} //Destrutor

//getters
int Instancia::getClientes(){
    return this->clientes;
}

int Instancia::getDepositos(){
    return this->depositos;
}

double Instancia::getFDRONE(){
    return this->FDRONE;
}

double Instancia::getFTRUCK(){
    return this->FTRUCK;
}

double Instancia::getCD(){
    return this->CD;
}

double Instancia::getCT(){
    return this->CT;
}

double Instancia::getCTW(){
    return this->CTW;
}

int Instancia::getDronesPorCaminhao(){
    return this->DronesPorCaminhao;
}

int Instancia::getVD(){
    return this->VD;
}

int Instancia::getVT(){
    return this->VT;
}

int Instancia::getSI(){
    return this->SI;
}

int Instancia::getTUNLOAD(){
    return this->TUNLOAD;
}

int Instancia::getTLOAD(){
    return this->TLOAD;
}

int Instancia::getQMAX(){
    return this->QMAX;
}

double Instancia::getKDRONE(){
    return this->KDRONE;
}

double Instancia::getKTRUCK(){
    return this->KTRUCK;
}

int Instancia::getQDEPOT(){
    return this->QDEPOT;
}

int Instancia::getDEPOTFIXCOST(){
    return this->DEPOTFIXCOST;
}

void Instancia::getMatrizDistancias(){
    ofstream arq("Matriz.txt", ios::out);
    for (int i = 0; i < size; i++){
        for (int j = 0; j < size; j++){
            arq<<endl;
            arq << "de "<<locaisDefinidos[i]->getId() << " para "<<locaisDefinidos[j]->getId()<<" : ";
            arq<< this->matriz_distancias[i][j];
        } 
    }
}

float Instancia::getDistancia(int origem, int destino){
    return matriz_distancias[origem][destino];
}

int Instancia::getSize(){
    return locaisDefinidos.size();
}

//setters
void Instancia::setClientes(int clientes){
    this->clientes = clientes;
}

void Instancia::setDepositos(int depositos){
    this->depositos = depositos;
}

//Métodos personalizados
void Instancia::atualizaTamanho(){
    int tamanho = this->locaisDefinidos.size();
    this->visitado.resize(tamanho);
    this->size = tamanho;
    this->matriz_distancias = (float**) malloc( sizeof(float *)*tamanho);

    for(int i = 0; i < tamanho; i++){
        this->matriz_distancias[i] = (float *)malloc(sizeof(float)*tamanho);
    }

    for(int j = 0; j < tamanho; j++){
        this->visitado[j] = false;
    }
}

void Instancia::somaClientes(){
    this->clientes++;
}

void Instancia::somaDepositos(){
    this->depositos++;
}

void Instancia::insereLocal(Localidade* Local){
    this->locaisDefinidos.push_back(Local);
}

void Instancia::limpaVisitado(){
    for (int i = 0; i < visitado.size(); i++){
        visitado[i] = -1;
    }
}

void Instancia::criaMatriz(){
    for(int i = 0; i < locaisDefinidos.size();i++){
        for(int j = 0; j < locaisDefinidos.size(); j++){
            this->matriz_distancias[i][j]= sqrt(pow((locaisDefinidos[j]->getx()-locaisDefinidos[i]->getx()),2)+pow((locaisDefinidos[j]->gety()-locaisDefinidos[i]->gety()),2));
        }
    }
}

void Instancia::criaArraysDeDepositosEclientes(){
    Localidade *local;
    for(int i = 0; i < this->locaisDefinidos.size(); i++){
        local = locaisDefinidos[i];
        if(local->gettipo() == 'd'){
            this->depositosLoc.push_back(local);
        } else{
            this->clientesLoc.push_back(local);
        }
    }
}

void Instancia::alocaVisitado(int indice, int rota){
    this->visitado[indice] = rota;
}

int Instancia::checaVisitado(int indice){
    return this->visitado[indice];
}

void Instancia::setFDRONE(double fdrone){
    this->FDRONE = fdrone;
}

void Instancia::setFTRUCK(double ftruck){
    this->FTRUCK = ftruck;
}

void Instancia::setCD(double cd){
    this->CD = cd;
}

void Instancia::setCT(double ct){
    this->CT = ct;
}

void Instancia::setCTW(double ctw){
    this->CTW = ctw;
}

void Instancia::setDronesPorCaminhao(int h){
    this->DronesPorCaminhao = h;
}

void Instancia::setVD(int vd){
    this->VD = vd;
}

void Instancia::setVT(int vt){
    this->VT = vt;
}

void Instancia::setSI(int si){
    this->SI = si;
}

void Instancia::setTUNLOAD(int tunload){
    this->TUNLOAD = tunload;
}

void Instancia::setTLOAD(int tload){
    this->TLOAD = tload;
}

void Instancia::setQMAX(int qmax){
    this->QMAX = qmax;
}

void Instancia::setKDRONE(double kdrone){
    this->KDRONE = kdrone;
}

void Instancia::setKTRUCK(double ktruck){
    this->KTRUCK = ktruck;
}

void Instancia::setQDEPOT(int qdepot){
    this->QDEPOT = qdepot;
}

void Instancia::setDEPOTFIXCOST(int depotFixCost){
    this->DEPOTFIXCOST = depotFixCost;
}