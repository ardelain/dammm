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

#include <unistd.h>
#include <netdb.h>      
#include <string.h>     /* pour memset */
#include <ctype.h>      /* pour toupper */
#include <arpa/inet.h>  /* pour inet_ntop */
#include <sys/types.h> 
/***************************************************************************************************************************
	Variable grobale
***************************************************************************************************************************/
#define N 200
#define TAILLE 50
#define LINE_MAX 1024  /* taille MAX en réception */
#define GROUP   "230.0.0.0"//"127.0.0.1"
#define PORT	"5000"
#define REQUEST_MAX 1024 
#define MAX_CLIENTS 20
#define DGRAM_MAX 1024  /* taille MAX en réception */

/***************************************************************************************************************************
	struct
***************************************************************************************************************************/

typedef struct Client
{
	//struct sockaddr_in addr;//?
	int addr;
	int isCo ; //si le client est connecté
	int isinGame;//en jeu ou spectaur
	int demandeDeJeu;
	int socketJDemande;//table des joueur qui ont demandé de jouer
	char nomDemande[30];
	int numero;
	int numJeu;
	int isInvite;
	int nbParties;
	int nbPoints;
	char nom[30];
	char mdp[30];
}
Client;

typedef struct Piece
{
	int numero;//numero client proprietaire
	int type; // super dame ...
	int x;
	int y;
}
Piece;

typedef struct Cellule
{
	struct Piece piece;//si = null pas de piece presente
	int isuse;
}
Cellule;

typedef struct
{
	int numero;
	struct Cellule tabJeu[10][10];
	struct Piece tabP[40];
	int tour;
	struct Client client1;
	struct Client client2;
}
Jeu;

struct DeplacementUser{
	int numJeu;
	int x1;
	int y1;
	int x2;
	int y2;
};
/***************************************************************************************************************************
	variable
***************************************************************************************************************************/
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

char* FICHIER_CLIENT = "Clients.bin";
char* FICHIER_JEUX = "Jeux.bin";//utile ?
Jeu jeux[100];
int nbJeux= 0;
Client clients[100];
int nbClient=0;//nb de client

struct sockaddr_in addr;
pthread_t tabthread[100];//client co
int nbClientCo=0;
int nbInvite=0;
/***************************************************************************************************************************
	methode lambda
***************************************************************************************************************************/
int ifaddrClientCo(struct sockaddr_in  src_addr){
	int i;
   		for(i = 0;i<nbClient;i++){
   			if(clients[i].isCo != 1){
   				return 1;
   			}
		}
   	return 0;
}

int ifaddrClientExist(char nom[30]){
	int i;
   		for(i = 0;i<nbClient;i++){
   			if(strcmp(nom,clients[i].nom)==0){
   				return 1;
   			}
   		}
   	return 0;
}

Client getClient(char nom[30]){
	int i;
	Client client	;//client null == aucun correspondant
    for(i = 0;i<nbClient;i++){
   		if(strcmp(nom,clients[i].nom)==0){
   			return clients[i];
   		}
   	}

   	return client;
}

int getClientIt(char nom[30]){
	int i;
   		for(i = 0;i<nbClient;i++){
   			if(strcmp(nom,clients[i].nom)==0){
   				return i;
   			}
   		}
   	return -1;
}

void afficherTtClients(){
	int i;
   		for(i = 0;i<nbClient;i++){
   			printf(" nom : %s,  co : %d\n",clients[i].nom,clients[i].isCo);
   		}
}

void decoClient(char nom[30]){
	int i;
   		for(i = 0;i<nbClient;i++){
   			if(strcmp(nom,clients[i].nom)==0){
   				clients[i].isCo = 0;
   				return;
   			}
   		}
}


/*
	void* T : table a sauvegarger exemple :  type nom[]
	int size : taille de la table a sauver
	int *nb : nombre d'element dans la table
	char* nomFichier : nom du fichier de sauvegarde exemple : char* FICHIER_ = ".bin";

	la sauvegare ecrit d'abord le nombre d'élement puis la table
*/
void sauv( void* T,int size ,int *nb, char* nomFichier)
{
	
	Client T2[MAX_CLIENTS];
	int i,j,n=0;
   	for(i = 0;i<nbClient;i++){
   		if(clients[i].isInvite!=1){
   			T2[n] = clients[i];
   			n++;
   		}
   	}

	FILE *fe;
	fe=fopen(nomFichier,"wb");

	int nn=*nb-nbInvite;	
	if ( fe == NULL) {printf("\n pb ouverture %s\n", nomFichier);}
	fwrite(&nn,sizeof(int),1,fe);//nb= getlastindex
	fwrite(T2,size,TAILLE,fe);//sauvegarde entier du fichier, voir pour augmenter la taille physique
	fclose(fe);
}


void charge(Client*T,int size, int *nb, char* nomFichier)
{
	FILE *fe;
	fe=fopen( nomFichier,"rb");
	if( fe == NULL) {printf("\n pb ouverture %s\n", nomFichier); *nb = 0; return;}
	fread(nb,sizeof(int),1,fe);
	fread(T,size,*nb,fe);
	fclose(fe);
}



void *ecouter(void *sock){
	int lg,ret;
	int s= *(int*)sock;
	char msg[LINE_MAX];
	char request[LINE_MAX];
	while(1){
		//response[ret]=0;msg[strlen(msg)-1] = '\0';	
		if((ret=recv(s, request , LINE_MAX , 0))==-1) {
    			perror("recvfrom !"); return;//exit(1);
   		}
   		printf("%s\n",request);
   		//clear the message buffer
		memset(request, 0, LINE_MAX);
   		sleep (0.1);
	}
	pthread_exit (NULL);
}
void *emettre(void *sock){
	int lg,ret;
	int s= *(int*)sock;
	char msg[LINE_MAX];
	while(1 && strcmp(msg,"quit") !=0 ){
		scanf("%s",&msg);
		if(msg == NULL){
				printf("\nMessage null !");return;//exit(1);
		}
		if(send(s , msg , LINE_MAX , 0)==-1) {
			perror("sendto"); return;//exit(1);
		}
		memset(msg, 0, LINE_MAX);
		sleep (0.1);
	}

	pthread_exit (NULL);
}


