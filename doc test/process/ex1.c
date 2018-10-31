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


int main(int argc, char **argv) {
	
		int tube[2];
		int n ,d ; 
 		int i,j;
 		int status ;
      	pid_t pid;
      	
      	/*
      	printf("donner arg :");
		status = sscanf(argv[1],"%d",&n); //0 -> pas de correspondance, 1 -> correspondance (une))
      	*/
		
		if(pipe(tube)==-1){perror("pipe");exit(errno);}
		/*if((pid=fork()) ==-1){perror("PID");exit(errno);}
		else if(pid ==0){
			close(tube[1]);
			
			fils(tube[1]);
			exit(0);
			}
		close(tube[0]);
		pere(tube[0]);*/
		
		printf("\n===========================\n");

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
		printf("\n===========================\n");
		exit(0);
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
			printf("fils : lecture de  %s ",c);
	}
	printf("fin fils");
	exit(0);
	
}
	
/*
./a 4
je tape
Destination invalide
1 texte
Fils 1 : text
7 texte
Fils 7 inconnu

Ctrl+d ==> fin fichier
	ferme
Fils 1 se termine
Pere se termine
Fims ... se terminents

/////////////////////
./ setsetges
Fils 1 : setsetges*/
