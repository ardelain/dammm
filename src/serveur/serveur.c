#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <sys/socket.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <pthread.h>


#include <fcntl.h>//?
#include <sys/mman.h>//?
#include <sys/stat.h>//?
/***************************************************************************************************************************
	struct
***************************************************************************************************************************/
typedef struct
{
	int isCo = 0; //si le client est connecté
	struct sockaddr_in addr;//?
	int numero;
	char nom[30];
	char mdp[30];
}
Client;

typedef struct
{
	int numero;//numero client proprietaire
	int type; // super dame ...
}
Piece;

typedef struct
{
	struct Piece piece;
	int isuse;
	int x;
	int y
}
Cellule;

typedef struct
{
	int numero;
	struct Cellule tabJeu[]
	struct Piece tabP[]
	struct Client client1;
	struct Client client2;
}
Jeu;
/***************************************************************************************************************************
	Variable grobale
***************************************************************************************************************************/
#define N 200
#define TAILLE 50
#define LINE_MAX 1024  /* taille MAX en réception */
#define GROUP   "230.0.0.0"
#define PORT	"5000"
#define REQUEST_MAX 1024 

char* FICHIER_CLIENT = "Clients.bin";
char* FICHIER_JEUX = "Jeux.bin";//utile ?
Jeu jeux[100];
Client clients[100];
int nbClient=0;

struct sockaddr_in addr;
pthread_t tabthread[100];//client co
int nbClientCo=0;

/***************************************************************************************************************************
	main
***************************************************************************************************************************/
//1) sauvegarde des joureur et des jeux (inscription ...)  ?
//my_struct = malloc(sizeof(t_struct )); -> pour pointeur
int main(int argc, char **argv){
	struct sockaddr_in  client;
 	struct ip_mreqn mreqn;
	int s, ret;
	socklen_t len_src_addr;
	s=init(addr,mreqn);

	int *new_sock;

	// charge(void*T,int size, int *nb, char* nomFichier)

	while( (client_sock = accept(s, (struct sockaddr *)&client, (socklen_t*)&ret)) )
    {
    	pthread_t _thread;
    	new_sock = malloc(sizeof *new_sock);
  		*new_sock = client_sock;
	
    	if( pthread_create( &_thread , NULL ,  connexion , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
        tabthread[nbClientCo+1] = _thread;
   		/*if(ifaddrClientCo(client_sock) == 0){
   		}else{
   		}*/
   		
  		//request[ret]=0;
  		//printf("%s\n", request);
	}


	//sauv( void* T,int size ,int *nb, char* nomFichier)

}

int ifaddrClientCo(struct sockaddr_in  src_addr){
	int i;
   		for(i = 0;i<nbClient:i++){
   			if(clients[i]!= NULL{
   				if(src_addr ==clients[i].addr){
   					return 1;
   				}
   			}
		}
   	return 0;
}

int ifaddrClientExist(struct sockaddr_in  src_addr){
	int i;
   		for(i = 0;i<nbClient:i++){
   			if(src_addr ==clients[i].addr){
   				return 1;
   			}
   		}
   	return 0;
}
/***************************************************************************************************************************
	serveur de base
***************************************************************************************************************************/
//initialisation du serveur
void init(struct sockaddr_in addr,struct ip_mreqn mreqn){
	int s, ret;
	if((s=socket(AF_INET, SOCK_DGRAM, 0))==-1) {
	   perror("socket"); exit(1);
	}

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl (INADDR_ANY);
	addr.sin_port = htons(atoi(PORT));
	memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));

	if(inet_aton(GROUP, &mreqn.imr_multiaddr)==0) {
		fprintf(stderr,"Pb Adr multicast : %s !\n", GROUP);
		exit(1);
	}
	 
	mreqn.imr_address.s_addr = htonl(INADDR_ANY);
	mreqn.imr_ifindex=0; /* n'importe quelle interface */
	 
	if (setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP, 
			(void *) &mreqn, sizeof(struct ip_mreqn)) < 0)
		perror("setsockopt – IP_ADD_MEMBERSHIP");

	if(bind(s, (struct sockaddr *)&addr, sizeof addr)) {
	   perror("bind"); exit(1);
	}
	return s;
}

void fermeture(){

}

void salleAttente(){

}
/***************************************************************************************************************************
	connection et inscription
***************************************************************************************************************************/