/***************************************************************************************************************************
	serveur de base
***************************************************************************************************************************/



/***************************************************************************************************************************
	gerer Client
***************************************************************************************************************************/
int verificationConnexion(char *name, char * mdp){
	int i;
   	for(i = 0;i<nbClient;i++){
   		if(strcmp(clients[i].nom, name) == 0 && strcmp(clients[i].mdp, mdp) == 0 && clients[i].isCo != 1){
   			clients[i].isCo = 1;
   			return 1;
   		}else{
			if(strcmp(clients[i].nom, name) == 0 && strcmp(clients[i].mdp, mdp) == 0 && clients[i].isCo == 1){
				return -1;
			}
		}
	}
   	return 0;
}

void envoyerClient(int sock,Client client){
	char cli[REQUEST_MAX]="client";
	if(send(sock , cli , REQUEST_MAX , 0)==-1) {
		perror("envoyerClient sendto"); return;//exit(1);
	}
	if(send(sock , &client , sizeof(client) , 0)==-1) {
		perror("envoyerClient sendto"); return;//exit(1);
	}
}
//connexion et inscription
Client connexion(int sock){
	int ret;
	int inscription = 0;
	socklen_t len_src_addr;
	char el[REQUEST_MAX]="1.connexion\t2.inscription\t3.inviter";//noter le numero d'action a réaliser (0 pour retour à ce menu):\n
	char m[REQUEST_MAX]="Noter le numero d'action a réaliser :";
	char m2[REQUEST_MAX]="1.connexion\n";
	char m3[REQUEST_MAX]="2.inscription\n";
	char m4[REQUEST_MAX]="3.inviter\n";
	char msg[REQUEST_MAX]="Pseudo : ";
	char msg2[REQUEST_MAX]="Mot de passe: ";
	char request[REQUEST_MAX];
	char mess[REQUEST_MAX]="Probleme mauvaise entrer, recommencez";

	char mdp[1024],nom[1024];

	/*
	if(send(sock , m , REQUEST_MAX , 0)==-1 ||send(sock , m2 , REQUEST_MAX , 0)==-1 ||send(sock , m3, REQUEST_MAX , 0)==-1 ||send(sock , m4 , REQUEST_MAX , 0)==-1) {
			perror("sendto"); exit(1);
	}
	*/
	char etoile[REQUEST_MAX]="\n************************************************************";
    if(send(sock , etoile, REQUEST_MAX , 0)==-1) {
							perror("sendto"); return;
	}
	if(send(sock , etoile, REQUEST_MAX , 0)==-1) {
							perror("sendto"); return;
	}
    if(send(sock , m , REQUEST_MAX , 0)==-1) {
			perror("sendto"); return;//exit(1);
	}
	if(send(sock , el , REQUEST_MAX , 0)==-1) {
    			perror("sendto"); return;//exit(1);
    }

    if((ret=recv(sock , request , REQUEST_MAX , 0))==-1) {
    	perror("recvfrom !2"); return;//exit(1);
   	}
   	if(request == NULL || sizeof(request) > 1024){
					printf("\nMessage PB");return;//exit(1);
	}
	if(strcmp(request,"")==0){
	 	puts("connexion Message PB");
		return;
	}
	int i;
   	if((i = atoi(request)) < 0){
   		puts("ERREUT atoi");
		return;
	}
	Client nvClient;
	int inscr = 0;
	memset(request, 0, LINE_MAX);
   	switch (i){
   		case 1://connexion
	   		memset(request, 0, LINE_MAX);
	   		int co = 0;
	   		while(co == 0){
	   			if(send(sock , msg , REQUEST_MAX , 0)==-1) {
							perror("sendto"); return;//exit(1);
				}
	   			if((ret=recv(sock , request , REQUEST_MAX , 0))==-1) {
	    			perror("recvfrom !"); return;//exit(1);
		   		}
				if(strcmp(request,"")==0){ //|| request == NULL || sizeof(request) > 1024
				   		puts("connexion Message PB");
				   		return;
				}
		   		strcpy(nom,request);

		   		memset(request, 0, LINE_MAX);
				if(ifaddrClientExist(nom)==1){
					if(send(sock , msg2 , REQUEST_MAX , 0)==-1) {
							perror("sendto"); return;//exit(1);
					}
					if((ret=recv(sock , request , REQUEST_MAX , 0))==-1) {
				    	perror(" recvfrom !"); return;//exit(1);
				   	}
					if(strcmp(request,"")==0){//|| request == NULL || sizeof(request) > 1024
				   		puts("connexion Message PB");
				   		return;
				   	}
					strcpy(mdp,request);
				   	memset(request, 0, LINE_MAX);
				   	int vari;
					if((vari = verificationConnexion(nom, mdp))<=0){
						if(vari == -1 ){
							char mv[REQUEST_MAX]="Joueur Deja Connecté:";
							if(send(sock , mv , REQUEST_MAX , 0)==-1) {
								perror("sendto"); return;//exit(1);
							}
						}else{
							char mv[REQUEST_MAX]="Probleme mdp, reesayer (psudo + mdp):";
							if(send(sock , mv , REQUEST_MAX , 0)==-1) {
								perror("sendto"); return;//exit(1);
							}
						}
					}else{
						char mc[REQUEST_MAX]="Vous etes connecté";
						if(send(sock , mc , REQUEST_MAX , 0)==-1) {
							perror("sendto"); return;//exit(1);
						}
						co = 1;
						pthread_mutex_lock((pthread_mutex_t*)&m);
						clients[getClientIt(nom)].isCo = 1;
						clients[getClientIt(nom)].addr = sock;
						Client c = getClient(nom);
						pthread_mutex_unlock((pthread_mutex_t*)&m);
						printf("Connexion: nom : %s sock %d\n",nom,c.addr);
						envoyerClient(sock,c);
						return c;
					}
				}else{
					char mv[REQUEST_MAX]="pseudo inexistant, reesayer :";
					if(send(sock , mv , REQUEST_MAX , 0)==-1) {
							perror("sendto"); return;//exit(1);
					}
				}

	   		}
   		break;
			
		case 2://inscription
			memset(request, 0, LINE_MAX);

	   		while(inscr == 0){
	   			if(send(sock , msg , REQUEST_MAX , 0)==-1) {
							perror("sendto"); return;//exit(1);
				}
	   			if((ret=recv(sock , request , REQUEST_MAX , 0))==-1) {
	    		perror("recvfrom !"); return;//exit(1);
		   		}
		   		if(strcmp(request,"")==0){//|| request == NULL || sizeof(request) > 1024
		   				puts(" inscription strcmp(request,"")==0 : nom");
				   		return;
				   }
		   		strcpy(nom,request);
		   		memset(request, 0, LINE_MAX);
				if(ifaddrClientExist(nom)==0){
					strcpy(nvClient.nom,nom);
					if(send(sock , msg2 , REQUEST_MAX , 0)==-1) {
							perror("sendto"); return;//exit(1);
					}
					if((ret=recv(sock , request , REQUEST_MAX , 0))==-1) {
				    	perror("recvfrom !"); return;//exit(1);
				   	}
				   	if(strcmp(request,"")==0){//|| request == NULL || sizeof(request) > 1024
				   		puts(" inscription strcmp(request,"")==0 : mdp");
				   		return;
				   	}
				   	char mc[REQUEST_MAX]="Vous etes connecté et inscrit";
					if(send(sock , mc , REQUEST_MAX , 0)==-1) {
							perror("sendto"); return;//exit(1);
					}
				   	pthread_mutex_lock((pthread_mutex_t*)&m);
					strcpy(mdp,request);
				   	strcpy(nvClient.mdp,mdp);
				   	nvClient.addr = sock;
				   	nvClient.isCo = 1; 
				   	//nvClient.num = nbClient;
					inscr = 1;
					memset(request, 0, LINE_MAX);
					clients[nbClient] = nvClient;
					pthread_mutex_unlock((pthread_mutex_t*)&m);
					nbClient++;
					envoyerClient(sock,nvClient);
					printf("Connexion: nom : %s\n",nom);
					return nvClient;
				}else{
					char mv[REQUEST_MAX]="pseudo existant, reesayer :";
					if(send(sock , mv , REQUEST_MAX , 0)==-1) {
							perror("sendto"); return;//exit(1);
					}
				}
	   		}
   		break;

   		case 3://inviter
   			memset(request, 0, LINE_MAX);

	   		while(inscr == 0){
	   			if(send(sock , msg , REQUEST_MAX , 0)==-1) {
							perror("sendto"); return;//exit(1);
				}
	   			if((ret=recv(sock , request , REQUEST_MAX , 0))==-1) {
	    		perror("recvfrom !"); return;//exit(1);
		   		}
		   		if(strcmp(request,"")==0){//|| request == NULL || sizeof(request) > 1024
		   				puts(" inviter strcmp(request,"")==0 : nom");
				   		return;
				   }
		   		strcpy(nom,request);

		   		memset(request, 0, LINE_MAX);
				if(ifaddrClientExist(nom)==0){
					strcpy(nvClient.nom,nom);
				   	char mc[REQUEST_MAX]="Vous etes invité !";
					if(send(sock , mc , REQUEST_MAX , 0)==-1) {
							perror("sendto"); return;//exit(1);
					}
				   	pthread_mutex_lock((pthread_mutex_t*)&m);
					strcpy(mdp,request);
				   	strcpy(nvClient.mdp,mdp);
				   	nvClient.addr = sock;
				   	nvClient.isCo = 1; 
				   	nvClient.isInvite = 1;
				   	//nvClient.num = nbClient;
					inscr = 1;
					memset(request, 0, LINE_MAX);
					clients[nbClient] = nvClient;
					pthread_mutex_unlock((pthread_mutex_t*)&m);
					nbClient++;
					nbInvite++;
					envoyerClient(sock,nvClient);
					printf("Connexion: nom : %s\n",nom);
					return nvClient;

				}else{
					char mv[REQUEST_MAX]="pseudo existant, reesayer :";
					if(send(sock , mv , REQUEST_MAX , 0)==-1) {
							perror("sendto"); return;//exit(1);
					}
				}
			}
   		break;
   		default:
   			
			if(send(sock , mess , REQUEST_MAX , 0)==-1) {
					perror("sendto"); return;//exit(1);
			}
   			printf("\nPB mauvaise commande\n");
   			connexion(sock);
   		break;
   	}
}

