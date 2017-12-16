//Avalia o fitness de um determinado robô da população.
float evaluate (int n ){    

	float fitness_trial = 0.0; //fitness obtido em cada trial. 
	float fitness = 0.0; //fitness final (media dos fitness em cada trial)

	//Input que sera' aplicado a rede
	int atual, penultimo, ultimo, //> ATUAL = N , PENULTIMO = N-1 , ULTIMO = N-2
	    ant; //>  GUARDA INÍCIO DA JANELA ATUAL
	int tarefa[20];
	int i = 0;
	atual = penultimo = ultimo = ant = 0; //> INICIALIZAM COM 0

	/*
	acertosA -> true positive
	acertosB -> true negative
	errosA   -> false positive
	errosB   -> false negative
	*/		
    float TP, TN, FP, FN;
	//float acertosA, acertosB, errosA, errosB;
	float acertosTotais, errosTotais;
	acertosTotais = errosTotais = 0;

	int dinamicaNeuronios[2][16];
	for(int i=0; i<2; i++){
		for(int j=0; j<16; j++){
			dinamicaNeuronios[i][j] = 0;
		}
	}	

	FILE * file_firings, *file_dynamic, *dados;
    if(logDynamic ){ //Grava apenas o ultimo trial
        file_firings  = fopen("firings.txt","w");
        file_dynamic  = fopen("file_dynamic.txt","w");
        dados  = fopen("dados.txt","w");	
    }

    //Define a matriz de pesos (a partir do cromossomo)
    int contador=0;//Para acessar o cromossomo.
    int saveCont=0; //> SALVA CONTADOR ANTES DOS TRIALS
    for(int i=0;i<(*Iz).N;i++){
        for (int j=0;j<(*Iz).N;j++){ //fill in the column number i
            if(i==j || (j==0 || j==1 || j==2 || j==3))//For self-connections and connections to neurons 0,1,2,3
                (*Iz).S[j][i]=0;
            else{
                (*Iz).S[j][i]=gene[n][contador]*30-15;
                contador++;
            }
        }
        //Define o alpha a partir do cromossomo.
        (*Iz).alfa[i]=0.5+gene[n][contador]*9.5; //[0.5,10]
        contador++;
    }
    saveCont = contador; //> SALVA CONTADOR ANTES DOS TRIALS
//    printf("\nContador: %d\n",contador);

	for (int k=0;k<trials;k++){
		
		float alta, //quantidade de vezes q a frequencia deve ser alta (n==n-2)
            baixa;//quantidade de vezes q a frequencia deve ser baixa (n==n-2)

		alta = baixa = TP = TN = FP = FN = 0.0;

        contador = saveCont; //> REINICIA CONTADOR

		//Define estado inicial para rede
	   	(*Iz).reset_network();

		for (int iterations=0;iterations<=5000;iterations++){ //> VALOR PEQUENO PRA TESTAR

			//Calculo do fitness (se necessario)

            if(iterations%100==0){				
                //FITNESS INTERMEDIARIO
        		if(iterations>=300){ //> SE JÁ PASSARAM AS DUAS PRIMEIRAS JANELAS E O FITNESS PODE SER CALCULADO
					ant = iterations; //> GUARDA FIM DA ÚLTIMA JANELA
					if(atual == penultimo)
                        alta++;//frequencia mais alta
                    else
                        baixa++;
					if((((*Iz).disparos[entradas] >= limiar_pulsos)&&(atual == penultimo))){
						//acertosA++;
						TP++;
						if(atual == 1){
							dinamicaNeuronios[0][0]++;
						}
						else if(atual == 2){
							dinamicaNeuronios[0][5]++;
						}
						else if(atual == 3){
							dinamicaNeuronios[0][10]++;
						}
						else if(atual == 4){
							dinamicaNeuronios[0][15]++;
						}
					}
					if((((*Iz).disparos[entradas] < limiar_pulsos)&&(atual != penultimo))){
						//acertosB++;
						TN++;
						if(atual == 1){
							if(penultimo == 2)
								dinamicaNeuronios[0][1]++;
							else if(penultimo == 3)
								dinamicaNeuronios[0][2]++;
							else if(penultimo == 4)
								dinamicaNeuronios[0][3]++;
						}
						else if(atual == 2){
							if(penultimo == 1)
								dinamicaNeuronios[0][4]++;
							else if(penultimo == 3)
								dinamicaNeuronios[0][6]++;
							else if(penultimo == 4)
								dinamicaNeuronios[0][7]++;
						}
						else if(atual == 3){
							if(penultimo == 1)
								dinamicaNeuronios[0][8]++;
							else if(penultimo == 2)
								dinamicaNeuronios[0][9]++;
							else if(penultimo == 4)
								dinamicaNeuronios[0][11]++;
						}
						else if(atual == 4){
							if(penultimo == 1)
								dinamicaNeuronios[0][12]++;
							else if(penultimo == 2)
								dinamicaNeuronios[0][13]++;
							else if(penultimo == 3)
								dinamicaNeuronios[0][14]++;
						}
					}
					if((((*Iz).disparos[entradas] >= limiar_pulsos)&&(atual != penultimo))){
						//errosA++;
						FP++;
						if(atual == 1){
							if(penultimo == 2)
								dinamicaNeuronios[1][1]++;
							else if(penultimo == 3)
								dinamicaNeuronios[1][2]++;
							else if(penultimo == 4)
								dinamicaNeuronios[1][3]++;
						}
						else if(atual == 2){
							if(penultimo == 1)
								dinamicaNeuronios[1][4]++;
							else if(penultimo == 3)
								dinamicaNeuronios[1][6]++;
							else if(penultimo == 4)
								dinamicaNeuronios[1][7]++;
						}
						else if(atual == 3){
							if(penultimo == 1)
								dinamicaNeuronios[1][8]++;
							else if(penultimo == 2)
								dinamicaNeuronios[1][9]++;
							else if(penultimo == 4)
								dinamicaNeuronios[1][11]++;
						}
						else if(atual == 4){
							if(penultimo == 1)
								dinamicaNeuronios[1][12]++;
							else if(penultimo == 2)
								dinamicaNeuronios[1][13]++;
							else if(penultimo == 3)
								dinamicaNeuronios[1][14]++;
						}
					}
					if(((*Iz).disparos[entradas] < limiar_pulsos)&&(atual == penultimo)){					
						//errosB++;
						FN++;
						if(atual == 1){
							dinamicaNeuronios[1][0]++;
						}
						else if(atual == 2){
							dinamicaNeuronios[1][5]++;
						}
						else if(atual == 3){
							dinamicaNeuronios[1][10]++;
						}
						else if(atual == 4){
							dinamicaNeuronios[1][15]++;
						}
					}
				}
				(*Iz).I[atual]=0; //> INPUT DO ANTIGO ATUAL TORNA-SE 0
				penultimo = ultimo; //> N-2 RECEBE ANTIGO N-1
				ultimo = atual; //> N-1 RECEBE ANTIGO N
				atual = rand()%4; //> NOVO N É CALCULADO ALEATORIAMENTE
				(*Iz).I[atual]=15; //> INPUT DO ATUAL TORNA-SE 100
				(*Iz).disparos[entradas] = 0; //> ZERA DISPAROS DA JANELA
			}
            //if(  (iterations-50)%100==0)//input durante 20ms
            //        (*Iz).I[atual]=0;
		   	(*Iz).Update();

			if(logDynamic && k==trials-1){
			   fprintf(file_dynamic,"%lld",(*Iz).time);
			   for (int i=0;i<(*Iz).N;i++) //Para cada neuronio, gravar...
				   fprintf(file_dynamic," %.2f %.2f %.2f %2f %d", (*Iz).n[i].v,(*Iz).G[i],(*Iz).delta_fire[i], (*Iz).I[i],atual );
			   fprintf(file_dynamic,"%d\n",k);
			   for(int j=0; j<16 ; j++){
				fprintf(dados,"%d %d\n",dinamicaNeuronios[0][j],dinamicaNeuronios[1][j]);
			   }
			}
		}//for (interations...)

		float TNR=0;
		if((TN+FP)!=0)
            TNR=TN/(TN+FP);

		float TPR=0;
		if((TP+FN)!=0)
            TPR=TP/(TP+FN);

        fitness_trial = 2*(TNR*TPR)/(TNR+TPR);

        if(fitness_trial<=0.01 && !logDynamic) //Termina se um dos trials ja' está pessimo.
            return 0;
	
		fitness += fitness_trial;
				
		if(logDynamic){ //Grava apenas um trial
//	            printf("Trial %d - Acertos alta:%.1f Acertos baixa:%.1f Qtde alta:%.1f Qtde baixa:%.1f\n",k+1,acertosA,acertosB, alta, baixa);
	            printf("\nTNR: %.3f TPR: %.3f Fitness do trial: %.3f Soma dos fitness %.4f \n(n=n-2)->%.0f   acertos:%.0f (%.1f%)\n(n!=n-2)->%.0f    acertos:%.0f (%.1f%)\n",TNR, TPR, fitness_trial,fitness,alta,TP,(TP*100)/alta,baixa,TN,(TN * 100)/baixa);
      }
		acertosTotais += TP + TN;
		errosTotais += FP + FN;

	}//for trials
	if(logDynamic){ //Grava apenas um trial
	            printf("TOTAL DE ACERTOS: %.0f TOTAL DE ERROS: %.0f\n",acertosTotais,errosTotais);
   }
	return (fitness/trials); //> MÉDIA DOS FITNESS
}
