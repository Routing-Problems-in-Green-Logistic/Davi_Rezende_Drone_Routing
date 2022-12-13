#ifndef COLONY_H_
#define COLONY_H_
#include <vector>
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
		vector<vector<double>> pheromoneList; //matriz de feromônios do primeiro aco
		vector<vector<double>> secondACOpheromoneList; //matriz de feromônios do segundo aco
		vector<vector<double>> thirdACOpheromoneList; //matriz de feromônios do terceiro aco
		vector<int> openDepots; //array com os indices dos depósitos abertos nessa instância (calculado após o primeiro ACO)
		vector<Localidade*> depositosAbertosLoc;
		vector<vector<Localidade*>> solucaoSegundoACO;
		vector<vector<Localidade*>> solucaoTerceiroACO; //cada posição é um vector que indica a rota para esse depósito. Os depósitos estão na mesma ordem da variável depositosAbertosLoc
		vector<double> finalCosts; //guarda os custos finais para cada depósito (considerando todos os clusters associados a ele)

		//MÉTODOS AUXILIARES
		double somatorioVetorDouble(vector<double> vec); //retorna a soma dos elementos de um array de doubles
		vector<double> evaporacao(vector<double> auxVector, double taxaDeEvaporacao); //aplica a evaporação naquela linha da matriz
		double calculateDistanceBetweenPoints(float xa, float ya, float xb, float yb); //calcula a distância entre dois pontos, dadas as coordenadas
		void calculateFinalCosts(); //calcula os custos finais para cada instância

		//primeiro aco
		void initializeFirstACO();
		int howManyDepots_firstColony_DepotSelection();
		int roletaPrimeiroACO(vector<double> vetorDeHeuristicas, vector<int> depositosJaSelecionados);
		
		//segundo aco
		double encontraMenorDistanciaEntreClusters(Localidade* clusterEspecifico, vector<Localidade*> conjuntoDeClusters);
		void initializeSecondACO();

		//terceiro aco
		void initializeThirdACO();

		//ROLETA
		int roletaACO(vector<double> vetorDeHeuristicas);
};
#endif /* COLONY_H_ */