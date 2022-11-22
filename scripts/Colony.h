#ifndef COLONY_H_
#define COLONY_H_
#include <vector>
//#include <tuple>
#include "Instancia.h"
#include "Localidade.h"

class Colony {
	public:
		//CONSTRUTOR / DESTRUTOR
		Colony(int nAnts, Instancia* instancia, int iteracoes, double initialPheromoneLevel);
		virtual ~Colony();

		//ATRIBUTOS
		int nAnts; //número de formigas
		int iteracoes; //número de iterações
		Instancia* instancia; //instância do problema
		double initialPheromoneLevel; //valor inicial de feromônio
		vector<vector<double>> pheromoneList; //matriz de feromônios
		vector<int> openDepots; //array com os indices dos depósitos abertos nessa instância (calculado após o primeiro ACO)
		
		//vector<vector<int>> depotsClustersAssociations;  AINDA NÃO TO USANDO ESSE CAMPO

		//MÉTODOS AUXILIARES
		double somatorioVetorDouble(vector<double> vec); //retorna a soma dos elementos de um array de doubles
		vector<double> evaporacao(vector<double> auxVector, double taxaDeEvaporacao); //aplica a evaporação naquela linha da matriz

		//primeiro aco
		void initializeFirstACO();
		int howManyDepots_firstColony_DepotSelection();
		int roletaPrimeiroACO(vector<double> vetorDeHeuristicas, vector<int> depositosJaSelecionados);
		
		//segundo aco
		//void initializeSecondACO(vector<vector<string>> clusters, vector<vector<float> pairsDistances);
		
		//terceiro aco
		//void initializeThirdACO();
};
#endif /* COLONY_H_ */