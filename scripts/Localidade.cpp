#include "Localidade.h"

Localidade::Localidade(){}//Construtor padrão

Localidade::Localidade(string Id, char tipo, float x, float y){//Construtor personalizado
    this->Id = Id;
    this->tipo = tipo;
    this->x = x;
    this->y = y;
}

Localidade::~Localidade(){}//Destrutor

//getters
string Localidade::getId(){
    return this->Id;
}

char Localidade::gettipo(){
    return this->tipo;
}

float Localidade::getx(){
    return this->x;
}

float Localidade::gety(){
    return this->y;
}

//setters
void Localidade::setId(string ID){
    this->Id = ID;
}

void Localidade::settipo(char tipo){
    this->tipo = tipo;
}

void Localidade::setx(float x){
    this->x = x;
}

void Localidade::sety(float y){
    this->y = y;
}