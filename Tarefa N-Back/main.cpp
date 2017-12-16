#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <sys/resource.h>
#include <unistd.h>

#define NEURONS 5 //Quantidade de neuronios

#define writeBestGene 1 //Save the best genotype of the simulation in a file
#define writePopulation 1 //Write the last population of a simulation in a file

//Variáveis carregadas do arquivo config.txt
bool loadPopulationFromFile; //Load all individuals genotype from file
bool logDynamic; //Save the dynamic of a individual (usuallly the best)
bool loadBestGeneFromFile; //Load only the best  from the last simulation
int  trials;        //Trials for each individual
int  end;
int  start;		//start value for logGA (column "iteration")

//Protótipos
float evaluate (int n );

#include "izhikevich.h"
#include "extras.h" //void init_pop(); void write_pop(int best); void read_conf_file();

Izhikevich_network  *Iz; //Variavel global.

#include "genetico.h"

int limiar_pulsos=0;
#include "evaluate.h" //avalia um individuo da populacao

int main(int argc, char *argv[]){

	if(argc!=2){
		printf("main <limiar de pulsos para o genetico> \n");
		return 0;
	}

	limiar_pulsos=atoi(argv[1]); 

	int quantidadeGeracoes;

	int t;
    unsigned long semente = time(0);
	printf("\nSemente utilizada: %ld \n",semente);
    srand48(semente); srand (semente);
//    srand48(1508420298); srand (1508420298);
//    printf("Semente de aleorios FIXA (1508420298)\n");
	read_conf_file(); //Leitura do arquivo de configuração da simulação

	FILE * file= fopen("logGA.txt","w");//Armazena evolução do GA
	//Escolhe os dois primeiros individuos

	//Cria a rede apenas 1 vez. Dependendo do modelo, esta solucao pode nao funcionanar.
	Iz = new Izhikevich_network(NEURONS);
	Genetico * G = new Genetico();

	// LÓGICA PARA SALVAR NÚMERO DE GERAÇÕES
	FILE *arq2 = fopen("quantidadeGeracoes.txt","r");

	while( (fscanf(arq2,"%d\n", &quantidadeGeracoes))!=EOF )
		(*G).geracao = quantidadeGeracoes; // Para não reiniciar taxa de cruzamento sempre que carregar população

	fclose(arq2);
	// END

	(*G).init_pop();//inicializa a população

	for (t=0;t<end;t++){
        if((*G).produzir_nova_geracao()==1)
            break;
        (*G).write_pop(0); 
        printf("%6d %2d %.5f %.5f %.15lf\n",t,(*G).best,(*G).avaliacao_best,(*G).media_populacao,(*G).porcentagem_mutacao);
        fprintf(file,"%6d %2d %.5f %.5f %.15lf\n",t,(*G).best,(*G).avaliacao_best,(*G).media_populacao,(*G).porcentagem_mutacao);
        fflush(file);

        if(t%20==0){
        	struct rusage ru;
        	getrusage(RUSAGE_SELF, &ru);
            double tempo_execucao=ru.ru_utime.tv_sec+ ru.ru_utime.tv_usec/1000000.0+
											ru.ru_stime.tv_sec+ ru.ru_stime.tv_usec/1000000.0;
            if(tempo_execucao > (5*60*60+30*60)){//5 horas
                printf("\nTempo esgotado. Tempo em execucao: %lf", tempo_execucao );
                break;
            }
        }
    }
	logDynamic=true;//Registra em arquivo a dinamica interna do melhor individuo
	float teste=evaluate((*G).best);
	printf("\nMelhor individuo (%d). Fitness: %.5f",(*G).best, teste);
	(*G).write_pop((*G).best); 
	fclose(file);

	delete(Iz);
    delete(G);
	struct rusage ru;
	//Leitura das informacoes do processo.
	getrusage(RUSAGE_SELF, &ru);
	printf("\nTempo: %.5f\n",	ru.ru_utime.tv_sec+ ru.ru_utime.tv_usec/1000000.0+
											ru.ru_stime.tv_sec+ ru.ru_stime.tv_usec/1000000.0);
	if((*G).geracao < (*G).geracoesPrevistas)
		system("./enviar.sh");
	return 0;
}
