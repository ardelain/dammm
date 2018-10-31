#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <bits/types.h>
#include <unistd.h>

//=============================================================================
//=============================================================================
#define LG 80



int multipleFils(int nb){
	
	int i;
	int* tabe = (int*)malloc(sizeof(int)*nb);
	int* tabs = (int*)malloc(sizeof(int)*nb);
	for(i=0;i<n;i++){
		tab[i][0] = (int)malloc(sizeof(int)*n);
		tab[i][1] = (int)malloc(sizeof(int)*n);
		FILE* flux;
		flux= fdopen(tube[1],"w");
		tabe[i] = tube[1];
		tabs[i] = tube[0];
	}
	
	
	for(i=0;i<nb;i++){
		int tube[2];
		if(pipe(tube)==-1){perror("pipe");exit(errno);}

		if()
		switch( fork() ) 
		{
               case -1 : 
               			 perror("fork"); exit(errno);
               			 printf("ERREUR");
                        
                         break;
				case 0: 
               			 close(tube[1]);
               			 dup2(tube[0],0);
               			 close(tube[0]);
						 fils(tube);
                         break;
              default : 
					close(tube[0]);
					pere(tube);
					break;
		}
		
	}	
}


int pere(int tube[2]){
	char texte[LG];
	FILE* flux;
	flux= fdopen(tube[1],"w");
	if(flux==NULL)
	{
		fprintf(stderr, " Erreur ouverture du flux");
		exit(-1);
	}
	

	while(fgets(texte,LG,stdin) != NULL)
	{
			fputs(texte,flux);
			fflush(flux);
	}
	fclose(flux);
	wait(NULL);
	exit(0);
}

int fils(int tube[2]){
	char c[LG];
	while(fgets(c,LG,stdin)!= NULL)
	{
			if(chaine[})
			printf("fils : lecture de  %s ",c);
	}
	printf("fin fils");
	exit(0);
	
}

int main(int argc, char **argv) {
	
		
		int n ,d ; 
 		int i,j;
 		int status ;
      	pid_t pid;
      	
      	
      	printf("donner arg :");
		status = sscanf(argv[1],"%d",&n); //0 -> pas de correspondance, 1 -> correspondance (une))
      	
		mulipleFils(n);
		
		printf("\n===========================\n");
		exit(0);
}

	