/***************************************************************************************************************************
	interaction salle d'attente
***************************************************************************************************************************/
//
char* listerJoueurDiponible(){
	char *list;
	int i;
	for(i = 0;i<nbClient;i++){
   		if(clients[i].isCo == 0){
   				strcat(list,"\n");
   				strcat(list,clients[i].nom);
   		}
   	}
   	return list;
}
//
int  jouerAvecJoueur(int sock,char monnom[30],char nom[30]){
		int i;
		//int s= *(int*)sock;
		char msg[LINE_MAX] ;
		strcat(msg," (oui/non) accepter de jouer avec  ");
		strcat(msg, monnom);
   		for(i = 0;i<nbClient;i++){
   			if(strcmp(nom,clients[i].nom)==0 && strcmp(nom,monnom)!=0){
   				clients[i].socketJDemande = sock;
   				clients[i].demandeDeJeu = 1;
   				strcpy(clients[i].nomDemande,monnom);
   				char etoile[REQUEST_MAX]="************************************************************\n";
				if(send(clients[i].addr , etoile, REQUEST_MAX , 0)==-1) {
											perror("sendto"); return;
				}
   				if(send(clients[i].addr , msg , LINE_MAX , 0)==-1) {
					perror("jouerAvecJoueur send failed");
				}
				strcpy(msg,"reception demande de jeu");
				if(send(sock, msg , LINE_MAX , 0)==-1) {//inutile
					perror("jouerAvecJoueur send failed");
				}
				return 1 ;
   			}
   		}
   		return 0 ;
}

//
void listerPartie(){
	
}


//
void voirPartie(int num_partie){
	
}


