/*
Matrix S 3x3

   1   2   3
  -------------
1|   | x |   |
2|   |   |   |
3|   | x |   |
 -------------

1x2 => indicates synapse from neuron 2 to neuron 1
3x2 => indicates synapse from neuron 2 to neuron 3
*/

struct Neuron{
    float a,b,c,d,v,u;
};


class Izhikevich_network{


	public:
		float ** S;//Connection matrix
		Neuron *n;//Array of neurons
		float *I; //Input of each neuron.
		float *G; //Conductunce for each neuron
		float *alfa;//Conductante
		float *delta_fire;//time interval from the spike to the currennt momment
		unsigned long long int time; //iteration time - used in the euler step (ms)
		int N;
        	int *disparos; //> CONTA DISPAROS POR NEURÔNIO


	Izhikevich_network(int total){
		N = total; //Total number of neurons
		time=0;
		//Conductance
		G = (float*) calloc(N,sizeof(float)); //Aloca e inicia como 0
		alfa= (float*) calloc(N,sizeof(float));
		delta_fire=(float*) calloc(N,sizeof(float));
		n = (Neuron*) calloc(N,sizeof(Neuron));
		I = (float*) calloc(N,sizeof(float));
        	disparos = (int*) calloc(N,sizeof(int)); //> ZERA DISPAROS

		//Conection matrix -> NxN
		S = (float **) calloc(N,sizeof(float *));
		for (int l=0;l<N;l++){
			S[l]=(float *)calloc(N,sizeof(float));
			delta_fire[l]=-1;//Indica que o neurônio não disparou.

			//****JA DEFINE O TIPO DOS NEURONIOS.*****
            if(l==0 || l==1 || l==2 || l==3)
                setNeuronParameters(l,0.02,0.2,-50,2);
            else
                setNeuronParameters(l,0.02,0.25, -65,6);

		}
	}

    void setNeuronParameters(int neuronId,float a,float b,float c,float d){
        n[neuronId].a=a;
        n[neuronId].b=b;
        n[neuronId].c=c;
        n[neuronId].d=d;
        n[neuronId].v=c;
        n[neuronId].u=b * n[neuronId].v;
    }

	~Izhikevich_network(){
		for (int l=0;l<N;l++)
			free(S[l]);
		free(S);
		free(n);
		free(I);
		free(G);
		free(alfa);
		free(delta_fire);
        free(disparos);
	}
	void reset_network(){//Reinicia todos os parametros da rede
		time=0;
		for (int l=0;l<N;l++){
			G[l]=0; //reinicia condutancia
			delta_fire[l]=-1;//Indica que o neurônio não disparou.
			n[l].v=n[l].c; //estado inicial do neuronio
			n[l].u=n[l].b*n[l].v;
            disparos[l]=0;
			//I[i]=0;//desnecessario... just in case
			//alfa[i]=0;//just in case
		}
	}

   void Update()
   {
        time++;
        int i,j;
		for (i=0;i<N;i++){

			if (n[i].v>=30){
				n[i].v = n[i].c;
				n[i].u = n[i].u + n[i].d;
				delta_fire[i] = 1;
				G[i]  = delta_fire[i]/alfa[i] * exp(1- delta_fire[i]/alfa[i]);
		                disparos[i] += 1; //> INCREMENTA DISPAROS EM i
			}
			else{

				if(delta_fire[i]!=-1){//Somente altera o delta_fire se já houve algum disparo.
					delta_fire[i]++;
					G[i]  = delta_fire[i]/alfa[i] * exp(1- delta_fire[i]/alfa[i]);
				}
				if((i!=0)&&(i!=1)&&(i!=2)&&(i!=3)){ //> NÃO ZERA PARA NEURÔNIOS DE INPUT
					I[i]=0; //o input nao pode ser zerado para os neuronios de entrada da rede
					for (j = 0; j < N; j++)  //Sum the synapses from neuron j to i
						I[i] = I[i] + S[i][j] * G[j];	//Input of neuron i
				}

				n[i].v=n[i].v+0.5*(0.04*(n[i].v*n[i].v)+5*n[i].v+140-n[i].u+I[i]);
				n[i].v=n[i].v+0.5*(0.04*(n[i].v*n[i].v)+5*n[i].v+140-n[i].u+I[i]);

				n[i].u+=n[i].a*(n[i].b*n[i].v-n[i].u);

				if (n[i].v>=30){
					n[i].v=30; //Set a limit to the threashold
					//fprintf(file_firings,"%lld %d\n",(*Iz).time,i);

				}
			}
		}
	}


};
