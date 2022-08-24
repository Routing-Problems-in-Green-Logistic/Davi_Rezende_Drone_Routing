//Classe para representar uma localidade do problema, podendo ser um depósito ou um cliente

#ifndef LOCALIDADE_H_INCLUDED
#define LOCALIDADE_H_INCLUDED
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

class Localidade{
    private:
        string Id; //id do objeto
        char tipo; //d(depósito) e c(cliente) 
        float x; //coord. x
        float y; //coord. y

    public:
        Localidade(); //construtor padrão
        Localidade(string Id, char tipo, float x, float y);//construtor personalizado
        ~Localidade();//destrutor

        //getters
        string getId();
        char gettipo();
        float getx();
        float gety();

        //setters
        void setId(string id);
        void settipo(char tipo);
        void setx(float x);
        void sety(float y);
};
#endif // LOCALIDADE_H_INCLUDED