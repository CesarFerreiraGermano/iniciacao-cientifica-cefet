void read_conf_file(){ // Leitura de parâmetros no arquivo "config.txt"
    char *cmd;
    char buf[200];
    FILE *arq;

    arq = fopen("config.txt","r");
    if (arq == NULL){
        printf("\nErro ao abrir arquivo de configuracao\n");
        return;
    }
    if (fgets(buf,200,arq))
        ;
    while (!feof(arq)){
        cmd = strtok(buf,"=");
        int val = atoi(strtok(NULL,";"));
		if (strcmp(cmd,"evolution") == 0)
		    if (val==0){
                loadPopulationFromFile=false;
				trials=1;
				loadBestGeneFromFile=true;
				logDynamic=true;
			    break;
			}
		if (strcmp(cmd,"loadPopulationFromFile") == 0)
			loadPopulationFromFile=val;
		if (strcmp(cmd,"trials") == 0)
			trials=val;
		if (strcmp(cmd,"logDynamic")==0)
			logDynamic=val;
		if (strcmp(cmd,"loadBestGeneFromFile")==0)
			loadBestGeneFromFile=val;
		if (strcmp(cmd,"end")==0)
			end=val;
		if (strcmp(cmd,"start")==0)
			start=val;
        if(fgets(buf,200,arq))
            ; //Uso de if para evitar warning com compilação -Ofast
    }
    fclose(arq);
}
