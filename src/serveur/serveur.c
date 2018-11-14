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
	FILE *fe;
	fe=fopen(nomFichier,"wb");
	if ( fe == NULL) {printf("\n pb ouverture %s\n", nomFichier);}
	fwrite(nb,sizeof(int),1,fe);//nb= getlastindex
	fwrite(T,size,TAILLE,fe);//sauvegarde entier du fichier, voir pour augmenter la taille physique
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
   		sleep (1); 
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
		sleep (1); 
	}

	pthread_exit (NULL);
}


/***************************************************************************************************************************
	serveur de base
***************************************************************************************************************************/




//initialisation du serveur
int init(struct sockaddr_in addr){
	int s, ret;
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
      //perror("listen()");
      //exit(errno);
    }
	return s;
}

void fermeture(){

}

void salleAttente(){

}
/***************************************************************************************************************************
	gerer Client
***************************************************************************************************************************/
int verificationConnexion(char *name, char * mdp){
	int i;
   	for(i = 0;i<nbClient;i++){
   		if(strcmp(clients[i].nom, name) == 0 && strcmp(clients[i].mdp, mdp) == 0){
   			clients[i].isCo = 1;
   			return 1;
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
	char el[REQUEST_MAX]="1.connexion\n2.inscription\n3.inviter\n";//noter le numero d'action a réaliser (0 pour retour à ce menu):\n
	char m[REQUEST_MAX]="noter le numero d'action a réaliser\n";
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
    if(send(sock , m , REQUEST_MAX , 0)==-1) {
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
   	/*
   	if(!strcmp(request,"")){
    //.......
	}else if(!strcmp(request,"")){
    //.......
	}else if(!strcmp(request,"")){
    //.......
	}else if(!strcmp(request,"")){
    //.......
    }
	*/

	int i;
   	if((i = atoi(request)) < 0){
   		puts("ERREUT atoi");
		return;
	}
	memset(request, 0, LINE_MAX);
   	switch (i){
   		case 1://connexion
   			puts("1");
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
		   		
		   		printf("nom : %s\n",nom);
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
				   	printf("mdp : %s\n",mdp);
				   	memset(request, 0, LINE_MAX);
					if(verificationConnexion(nom, mdp)==0){
						char mv[REQUEST_MAX]="pb mdp, reesayer (psudo + mdp):";
						if(send(sock , mv , REQUEST_MAX , 0)==-1) {
								perror("sendto"); return;//exit(1);
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
			Client nvClient;
	   		int inscr = 0;
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
		   		printf("request : %s\n",request);
		   		strcpy(nom,request);
		   		
		   		printf("nom : %s\n",nom);
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
				   	printf("mdp : %s\n",mdp);
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
   			//a faire
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
		//puts("jouerAvecJoueur");
		//puts(monnom);
		//puts(nom);
   		for(i = 0;i<nbClient;i++){
   			if(strcmp(nom,clients[i].nom)==0 && strcmp(nom,monnom)!=0){
   				clients[i].socketJDemande = sock;
   				clients[i].demandeDeJeu = 1;
   				strcpy(clients[i].nomDemande,monnom);
   				//puts("send");
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
    //FILE *fp;
    int r;
    puts(command);
	
	memset (response, 0, sizeof (response));
	strcpy(response,"exec !");
   	if(strcmp(command,"list") == 0){
   		char list[1024];
		int i;
		strcat(list,"joueurs :");
		for(i = 0;i<nbClient;i++){
	   		if(clients[i].isCo == 1){
	   				strcat(list,"\n");
	   				strcat(list,clients[i].nom);
	   		}
	   	}
   		strcpy(response,list);
	}else if(strcmp(command,"jouer") == 0){
		char m[REQUEST_MAX] = "avec quel joueur jouer ? : ";
		if(send(sock , m , REQUEST_MAX , 0)==-1) {
							perror("sendto"); return;//exit(1);
		
		}
		if((recv(sock , response , REQUEST_MAX , 0))==-1) {
	    		perror("recvfrom !");return;// exit(1);
		}
		if(strcmp(response,"")==0){//response == NULL || sizeof(response) > 1024 || 
				printf("\nMessage null ! DECONEXION");return;//exit(1);
		}
	    if(jouerAvecJoueur(sock,client.nom,response) == 0){//enlever if
	    	puts("probleme adversaire deco ou null");
	    	//pthread_mutex_unlock((pthread_mutex_t*)&m);
	    	//int res = getexec(sock,command, response, size,client);
	    	char*cc;
    		strcpy(cc,response);
    		strcpy(response,"demande non envoyé a ");
    		strcat(response,cc);
    	}else{
    		char*cc;
	    	strcpy(cc,response);
	    	strcpy(response,"demande envoyé a ");
	    	strcat(response,cc);
    	}
    	//memset (response, 0, sizeof (response));// REPONSE VIDE ............................................
	}else if(strcmp(response,"voir") == 0){
    	char m[REQUEST_MAX] = "quelle partie ? : ";
		if(send(sock , m , REQUEST_MAX , 0)==-1) {
							perror("sendto"); return;//exit(1);
		
		}
		if((recv(sock , response , REQUEST_MAX , 0))==-1) {
	    		perror("recvfrom !");return;// exit(1);
		}
		if(strcmp(response,"")==0){//response == NULL || sizeof(response) > 1024 || 
				printf("\nMessage null ! DECONEXION");return;//exit(1);
		}
		//..........................................................................................................
	   	/*if(voirPartie(sock,client.nom,response) == 0){//enlever if
	   		puts("probleme adversaire deco ou null");
	   		getexec(sock,command, response, size,client) ;
	   		return;
    	}*/
	}else if(strcmp(response,"list parties")==0){
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
	}/*else if(strcmp(request,"")){
    //.......
    }
	memset(response, 0, LINE_MAX);
	*/
  	// Émission du datagramme réponse /
	if(send(sock , response , REQUEST_MAX , 0)==-1) {
							perror("sendto"); return;//exit(1);
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
void afficher(Jeu *jeu){
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
					printf("|x");
				}
				else if((*jeu).tabJeu[i][j].piece.numero == 2) 
					printf("|o");
			}
			else printf("| ");
		}
		printf("|");
	}
	printf("\n");
}

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
	puts("envoie jeu");
	int i,j;
	char c[REQUEST_MAX];
	strcpy(c,"xJeu");

	if(send(sock , c, REQUEST_MAX , 0)==-1) {
		perror("sendto"); return;//exit(1);
	}
	sleep(1);
	//puts("1");
	if(send(sock , &jeu.client1, sizeof(jeu.client1) , 0)==-1) {
		perror("sendto"); return;//exit(1);
	}
	sleep(1);
	//puts("2");
	if(send(sock , &jeu.client2, sizeof(jeu.client2) , 0)==-1) {
		perror("sendto"); return;//exit(1);
	}
	sleep(1);
	//puts("3");
	if(send(sock , &jeu.tabP, sizeof(jeu.tabP) , 0)==-1) {
		perror("sendto"); return;//exit(1);
	}
	sleep(1);
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
	puts("envoie jeu fini");
}

Jeu recupererJeu(int s){
	puts("JEU");
	Jeu jeu;
	int numero;//non uitile
	int i,j;
	Cellule tabJeu[10][10];
	Piece tabP[40];
	Client client1;
	Client client2;
	//puts("1");
	if(recv(s , &jeu.client1, sizeof(jeu.client1) , 0)==-1) {
		perror("recv"); return;//exit(1);
	}
	//puts("2");
	if(recv(s , &jeu.client2, sizeof(jeu.client2) , 0)==-1) {
		perror("recv"); return;//exit(1);
	}
	//puts("3");
	if(recv(s , &jeu.tabP, sizeof(jeu.tabP) , 0)==-1) {
		perror("recv"); return;//exit(1);
	}
	//puts("4");
	for(i = 0; i < 10; i++){ //faire comme tabP si le contenant piece est inutile
		for(j = 0; j < 10; j++){
			if(recv(s ,&jeu.tabJeu[i][j], sizeof(jeu.tabJeu[i][j]) , 0)==-1) {
				perror("recv"); return;//exit(1);
			}
		}
	}

	/*if((recv(s, &jeu , sizeof(jeu) , 0))==-1) {
   		perror("recvfrom !"); exit(1);
   	}*/
   	puts("JEU RECUPERE");
 	afficher(&jeu);
 	return jeu;
}

void envoyerJeu(int sock, Jeu jeu){ //inutile
				char c[REQUEST_MAX];
				int i, j, k;
				strcat(c," ");
				for(k = 0; k < 10; k++)
				{
					strcat(c," ");
					char s[5]; // Nombre maximal de chiffres + 1
			   		sprintf(s, "%d", k);
					strcat(c,s);
					//printf(" %d", k);
				}
				if(send(sock , c, REQUEST_MAX , 0)==-1) {
						perror("sendto"); return;//exit(1);
		
				}
				memset(c, 0, LINE_MAX);
				//strcat(c," ");
				for(k = 0; k < 10; k++)
				{
					//printf(" %d", k);
				}
				//printf("\n");
				//strcat(c,"\n");
				for(i = 0; i < 10; i++){
					char s[5]; // Nombre maximal de chiffres + 1
				   	sprintf(s, "%d", i);
					strcat(c,s);
					//strcat(c,"\n");
					for(j = 0; j < 10; j++){
						if((jeu).tabJeu[i][j].isuse == 1){
							if((jeu).tabJeu[i][j].piece.numero == 1){
								// printf("%d %d", i, j);
								strcat(c,"|x");
							}
							else if((jeu).tabJeu[i][j].piece.numero == 2) 
								strcat(c,"|o");
						}
						else strcat(c,"| ");
					}
					strcat(c,"| ");
					if(send(sock , c, REQUEST_MAX , 0)==-1) {
						perror("sendto"); return;//exit(1);
		
					}
					memset(c, 0, LINE_MAX);
				}	
				/*if(send(sock , charJeu(&jeu), REQUEST_MAX , 0)==-1) {
					perror("sendto"); return;//exit(1);
				}*/
}

void misEnPlaceJeu(Jeu *jeu){
	int i, j;
	// (*jeu).client1 = client1;
	// (*jeu).client2 = client2;

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

//inutile ?
void actionjoueur(Cellule celluleDepart,Cellule celluleArrive){

}


//jeux en cours
void *jouer(void *cl){ // en lever thread
	pthread_mutex_lock((pthread_mutex_t*)&m);
	Jeu jeu;
	misEnPlaceJeu(&jeu);

	struct source s= *(struct source*)cl;//*(source*)cl;

	char n1[30];
	strcpy(n1,s.nom1);
	char n2[30];
	strcpy(n2,s.nom2);

	puts("in game");
	clients[getClientIt(n1)].isinGame = 1;
	clients[getClientIt(n2)].isinGame = 1;

	clients[getClientIt(n1)].numJeu = 1;
	clients[getClientIt(n2)].numJeu = 2;

	puts(n1);
	puts(n2);
	jeu.client1 = getClient(n1);
	jeu.client2 = getClient(n2);
	jeu.tour = 1;
	jeu.numero = nbJeux++;
	jeux[nbJeux] = jeu;
	nbJeux++;

	//while(1){
		/*
	   	if(!strcmp(request,"")){
	    //.......
		}else if(!strcmp(request,"")){
	    //.......
		}else if(!strcmp(request,"")){
	    //.......
		}else if(!strcmp(request,"")){
	    //.......
	    }
		*/
	//}
	sleep(1);
	pthread_mutex_unlock((pthread_mutex_t*)&m);
	pthread_exit(NULL); 
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
	Client elClient;

    elClient = connexion(sock);
    sauv( &clients,sizeof(clients),&nbClient ,FICHIER_CLIENT);
    //puts("sauv");
    afficherTtClients();
   	//nbClientCo++;
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
				puts("request reçu null");
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
		char ss[5]; // Nombre maximal de chiffres + 1
					sprintf(ss, "%d", elClient.isinGame);
					puts(elClient.nom);
					puts(ss);
		if(elClient.isinGame == 1){
			if(strcmp(request,"jeu") == 0){
				Jeu jeu = getJeu(elClient.nom);
				envoyerJeu(sock,jeu);
		   		//strcpy(response,list);
			}else if(strcmp(request,"xjeu") == 0){
				Jeu jeu = getJeu(elClient.nom);
			    envoyerStrucJeu(sock,jeu);
			}else if(strcmp(request,"ojeu") == 0){
				puts("ojeu1");
				Jeu jeu = getJeu(elClient.nom);
				char ct[REQUEST_MAX]="Tour au joueur adverse";
				if(jeu.tour == 1){ //on verifie que c'est bien son tour
					if(strcmp(elClient.nom,jeu.client1.nom)==0){
						jeux[getJeuInt(elClient.nom)] = recupererJeu(sock);
						jeux[getJeuInt(elClient.nom)].tour = 2;
			    		envoyerStrucJeu(socketAutreClientJeu(jeux[getJeuInt(elClient.nom)],elClient.nom),jeux[getJeuInt(elClient.nom)]);///jeux[getJeuInt(elClient.nom)].client2.addr
					//..........................................................................................................
					}else{
						puts("Mauvais Tour");
						if(send(sock , ct, REQUEST_MAX , 0)==-1) {
							perror("sendto"); return;//exit(1);
						}
					}
				}else{
					if(strcmp(elClient.nom,jeu.client2.nom)==0){
						jeux[getJeuInt(elClient.nom)] = recupererJeu(sock);
						jeux[getJeuInt(elClient.nom)].tour = 1;
			    		envoyerStrucJeu(socketAutreClientJeu(jeux[getJeuInt(elClient.nom)],elClient.nom),jeux[getJeuInt(elClient.nom)]);		
			    		//..........................................................................................................				
					}else{
						puts("Mauvais Tour");
						if(send(sock , ct, REQUEST_MAX , 0)==-1) {
							perror("sendto"); return;//exit(1);
						}
					}
				}
			}else{
				//...
				puts("Mauvaise requete");
			}
		}else if(elClient.demandeDeJeu == 1){//jouerAvecJoueur(elClient.socketJDemande,monnom,nom);
			if(strcmp(request,"oui")==0){//|| request == NULL || sizeof(request) > 1024
			   		puts(" jouerAvecJoueur ACCEPT ");

			   		clients[getClientIt(elClient.nom)].demandeDeJeu = 0; 
			   		struct source cl;

			   		pthread_t _thread;
			    	strcpy(cl.nom1 ,elClient.nom);
			    	strcpy(cl.nom2,elClient.nomDemande);

					char ss[5]; // Nombre maximal de chiffres + 1
					sprintf(ss, "%d", elClient.isinGame);
					puts(elClient.nomDemande);
					puts(ss);
			    	if( (tabthread[nbClientCo+1]=pthread_create( &_thread , NULL ,  jouer , (void*) &cl) )< 0) //thread inutile ?
			        {
			            perror("could not create thread");
			            return;
			        } //= _thread;
			        pthread_join (_thread, NULL); 
			        Jeu jeu = getJeu(elClient.nom);
			        Client c = getClient(elClient.nomDemande);
			        envoyerClient(sock,jeu.client1);
			        envoyerClient(c.addr,jeu.client2);
			        //afficher(&jeu);
			        envoyerStrucJeu(sock,jeu);

			}else if(strcmp(request,"non")==0){//|| request == NULL || sizeof(request) > 1024
			   		puts(" jouerAvecJoueur REJECT");
			}else{
				puts("(oui / non) :");
				puts(request);
			}
			clients[getClientIt(elClient.nom)].demandeDeJeu =0;
		}else{
			
	        request[ret]=0;
			// traitement de la requête 
			printf("(%s) Commande : %s\n",elClient.nom, request);//, inet_ntoa(src_addr.sin_addr)
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
	puts("============================================");
	puts("============================================");
	puts("\tServeur DAMMM :");
	puts("============================================");
	puts("============================================");
	puts("\nClients : ");
	charge( clients,sizeof(clients),&nbClient ,FICHIER_CLIENT);
	afficherTtClients();

	int i;
   	for(i = 0;i<nbClient;i++){
   		clients[i].isinGame = 0;
   		clients[i].demandeDeJeu = 0;
   		clients[i].isCo = 0;
   	}
   	sauv( clients,sizeof(clients),&nbClient ,FICHIER_CLIENT);
	puts("============================================\n");
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

	// charge(void*T,int size, int *nb, char* nomFichier) //chargement des ancienne données (uesr,...)


	puts("Waiting for incoming connections...");
	while(1)
    {
    	lg=sizeof(client);
    	//puts("Connection accepted1\n");
    	client_sock = accept(s, (struct sockaddr *)&client,(socklen_t*)&lg);
    	//puts("Connection accepted2\n");
    	if(client_sock<-1){
    		perror("client_sock accept");
            return 1;
    	}
    	pthread_t _thread;
    	
    	new_sock = malloc(sizeof *new_sock);
  		*new_sock = client_sock;

		puts("Connection Accepté ! ");

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
   		/*if(ifaddrClientCo(client_sock) == 0){
   		}else{
   		}*/
   		
  		//request[ret]=0;
  		//printf("%s\n", request);
	}


	//sauv( void* T,int size ,int *nb, char* nomFichier)

}