int getexec(int sock,char *command, char *response, unsigned size,Client client) {
    pthread_mutex_lock((pthread_mutex_t*)&m);
    int r;
	memset (response, 0, sizeof (response));
	strcpy(response,"exec !");
	int boo = 0;//0 si on envoie le message (action normale) 1 sinon
    /*
    if(send(sock , response , REQUEST_MAX , 0)==-1) {
							perror("sendto"); return;//exit(1);
		}*/
   	if(strcmp(command,"list") == 0){
   		char list[1024];
		int i;
		strcat(list,"\njoueurs :\n");
		for(i = 0;i<nbClient;i++){
	   		if(clients[i].isCo == 1){
	   				strcat(list,"\t-");
	   				strcat(list,clients[i].nom);
	   				strcat(list,"\n");
	   		}
	   	}
   		strcpy(response,list);
   		memset(list, 0, LINE_MAX);
   		boo=0;
	}else if(strcmp(command,"jouer") == 0){
		char m[REQUEST_MAX] = "avec quel joueur jouer ? : ";
		if(send(sock , m , REQUEST_MAX , 0)==-1) {
							perror("sendto"); return;//exit(1);
		
		}
		if((recv(sock , response , REQUEST_MAX , 0))==-1) {
	    		perror("recvfrom !");return;// exit(1);
		}
		if(strcmp(response,"")==0){//response == NULL || sizeof(response) > 1024 || 
				printf("\nMessage null ! get exec jouer");return;//exit(1);
		}
	    if(jouerAvecJoueur(sock,client.nom,response) == 0){//enlever if
	    	puts("Probleme adversaire deco ou null");
	    	//pthread_mutex_unlock((pthread_mutex_t*)&m);
	    	//int res = getexec(sock,command, response, size,client);
	    	char cc[1024];
    		strcpy(cc,response);
    		strcpy(response,"Demande non envoyé a ");
    		strcat(response,cc);
    		boo = 0;
    	}else{
    		char cc[1024];
	    	strcpy(cc,response);
	    	strcpy(response,"Demande envoyé a ");
	    	strcat(response,cc);
	    	boo = 0;
    	}
    	//memset (response, 0, sizeof (response));// REPONSE VIDE ............................................TEST1TEST2ELTEST
	}else if(strcmp(command,"TEST1") == 0 || strcmp(command,"TEST2") == 0|| strcmp(command,"ELTEST") == 0){
		boo = 1;
	}else if(strcmp(command,"voir") == 0){
    	char m[REQUEST_MAX] = "Quelle partie ? : ";
		if(send(sock , m , REQUEST_MAX , 0)==-1) {
							perror("sendto"); return;//exit(1);
		
		}
		if((recv(sock , response , REQUEST_MAX , 0))==-1) {
	    		perror("recvfrom !");return;// exit(1);
		}
		if(strcmp(response,"")==0){//response == NULL || sizeof(response) > 1024 || 
				printf("\nMessage null ! get exec voir");return;//exit(1);
		}
		boo = 0;
		//..........................................................................................................
	   	/*if(voirPartie(sock,client.nom,response) == 0){//enlever if
	   		puts("probleme adversaire deco ou null");
	   		getexec(sock,command, response, size,client) ;
	   		return;
    	}*/
	}else if(strcmp(command,"parties") == 0){
		puts("paaa");
   		char list[1024];
		int i;
		strcat(list,"jeux : ");
		for(i = 0;i<nbJeux;i++){
	   			strcat(list,"\n");
	   			char s[5]; // Nombre maximal de chiffres + 1
				sprintf(s, "%d", jeux[i].numero);
	   			strcat(list,s);
	   			strcat(list," ");
	   			strcat(list,jeux[i].client1.nom);
	   			strcat(list," ");
	   			strcat(list,jeux[i].client2.nom);
	   	}
   		strcpy(response,list);
   		boo = 0;
	}/*else if(strcmp(request,"")){
    //.......
    */else{
    	strcpy(response,"Mauvaise Commande.");//(help pour la liste des commandes)
        //boo = 1;
        //memset(response, 0, LINE_MAX);
    }
	if(boo==0){
		char etoile[REQUEST_MAX]="\n************************************************************\n";
		if(send(sock , etoile, REQUEST_MAX , 0)==-1) {
				perror("sendto"); return;
		}
	    // Émission du datagramme réponse /
    	if(send(sock , response , REQUEST_MAX , 0)==-1) {
    							perror("sendto"); return;//exit(1);
    	}
    	boo =0;
	}


	pthread_mutex_unlock((pthread_mutex_t*)&m);
}

struct source{
	char nom1[30];
	char nom2[30];
};
/***************************************************************************************************************************
	Jeu
***************************************************************************************************************************/

void afficher(Jeu jeu){

	int i, j, k;
	printf("   ");
	for(k = 0; k < 10; k++)
	{
		printf(" %d", k);
	}
	printf("\n");

	for(i = 0; i < 10; i++){
		printf("\n%d  ", i);
		for(j = 0; j < 10; j++){
			if((jeu).tabJeu[i][j].isuse == 1){
				if((jeu).tabJeu[i][j].piece.numero == 1){
					// printf("%d %d", i, j);
					printf("|x");
				}
				else if((jeu).tabJeu[i][j].piece.numero == 2) 
					printf("|o");
			}
			else printf("| ");
		}
		printf("|");
	}
	printf("\n");
}

int nbPionClient(Jeu *jeu, Client c){

	int i, j, nbPion = 0;

	for(i = 0; i < 10; i++){
		for(j = 0; j < 10; j++){
			if((*jeu).tabJeu[i][j].isuse == 1){
				if((*jeu).tabJeu[i][j].piece.numero == c.numero){
					nbPion = nbPion +1;
				}
			}
		}
	}

	return nbPion;
}

int rechercherPionJoueur(Jeu *jeu, int x, int y, int numC){

	int i;
	//printf("--->%d %d\n",(*jeu).tabJeu[x][y].piece.numero ,numC);
	if(((*jeu).tabJeu[x][y].isuse == 1) && ((*jeu).tabJeu[x][y].piece.numero == numC)){
		return 0;
	}

	else if (((*jeu).tabJeu[x][y].isuse == 1) && ((*jeu).tabJeu[x][y].piece.numero != numC))
	{
		return 2;
	}

	return 1;
}