//connexion et inscription
void *connexion(void *socket_c){//struct sockaddr_in src_addr
	int sock = *(int*)socket_c;
	socklen_t len_src_addr;

	char msg[]="Pseudo : ";
	char request[REQUEST_MAX];
	
	Clients nvClient;
	nvClient.isCo = 1; //si le client est connecté
	nvClient.addr = sock; //sock != add !!!!!!!!!!!!!!!!!!!!!!

	/*len_src_addr=sizeof client;
    if((ret=recvfrom(sock , request , 2000 , 0))==-1) {
    		perror("recvfrom !"); exit(1);
   	}*/
	//request[ret]=0;

	if(sendto(sock , msg , 2000 , 0)==-1) {
			perror("sendto"); exit(1);
	}

	len_src_addr=sizeof src_addr;
    if((ret=recvfrom(sock , request , 2000 , 0))==-1) {
    	perror("recvfrom !"); exit(1);
   	}

   	strcpy(request,nvClient.nom);

   	msg="Mot de passe : ";
   	if(sendto(sock , msg , 2000 , 0)==-1) {
			perror("sendto"); exit(1);
	}
	
	if((ret=recvfrom(sock , request , 2000 , 0))==-1) {
    	perror("recvfrom !"); exit(1);
   	}
	strcpy(request,nvClient.mdp);
   	
   	nvClient.numero=nbClient+1;
   	

   	if(ifaddrClientExist(src_addr)==0){
   			nvClient( nvClient);
   			//sauvegarade
   	}else{
   			//confimer connexion
   			
   	}

   	//nbClientCo++;
 	
 	while( (ret = recv(sock , request , 2000 , 0)) > 0 )
    {  
        //case :
        	//action1
        	//action2
    		break
    			 //Send the message back to client
        		write(sock , strcat("invalide requete: ", request) , strlen("invalide requete: "+ request));
        		printf("%s : %s\n",nvClient.nom,strcat("invalide requete: ",request));


    }

	if(ret == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(ret == -1)
    {
        perror("recv failed");
    }

    //nbClientCo--;
    nbClient--;
    Clients[nvClient.num]=NULL;
    //Free the socket pointer
    free(socket_desc);
    close(sock);
    pthread_exit(NULL); 

}
void nvClient(Client nv){
	int i;
   	/*for(i = 0;i<nbClient:i++){
   		if(clients[i]==NULL){
   				clients[i]=nv;
   				return;
   		}
   	}*/
   	clients[nbClient++] = nv;
}
void inscription(char *name, char * mdp){
	
}
/***************************************************************************************************************************
	interaction
***************************************************************************************************************************/

//
char* listerJoueurDiponible(){
	char list[1024];
	for(i = 0;i<nbClient:i++){
   		if(clients[i]=!NULL){
   				strcat(list,"\n");
   				strcat(list,clients[i].nom);
   		}
   	}
   	return list;

}

//
void listerPartie(){
	
}

//
void jouerAvecJoueur(int num_client){
	
}

//
void voirPartie(int num_partie){
	
}

/***************************************************************************************************************************
	jeux
***************************************************************************************************************************/

void jeux(){

}

void recupererplateau(){

}

void actionjoueur(Cellule celluleDepart,Cellule celluleArrive){

}


/***************************************************************************************************************************
	autre
***************************************************************************************************************************/


/*
	void* T : table a sauvegarger exemple :  type nom[]
	int size : taille de la table a sauver
	int *nb : nombre d'element dans la table
	char* nomFichier : nom du fichier de sauvegarde exemple : char* FICHIER_ = ".bin";

	la sauvegare ecrit d'abord le nombre d'élement puis la table
*/
void sauv( void* T,int size ,int *nb, char* nomFichier)
{
	FILE *fe;
	fe=fopen(nomFichier,"wb");
	if ( fe == NULL) {printf("\n pb ouverture %s", nomFichier);}
	fwrite(nb,sizeof(int),1,fe);//nb= getlastindex
	fwrite(T,size,TAILLE,fe);//sauvegarde entier du fichier, voir pour augmenter la taille physique
	fclose(fe);
}


void charge(void*T,int size, int *nb, char* nomFichier)
{
	FILE *fe;
	fe=fopen( nomFichier,"rb");
	if( fe == NULL) {printf("\n pb ouverture %s", nomFichier); *nb = 0; return;}
	fread(nb,sizeof(int),1,fe);
	fread(T,size,*nb,fe);
	fclose(fe);
}