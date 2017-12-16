#define POP 1000       //Tamanho da populacao

#define entradas 4 //quantidade de neuronios de entrada
const int LEN = NEURONS*NEURONS - NEURONS - (entradas*3) - (NEURONS-entradas)*4 + NEURONS; //tamanho do cromossomo

//Populacao
double gene[POP][LEN]; 


class Genetico{

    private:
        float fitness_de_parada;//quando a avaliacao do individuo for igual a "fitness_de_parada" significa que o melhor individuo foi encontrado.
        // int geracao;
        double copy[POP][LEN];//Utilizado para armazenar temporariamente uma nova geracao.
        int estavel;//Estabilidade do genetico durantes as geracoes
        int avaliacao_best_anterior; //Para teste de estabilidade
        float soma_avaliacoes;
        float soma_aptidao[POP];
        float aptidao[POP];
        int aptidao_individuo[POP];
        float avaliacao[POP];      //Fitness of each individual
        int eletismo;//Se igual a 1, utiliza eletismo.
        float taxa_cruzamento;
        int controle_mutacao;
        float taxa_mutacao;
        int maximo_geracoes_com_melhor_estavel;//Se o fitness do melhor ficar estável após um certo número de gerações, o genético para de evoluir
        double max_porcentagem_mutacao;//Maior taxa da mutacao. Utilizada para implementar o decaimento na mutacao deterministica


	public:
	    int best;
        int geracao;
	int geracoesPrevistas; // Número de gerações previstas
        float media_populacao;
        float avaliacao_best;
        double porcentagem_mutacao;


	Genetico(){
        /*PARAMETROS DE CONFIGURACAO DO GENETICO*/
	geracoesPrevistas = 6000;
        taxa_cruzamento=0.7;
        eletismo=0; //Com eletismo
        controle_mutacao=2; //Tipo de mutacao 1: fixa; 2:deterministica
        taxa_mutacao=0.1;
        porcentagem_mutacao=0.5; /*Indica a porcentagem de mutacao Ex. 10% para mais ou para menos. Se controle_mutacao=2, entao a porcetagem de mutacao cai
        linearmente ate o valor 0 quando chega na última geracao da evolucao.*/
        max_porcentagem_mutacao=porcentagem_mutacao;
        maximo_geracoes_com_melhor_estavel=10000;//Após 10000 gerações sem alerar o fitness do melhor, o génetico retorno 1 (e não evolui mais a população).
        /*FIM - PARAMETROS */

        //geracao=0; //Conta o numero de geracoes  // Valor setado como 'start' do arquivo config.txt
	    best=0; //armazena o indice do melhor individuo de uma populacao
        estavel=0;//Estabilidade do genetico durantes as geracoes
        avaliacao_best_anterior=0; //Para teste de estabilidade
        fitness_de_parada=1; //Quando o melhor individuo tiver uma avaliacao maior ou igual a "fitness_de_parada", a evolucao termina.

        //Normalizacao de 0 a 100.
        for (int k=0;k<POP;k++){
    	  avaliacao[k]=0;
          aptidao[k] = 0 + (100 - 0)*  ((float)(POP-k))/(POP);
        }
        //Gera os intervalos para roleta
        for (int k=0;k<POP;k++){
          if(k==0)
                soma_aptidao[k] = aptidao[k];
           else
                soma_aptidao[k] = soma_aptidao[k-1]+aptidao[k];
        }
	}

	/* - avalia todos individuos da populacao.*/
	void avaliar_todos(){
        soma_avaliacoes=0;
        best=0;
	    for (int k=0;k<POP;k++){
		    avaliacao[k]=evaluate(k);
            soma_avaliacoes+=avaliacao[k];
			if (avaliacao[best]<avaliacao[k])
				best=k;
        }        
        avaliacao_best=avaliacao[best]; //atualiza a avaliacao do melhor
        media_populacao=soma_avaliacoes/POP; // calcula a media da populacao
	}