int rechercherPionPlateau(Jeu *jeu, int x, int y, int numC){

	int i;

	if(((*jeu).tabJeu[x][y].isuse != 1) && ((*jeu).tabJeu[x][y].isuse != 2)){
		return 1;
	}

	return 0;
}

//contrôle que la case vers où va le pion est libre pour le Joueur1 et le Joueur2
int deplacement(int numC, int x1, int x2, int y1, int y2){
	if(numC == 1){
		if((x2 == x1-1 && y2 == y1+1) || (x2 == x1+1 && y2 == y1+1))
			return 0;
	}
	if(numC == 2){
		if((x2 == x1-1 && y2 == y1-1) || (x2 == x1+1 && y2 == y1-1))
			return 0;
	}
	return 1;
}

//contrôle si la case adjacente au pion est libre
int caseIsLibreManger(Jeu *jeu, int numC, int x1, int x2, int y1, int y2, int *x3, int *y3){

	int p;

	if(x2 == x1-1 && y2 == y1+1){
		*x3 = x2-1;
		*y3 = y2+1;
	}
	else if(x2 == x1+1 && y2 == y1+1){
		*x3 = x2+1;
		*y3 = y2+1;
	}
	
	else if(x2 == x1-1 && y2 == y1-1){
		*x3 = x2-1;
		*y3 = y2-1;
	}
	else if(x2 == x1+1 && y2 == y1-1){
		*x3 = x2+1;
		*y3 = y2-1;
	}
	p=rechercherPionPlateau(jeu, *x3, *y3, numC);
	if(p == 1){
		return 1;
	}

	p=rechercherPionJoueur(jeu, *x3, *y3, numC);
	if(p == 1)
		return 0;
	else
		return 1;
}

void vider_buffer(void)
{
    int c;
  
    do {
        c = getchar();
    } while (c != '\n' && c != EOF);
}

int choisirPositionGauche(){

	vider_buffer();

	char pos=""; 
	int valeur;

	printf("Donnez moi sa position x (abscisse) :\n");
	scanf("%c", &pos);

	while(sscanf(&pos, "%d", &valeur) != 1){
		printf("\nEntrez un int pour continuer\n");
		vider_buffer();
		
		printf("Donnez moi sa position x (abscisse) :\n");
		scanf("%c", &pos);
	}

	return valeur;
}

int choisirPositionDroite(){

	vider_buffer();

	char pos=""; 
	int valeur;

	printf("Donnez moi sa position y (ordonnée) :\n");
	scanf("%c", &pos);

	while(sscanf(&pos, "%d", &valeur) != 1){
		printf("\nEntrez un int pour continuer\n");
		vider_buffer();
		
		printf("Donnez moi sa position y (ordonnée) :\n");
		scanf("%c", &pos);
	}

	return valeur;
}


void manger(Jeu *jeu, int numC, int x1, int x2, int y1, int y2, int *x3, int *y3){

	(*jeu).tabJeu[x1][y1].isuse = 2;
	(*jeu).tabJeu[x1][y1].piece.numero = 0;
	(*jeu).tabJeu[x2][y2].isuse = 2;
	(*jeu).tabJeu[x2][y2].piece.numero = 0;
	(*jeu).tabJeu[*x3][*y3].isuse = 1;
	(*jeu).tabJeu[*x3][*y3].piece.numero = numC;
}

int mangerAuto(Jeu *jeu, int x1, int x2, int y1, int y2, int numC){

	int p, *x3, *y3;

	if((*jeu).tabJeu[x2][y2].piece.numero == 1 || (*jeu).tabJeu[x2][y2].piece.numero == 2){
		if((*jeu).tabJeu[x2][y2].isuse == 1 && (*jeu).tabJeu[x2][y2].piece.numero != numC){
			p=caseIsLibreManger(jeu, numC, x1, x2, y1, y2, &x3, &y3);
			if(p == 0){
				manger(jeu, numC, x1, x2, y1, y2, &x3, &y3);
				return 0;
			}
		}
	}

	return 1;
}

int deplacerAuto(Jeu *jeu, Client c){

	int i, j, p, x1, x2, y1, y2;

	for(i = 0; i < 10; i++){
		for(j = 0; j < 10; j++){
			if((*jeu).tabJeu[i][j].isuse == 1 && (*jeu).tabJeu[i][j].piece.numero == c.numJeu){
				x1 = i;
				y1 = j;

				x2 = x1-1;
				y2 = y1+1;
				p=mangerAuto(jeu, x1, x2, y1, y2, c.numJeu);
				if (p == 0)
				{
					return 0;
				}

				x2 = x1+1;
				y2 = y1+1;
				p=mangerAuto(jeu, x1, x2, y1, y2, c.numJeu);
				if (p == 0)
				{
					return 0;
				}

				x2 = x1-1;
				y2 = y1-1;
				p=mangerAuto(jeu, x1, x2, y1, y2, c.numJeu);
				if (p == 0)
				{
					return 0;
				}

				x2 = x1+1;
				y2 = y1-1;
				p=mangerAuto(jeu, x1, x2, y1, y2, c.numJeu);
				if (p == 0)
				{
					return 0;
				}
			}

		}
	}

	return 1;
}

int deplacerPion(Jeu *jeu, int numJeu,int x1,int y1,int x2,int y2){
	int *x3, *y3, p, d, i;
	//printf("deplacerPion->> %d",numJeu);
	p=rechercherPionPlateau(jeu, x1, y1, numJeu);
	if(p == 1){
		//printf("\nErreur Jeton hors du plateau\n");
		return 1;
	}
	p=rechercherPionJoueur(jeu, x1, y1, numJeu);
	if(p == 0){
		p=rechercherPionPlateau(jeu, x2, y2, numJeu);
		if(p == 1){
			//printf("\nErreur Jeton hors du plateau\n");
			return 1;
		}
		p=rechercherPionJoueur(jeu, x2, y2, numJeu);
		if(p == 1){
			d=deplacement(numJeu, x1, x2, y1, y2);
			if(d == 1){
				//printf("\nDéplacement non valide !\n\n");
				return 1;
			}
			(*jeu).tabJeu[x1][y1].isuse = 2;
			(*jeu).tabJeu[x1][y1].piece.numero = 0;
			(*jeu).tabJeu[x2][y2].isuse = 1;
			(*jeu).tabJeu[x2][y2].piece.numero = numJeu;
			return 0;
		}
		else if(p == 2){
			p=caseIsLibreManger(jeu, numJeu, x1, x2, y1, y2, &x3, &y3);
			if(p == 0){
				manger(jeu, numJeu, x1, x2, y1, y2, &x3, &y3);
				return 0;
			}
			else{
				//printf("\nDéplacement impossible !\n");
				return 1;
			}
		}
		else{
			//printf("\nCette case est occupée !\n\n");
			return 1;
		}
	}
	else{
		//printf("Ce n'est pas votre pion !\n");
		return 1;
	}
}