    int produzir_nova_geracao(){

        geracao++;//Atualiza o numero de geracoes do genetico.

        /*ETAPA: seleção:*/
        //Avalia todos os individuos, encontra o melhor e calcula a media da populacao
        //Para as proximas geracoes, as avaliacoes já terao sido realizadas pois o procedimento
        //"avaliar_todos" é chamado depois que a nova populacao e' gerada (veja o final deste procedimento)
        if(geracao==1)
			avaliar_todos();

        //Somente para ver o que está ocorrendo. Para matar a curiosidade ;)
        //PODE SER RETIRADO DA EVOLUCAO.
        int curioso=0;
        if(curioso && geracao%10==0){
            logDynamic=true;
            printf("\n**Fit: %f\n",evaluate(best));
            logDynamic=false;
            //if(muito_curioso) ;)
                fflush(stdout); 
        }

        double cromossomo_best[LEN];
        if(eletismo==1)
            for(int k=0;k<LEN;k++)
                cromossomo_best[k]=gene[best][k];

        //Verifica se a melhor a avaliacao da geracao atual e' igual a melhor avaliacao
        //da geracao anterior. Se for igual, soma na variavel que conta a estabilidade ("estavel")
        if(avaliacao[best]==avaliacao_best_anterior)
            estavel++;
        else
            estavel=0;
        avaliacao_best_anterior=avaliacao[best];
        //Se chegou na melhor avaliacao e esta' estavel por 20000 geracoes
		if(avaliacao[best]>=fitness_de_parada || estavel==maximo_geracoes_com_melhor_estavel)
            return 1;
        /*O vetor aptidao_individuo contem os indices dos individuos da
        populacao. A primeira posicao contem o indice do individuo com
        maior avaliacao. As outras posicoes contem os indices dos outros
         individuos ordenados decrescentemnte por valor de avaliacao.
        */
        //MELHORAR A LOGICA
        int maior=0;//Assume que o primeiro e' o maior
        int cont=0;
        for(int k=0;k<POP;k++){
            //Procura o indice do maior
            for(int l=0;l<POP;l++){
               if(avaliacao[l]>=avaliacao[maior])
                    maior=l;
            }
            aptidao_individuo[cont]=maior;
            cont++;
            //MELHORAR            
            avaliacao[maior]=-999999;
        }
        
        //Gerar POP numeros aleatorios entre 0 e a soma das aptidoes (ultima posicao de soma_aptidao).
        //Esses numeros representam os individuos que serao cruzados para gerar novos individuos.
        int aleatorio[POP],selecionados[POP];
	    for (int k=0;k<POP;k++)
	        aleatorio[k]=drand48()*soma_aptidao[POP-1];
            //aleatorio[k]=rand()%((int)soma_aptidao[POP-1]);
        for (int k=0;k<POP;k++){
           for (int l=0;l<POP;l++){
                if(aleatorio[k]<soma_aptidao[l]){
                    selecionados[k]=aptidao_individuo[l];                    
                    break;
                }
           }
        }
        //Realiza o cruzamento
        for (int k=0;k<POP-1;k=k+2){
            int pai = selecionados[k];
            int mae = selecionados[k+ 1];
            float sorteado = drand48();
            if (sorteado < taxa_cruzamento) {
                float alpha=drand48(); //Um alpha para o individuo todo.
        		for (int  i=0; i<LEN; i++){
                    double gene_pai=gene[pai][i];
                    double gene_mae=gene[mae][i];
                    double gene_filho1=gene_pai*alpha+(1-alpha)*gene_mae;
                    double gene_filho2=gene_mae*alpha+(1-alpha)*gene_pai;
                    copy[k][i]  =gene_filho1; //Nova geracao
                    copy[k+1][i]=gene_filho2;

                }
            }
            else{//Faz a copia para a nova populacao
        		for (int  i=0; i<LEN; i++){
                    copy[k][i]  =gene[pai][i]; //Nova geracao
                    copy[k+1][i]=gene[mae][i];
                }
            }
        }
        //Mutacao em cima da nova populacao
        if (controle_mutacao==1) {
           //nao faz nada
        }
        if (controle_mutacao==2) {
           double decaimento = 1-( (double)geracao / (double)(geracoesPrevistas) );
           porcentagem_mutacao = max_porcentagem_mutacao * decaimento ;
        }
       for (int k=0;k<POP;k++){
           for (int l=0;l<LEN;l++){
                if(drand48()<taxa_mutacao){
                    copy[k][l]=copy[k][l]+(drand48()*(porcentagem_mutacao+porcentagem_mutacao)-porcentagem_mutacao);
                    if(copy[k][l]>1.0)
                        copy[k][l]=1.0;
                    if(copy[k][l]<0)
                        copy[k][l]=0;
                 }
            }
        }
        //Copia a nova populacao para a variavel gene[POP][LEN]
		for (int  k=0; k<POP; k++){
		    for (int  l=0; l<LEN; l++){
                    gene[k][l]=copy[k][l]; //Nova geracao
            }
        }
        //Se estiver usando eletismo, coloca cromossomo do melhor 
        //da ultima posicao da populacao. PODE APAGAR O MELHOR DA POPULACAO.
        if(eletismo==1)        	
            for(int k=0;k<LEN;k++)
                gene[POP-1][k]=cromossomo_best[k];
		avaliar_todos();

	// LÓGICA PARA SALVAR NÚMERO DE GERAÇÕES
	FILE *arq3 = fopen("quantidadeGeracoes.txt","w");

	fprintf(arq3,"%d",geracao);

	fclose(arq3);
	// END

        return 0;

    }

	~Genetico(){
	}

    /*Initialise the population at random or loading from file.*/
    void init_pop() {
	    int i,j;
	    if (loadPopulationFromFile) //Load the population from file
	    { FILE * popfile = fopen("logPopulation.txt","r");
		    for (int i=0;i<POP;i++)
		    {    for (int j=0;j<LEN;j++)
					     if(fscanf(popfile,"%lf",&(gene[i][j]))); //if para evitar warning com -Ofast
		    }
		    fclose(popfile);
	    }
	    else
	    {
		    for (i=0;i<POP;i++)
		    {    for (j=0;j<LEN;j++)
				       gene[i][j]=drand48(); //Number between 0 and 1
		    }
		    if (loadBestGeneFromFile) //Load only the best gene from another simulation
		    {	 FILE * bestGenefile = fopen("logBestGene.txt","r");
			     for(int n=0;n<LEN;n++)
                    //Uso de if somente para evitar warnings
				    if(fscanf(bestGenefile,"%lf",&(gene[0][n]))); //Store in position 0. O if evita warning com -Ofast
				    //printf(" %7.5f ",gene[0][n]);
				    fclose(bestGenefile);
		    }
	    }
    }

    void write_pop(int best)
    {
	    if (writeBestGene)
	    { FILE * bestGenefile = fopen("logBestGene.txt","w");
		    for ( int i=0; i<LEN; i++)
			    fprintf(bestGenefile,"%.15lf ",gene[best][i]);
		    fclose(bestGenefile);
	    }
	    if (writePopulation)
	    { FILE * popfile = fopen("logPopulation.txt","w");
		    for (int i=0;i<POP;i++){
		        for (int j=0;j<LEN;j++){
			         fprintf(popfile,"%.15lf ",gene[i][j]);
			      }
	         fprintf(popfile,"\n");
	      }
		    fclose(popfile);
	    }
    }



};