Jeu getJeu(char nom[30]){
	int i;
	Jeu j;
   	for(i = 0;i<nbJeux;i++){
  		if(strcmp(nom,jeux[i].client1.nom)==0 || strcmp(nom,jeux[i].client2.nom)==0){
   				return jeux[i];
  		}
   	}
   	return j;
}
int getJeuInt(char nom[30]){
	int i;
	Jeu j;
   	for(i = 0;i<nbJeux;i++){
  		if(strcmp(nom,jeux[i].client1.nom)==0 || strcmp(nom,jeux[i].client2.nom)==0){
   				return i;
  		}
   	}
   	return -1;
}

/*
void afficher(Jeu jeu){
	int i, j, k;
	printf("   ");
	for(k = 0; k < 10; k++)
	{
		printf(" %d", k);
	}
	printf("\n");

	for(i = 0; i < 10; i++){
		printf("\n%d  ", i);
		for(j = 0; j < 10; j++){
			if((jeu).tabJeu[i][j].isuse == 1){
				if((jeu).tabJeu[i][j].piece.numero == 1){
					// printf("%d %d", i, j);
					printf("|x");
				}
				else if((jeu).tabJeu[i][j].piece.numero == 2)
					printf("|o");
			}
			else printf("| ");
		}
		printf("|");
	}
	printf("\n");
}
*/

char * charJeu(Jeu *jeu){
	char * c;//char *c;
	int i, j, k;
	printf("   ");
	for(k = 0; k < 10; k++)
	{
		printf(" %d", k);
	}
	printf("\n");

	for(i = 0; i < 10; i++){
		printf("\n%d  ", i);
		for(j = 0; j < 10; j++){
			if((*jeu).tabJeu[i][j].isuse == 1){
				if((*jeu).tabJeu[i][j].piece.numero == 1){
					// printf("%d %d", i, j);
					strcat(c,"|x");
				}
				else if((*jeu).tabJeu[i][j].piece.numero == 2) 
					strcat(c,"|o");
			}
			else strcat(c,"| ");
		}
		strcat(c,"| ");
	}	
	char s[5]; // Nombre maximal de chiffres + 1
   	sprintf(s, "%d", i);
	strcat(c,s);
	strcat(c,"\n");
	return c;
}
void envoyerStrucJeu(int sock, Jeu jeu){
	puts("Envoie jeu");
	int i,j;
	char c[REQUEST_MAX];
	strcpy(c,"xJeu");

	/*char etoile[REQUEST_MAX]="\n************************************************************\n";
	if(send(sock , etoile, REQUEST_MAX , 0)==-1) {
			perror("sendto"); return;
	}*/
	if(send(sock , c, REQUEST_MAX , 0)==-1) {
		perror("sendto"); return;//exit(1);
	}
	sleep(0.1);
	//puts("1");
	if(send(sock , &jeu.client1, sizeof(jeu.client1) , 0)==-1) {
		perror("sendto"); return;//exit(1);
	}
	sleep(0.1);
	//puts("2");
	if(send(sock , &jeu.client2, sizeof(jeu.client2) , 0)==-1) {
		perror("sendto"); return;//exit(1);
	}
	sleep(0.1);
	//puts("3");
	if(send(sock , &jeu.tabP, sizeof(jeu.tabP) , 0)==-1) {
		perror("sendto"); return;//exit(1);
	}
	sleep(0.1);
	//puts("4");
	for(i = 0; i < 10; i++){
		sleep(0.1);
		for(j = 0; j < 10; j++){
			sleep(0.1);
			if(send(sock ,&jeu.tabJeu[i][j], sizeof(jeu.tabJeu[i][j]) , 0)==-1) {
				perror("sendto"); return;//exit(1);
			}
		}
	}
}


Jeu recupererJeu(int s,struct DeplacementUser * du){
	Jeu jeu;
	int i,j;
	if(recv(s , &jeu.client1, sizeof(jeu.client1) , 0)==-1) {
		perror("recv"); return;//exit(1);
	}
	if(recv(s , &jeu.client2, sizeof(jeu.client2) , 0)==-1) {
		perror("recv"); return;//exit(1);
	}
	if(recv(s , &jeu.tabP, sizeof(jeu.tabP) , 0)==-1) {
		perror("recv"); return;//exit(1);
	}
	for(i = 0; i < 10; i++){ //faire comme tabP si le contenant piece est inutile
		for(j = 0; j < 10; j++){
			if(recv(s ,&jeu.tabJeu[i][j], sizeof(jeu.tabJeu[i][j]) , 0)==-1) {
				perror("recv"); return;//exit(1);
			}
			//printf("%d\n",jeu.tabJeu[i]->piece.numero);
		}
	}
	if(recv(s , du, sizeof(jeu.client1) , 0)==-1) { //A tester !!
		perror("recv"); return;//exit(1);
	}
 	//afficher(jeu);
 	return jeu;
}

void misEnPlaceJeu(Jeu *jeu){
	int i, j;
	for(i = 0; i < 10; i++){
		for(j = 0; j < 10; j++){
			if((i == 1 || i == 3 || i == 5 || i == 7 || i == 9)){
				if(j == 1 || j == 3){
					(*jeu).tabJeu[i][j].isuse = 1;
					(*jeu).tabJeu[i][j].piece.numero = 1;
				}
				else if((j == 7 || j == 9)){
					(*jeu).tabJeu[i][j].isuse = 1;
					(*jeu).tabJeu[i][j].piece.numero = 2;
				}
				else{
					(*jeu).tabJeu[i][j].isuse = 2;
					(*jeu).tabJeu[i][j].piece.numero = 0;
				}
			}

			else if(i == 0 || i == 2 || i == 4 || i == 6 || i == 8){
				if((j == 0 || j == 2)){
					(*jeu).tabJeu[i][j].isuse = 1;
					(*jeu).tabJeu[i][j].piece.numero = 1;
				}
				else if((j == 6 || j == 8)){
					(*jeu).tabJeu[i][j].isuse = 1;
					(*jeu).tabJeu[i][j].piece.numero = 2;
				}
				else {
					(*jeu).tabJeu[i][j].isuse = 2;
					(*jeu).tabJeu[i][j].piece.numero = 0;
				}
			}
		}
	}
}


//jeux en cours
void creerJeu(int sock,char*n1,char*n2){ // en lever thread :  //void *jouer(void *cl){
	pthread_mutex_lock((pthread_mutex_t*)&m);
	Jeu jeu;
	misEnPlaceJeu(&jeu);

	//puts("In game");
	clients[getClientIt(n1)].isinGame = 1;
	clients[getClientIt(n2)].isinGame = 1;
	clients[getClientIt(n1)].numJeu = 1;
	clients[getClientIt(n2)].numJeu = 2;

	jeu.client1 = getClient(n1);
	jeu.client2 = getClient(n2);
	jeu.tour = 1;
	jeu.numero = nbJeux;//nbJeux++
	jeux[nbJeux] = jeu;
	nbJeux++;

    printf("Jeu %d créé: j1: %s j2 %s \n",jeu.numero,jeu.client1.nom,jeu.client2.nom);

	//Jeu jeu = getJeu(elClient.nom);
	Client c = getClient(n2);
	envoyerClient(sock,jeu.client1);//update 1 er client (client acceptant)
	envoyerClient(c.addr,jeu.client2);//update 2 eme client (client demandeur )
    envoyerStrucJeu(sock,jeu);//envoie du jeux créér au client acceptant

	//sleep(1);
	pthread_mutex_unlock((pthread_mutex_t*)&m);
	//pthread_exit(NULL);
}

/***************************************************************************************************************************
	THREAD PRINCIPALE
***************************************************************************************************************************/
int socketAutreClientJeu(Jeu jeu,char *nom){
	if(strcmp(jeu.client1.nom,nom) == 0){
		return jeu.client2.addr;
	}else{
		return jeu.client1.addr;
	}
}


void *clientThread(void *socket_c){//struct sockaddr_in src_addr
	int sock = *(int*)socket_c;
	int ret;
	char request[REQUEST_MAX];
	char saut[REQUEST_MAX]=" ";
	char etoile[REQUEST_MAX]="************************************************************";
	char bienv[REQUEST_MAX]="**                   Bienvenue Sur DAMMM  !               **";
	char commandes[REQUEST_MAX] = "Les Commandes :\n\tlist: lister les joueurs connectés\n\tparties : lister parties en cours\n\tjouer: faire une demande de jeu à un joueur\n\tvoir: regarder une partie en cours\n";
	char indic[REQUEST_MAX]="Noter l'action à réaliser :";
	Client elClient;

    elClient = connexion(sock);
    if(send(sock , saut, REQUEST_MAX , 0)==-1) {
							perror("sendto"); return;
	}
    if(send(sock , etoile, REQUEST_MAX , 0)==-1) {
							perror("sendto"); return;
	}
	if(send(sock , etoile, REQUEST_MAX , 0)==-1) {
							perror("sendto"); return;
	}
	if(send(sock , bienv, REQUEST_MAX , 0)==-1) {
							perror("sendto"); return;
	}
	if(send(sock , etoile, REQUEST_MAX , 0)==-1) {
							perror("sendto"); return;
	}
	if(send(sock , etoile, REQUEST_MAX , 0)==-1) {
							perror("sendto"); return;
	}
	if(send(sock , indic, REQUEST_MAX , 0)==-1) {
							perror("sendto"); return;
	}
	if(send(sock , commandes, REQUEST_MAX , 0)==-1) {
							perror("sendto"); return;
	}

    sauv( &clients,sizeof(clients),&nbClient ,FICHIER_CLIENT);

    //afficherTtClients();

   	int codeRet;
	char * resultCom;
	resultCom = (char *)malloc(sizeof(char)*DGRAM_MAX);
 	while(1)
    {  
    	sleep(1);
    	elClient = clients[getClientIt(elClient.nom)];
    	if((ret=recv(sock , request , REQUEST_MAX , 0))==-1) {
	    		perror("recvfrom !");return;// exit(1);
		}
		if(strcmp(request,"")==0){
				//puts("request reçu null");
				if(ret == 0)
			    {
			        puts("Client disconnected");
			        decoClient(elClient.nom);
			        sauv( &clients,sizeof(clients),&nbClient ,FICHIER_CLIENT);
			        fflush(stdout);
			    }
			    else if(ret == -1)
			    {
			        perror("recv failed");
			    }
			  	return;
		}
		if(elClient.isinGame == 1){
			if(strcmp(request,"xjeu") == 0){
				Jeu jeu = getJeu(elClient.nom);
			    envoyerStrucJeu(sock,jeu);
			}else if(strcmp(request,"ojeu") == 0){
				Jeu jeu = getJeu(elClient.nom);
				char ct[REQUEST_MAX]="Tour au joueur adverse";
				char cf[REQUEST_MAX]="Vous etes inconnu et/ou frauduleux";
				char cnom[100];
				int to;
				int  boo = 0;
				char elnom[30];
				if(jeu.tour == 1){ //on verifie que c'est bien son tour (et si c'est bien lui)
					if(strcmp(elClient.nom,jeu.client1.nom)==0){
						to = 2;
						strcpy(elnom, jeu.client1.nom);
					}else{
						printf("(%s) Mauvais Tour\n",elClient.nom);
						if(send(sock , ct, REQUEST_MAX , 0)==-1) {
							perror("sendto"); return;
						}
						boo = 1;
					}
				}else{
					if(strcmp(elClient.nom,jeu.client2.nom)==0){
						to = 1;
						strcpy(elnom, jeu.client2.nom);
					}else{
					    printf("(%s) Mauvais Tour\n",elClient.nom);
						if(send(sock , ct, REQUEST_MAX , 0)==-1) {
							perror("sendto"); return;
						}
						boo = 1;
					}
				}
				if(boo == 0){
					struct DeplacementUser d;
					Jeu j = recupererJeu(sock,&d);
					if(deplacerPion(&jeu, d.numJeu, d.x1, d.y1, d.x2, d.y2)!=1){
						int nn = getJeuInt(j.client1.nom);
						jeux[nn] = jeu;
						jeux[getJeuInt(elClient.nom)].tour = to;
			    		envoyerStrucJeu(socketAutreClientJeu(jeux[getJeuInt(elClient.nom)],elClient.nom),jeux[getJeuInt(elClient.nom)]);
					}else{
						printf("(%s) Frauduleux\n",elClient.nom);
						if(send(sock , cf, REQUEST_MAX , 0)==-1) {
							perror("sendto"); return;
						}
					}
				}

			}else{
				//printf("(%s) Mauvais requete: %s\n",elClient.nom,request);
				//...
			}
		}else if(elClient.demandeDeJeu == 1){
			if(strcmp(request,"oui")==0){//|| request == NULL || sizeof(request) > 1024
			   		//puts(" jouerAvecJoueur ACCEPT ");
			   		clients[getClientIt(elClient.nom)].demandeDeJeu = 0; 
			   		/*
			   		struct source cl;
			   		pthread_t _thread;
			    	strcpy(cl.nom1 ,elClient.nom);
			    	strcpy(cl.nom2,elClient.nomDemande);
			    	if( (tabthread[nbClientCo+1]=pthread_create( &_thread , NULL ,  creerJeu , (void*) &cl) )< 0) //thread inutile ?
			        {
			            perror("could not create thread");
			            return;
			        }*/
			        creerJeu(sock,elClient.nom,elClient.nomDemande);


			}else if(strcmp(request,"non")==0){//|| request == NULL || sizeof(request) > 1024
			   		puts(" jouerAvecJoueur REJECT");
			}else{
				printf("Demande de jeu mauvaise reponse: %s",request);
			}
			clients[getClientIt(elClient.nom)].demandeDeJeu =0;
		}else{
			
	        request[ret]=0;
			// traitement de la requête 
			printf("(%s %d) Commande : %s\n",elClient.nom,elClient.addr, request);//, inet_ntoa(src_addr.sin_addr)
			resultCom = (char *)malloc(sizeof(char)*DGRAM_MAX);
			codeRet = getexec(sock,request, resultCom, strlen(request),elClient);
		}
		memset(request, 0, LINE_MAX);
    }

    decoClient(elClient.nom);//inutile 
    sauv( &clients,sizeof(clients),&nbClient ,FICHIER_CLIENT);//inutile
    //clients[elClient.num].isCo=0;
    //Free the socket pointer
    //free(socket_desc);//
    
    close(sock);
    pthread_exit(NULL); 

}

/***************************************************************************************************************************
	main
***************************************************************************************************************************/
//1) sauvegarde des joureur et des jeux (inscription ...)  ?
//my_struct = malloc(sizeof(t_struct )); -> pour pointeur
int main(int argc, char **argv){
	puts("************************************************************");
	puts("************************************************************");
	puts("**                  Serveur DAMMM :                       **");
	puts("************************************************************");
	puts("************************************************************");
	puts("\nClients : ");
	charge( clients,sizeof(clients),&nbClient ,FICHIER_CLIENT);
	int i;
   	for(i = 0;i<nbClient;i++){
   		clients[i].isinGame = 0;
   		clients[i].demandeDeJeu = 0;
   		clients[i].isCo = 0;
   	}
   	sauv( clients,sizeof(clients),&nbClient ,FICHIER_CLIENT);
   	afficherTtClients();
	puts("************************************************************");
	//m = PTHREAD_MUTEX_INITIALIZER;
	struct sockaddr_in  client;
	int s, ret, client_sock,lg;
	int yes = 1;
	socklen_t len_src_addr;
	//int s, ret;
	struct ip_mreqn mreqn;
	if((s=socket(PF_INET, SOCK_STREAM, 0))==-1) {//AF_INET, SOCK_DGRAM
	   perror("socket"); exit(1);
	}

	if(setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		perror("setting socket option SO_REUSEADDR" );

	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(PORT));
	addr.sin_addr.s_addr = 0;//htonl (INADDR_ANY);
	memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));

	/*
	if(inet_aton(GROUP, &mreqn.imr_multiaddr)==0) {
		fprintf(stderr,"Pb Adr multicast : %s !\n", GROUP);
		exit(1);
	}
	mreqn.imr_address.s_addr = htonl(INADDR_ANY);
	mreqn.imr_ifindex=0; //n'importe quelle interface
	 
	if (setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP, 
			(void *) &mreqn, sizeof(struct ip_mreqn)) < 0)
		perror("setsockopt – IP_ADD_MEMBERSHIP");
	*/
	
	if(bind(s, (struct sockaddr *)&addr, sizeof addr)<0) {
	   perror("bind"); exit(1);
	}
	if(listen(s, 3) == -1)
    {
      perror("listen()");
      exit(errno);
    }

	int *new_sock;

	puts("En Attente de Connexion...");
	while(1)
    {
    	lg=sizeof(client);
    	client_sock = accept(s, (struct sockaddr *)&client,(socklen_t*)&lg);
    	if(client_sock<-1){
    		perror("client_sock accept");
            return 1;
    	}
    	pthread_t _thread;
    	new_sock = malloc(sizeof *new_sock);
  		*new_sock = client_sock;

        printf("Connection Accepté ! \n");//, inet_ntoa(src_addr.client)
    	if( (tabthread[nbClientCo+1]=pthread_create( &_thread , NULL ,  clientThread , (void*) new_sock) )< 0)
        {
            perror("could not create thread");
            return 1;
        } //= _thread;

        int i;
        for(i = 0;i<nbClientCo;i++){
        	pthread_join (tabthread[i], NULL); 
        }
        sleep (1);
	}
	//sauv( void* T,int size ,int *nb, char* nomFichier)
}
