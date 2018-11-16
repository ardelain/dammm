#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>      
#include <string.h>     /* pour memset */
#include <ctype.h>      /* pour toupper */
#include <arpa/inet.h>  /* pour inet_ntop */
#include <sys/types.h> 
#include <semaphore.h>
#include <pthread.h>
/***************************************************************************************************************************
	Structure :
***************************************************************************************************************************/
typedef struct Client
{
//struct sockaddr_in addr;//?
	int addr;
	int isCo ; //si le client est connecté
	int isinGame;//en jeu ou spectaur
	int demandeDeJeu;//1 si demande de jeu en cours
	int socketJDemande;//socket du joueur qui a demandé de jouer
	char nomDemande[30];//nom du joueur qui demande de jouer
	int numero;
	int numJeu;
	int isInvite;//si c'est un invite = 1
	int isSp;//si c'est un spectateur = 1
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
	int tour;//a qui est le tour
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
	int isAuto;
};

/***************************************************************************************************************************
	Variable grobale :
***************************************************************************************************************************/
#define LINE_MAX 1024  /* taille MAX en réception */
#define REQUEST_MAX 1024  /* taille MAX en réception */
#define GROUP  "127.0.0.1"// "230.0.0.0"/
#define PORT	"5000" 	

//Cellule jeux[100];
Client client;
Jeu jeu;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

/***************************************************************************************************************************
	Connection :
***************************************************************************************************************************/

//initialisation du stream socket
int init(struct sockaddr_in dst_addr,struct hostent * hostent){
	int s, ret,yes = 1,lg;
	socklen_t len_src_addr;
	char response[LINE_MAX];
	char msg[]=" ";
	if((s=socket(PF_INET, SOCK_STREAM, 0))==-1) {
		perror("socket"); exit(1);
	}
	printf("Noter l'addresse ip où joindre le Serveur:");
	char ip[64];
	scanf("%s",&ip);
	if((hostent=gethostbyname(ip))==NULL) {
		herror("gethostbyname"); exit(1);
	}
	dst_addr.sin_family = AF_INET;
	dst_addr.sin_port = htons(atoi(PORT));  
	if (!inet_aton((const char*)ip,&dst_addr.sin_addr)){
		perror("adresse");	
	}
	memset(dst_addr.sin_zero, '\0', sizeof(dst_addr.sin_zero));
	int ttl=2;
	if(setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		perror("setting socket option SO_REUSEADDR" );
	lg=sizeof(dst_addr);
    if (connect(s, (struct sockaddr *)&dst_addr,(socklen_t)lg))
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 
	return s;
}

/***************************************************************************************************************************
	Jeu :
***************************************************************************************************************************/

//afficher le jeu donné en parametre
void afficher(Jeu jeu){
	int i, j, k;
	printf("\t   ");
	for(k = 0; k < 10; k++)
	{
		printf(" %d", k);
	}
	printf("\n");
	for(i = 0; i < 10; i++){
		printf("\n\t%d  ", i);
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

//return le nombre de pion possedé par un client
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

//return 0 si le pion est possedé par le joueur ayant comme numero (dans le jeu) numC
int rechercherPionJoueur(Jeu *jeu, int x, int y, int numC){
	int i;
	if(((*jeu).tabJeu[x][y].isuse == 1) && ((*jeu).tabJeu[x][y].piece.numero == numC)){
		return 0;
	}
	else if (((*jeu).tabJeu[x][y].isuse == 1) && ((*jeu).tabJeu[x][y].piece.numero != numC))
	{
		return 2;
	}
	return 1;
}

//retourne 1 si le pion n'est pas sur le plateau
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

//permet de selectionner la position en abscisse d'une case
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

//permet de selectionner l'ordonnée en abscisse d'une case
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

//permet de deplacer un pion
int deplacerPion(Jeu *jeu, Client c,struct DeplacementUser * du){
	int x1=0, y1=0, x2, y2, *x3, *y3, p, d, i;

	printf("\nChoisissez un pion a déplacer :\n");
	x1=choisirPositionGauche();
	y1=choisirPositionDroite();

	p=rechercherPionPlateau(jeu, x1, y1, c.numJeu);
	if(p == 1){
		printf("\nErreur Jeton hors du plateau\n");
	}

	p=rechercherPionJoueur(jeu, x1, y1, c.numJeu);
	if(p == 0){
		printf("\nChoisissez une case de destination :\n");
		x2=choisirPositionGauche();
		y2=choisirPositionDroite();

		p=rechercherPionPlateau(jeu, x2, y2, c.numJeu);
		if(p == 1){
			printf("\nErreur Jeton hors du plateau\n");
			return 1;
		}
		p=rechercherPionJoueur(jeu, x2, y2, c.numJeu);
		if(p == 1){
			d=deplacement(c.numJeu, x1, x2, y1, y2);
			if(d == 1){
				printf("\nDéplacement non valide !\n\n");
				return 1;
			}
			(*jeu).tabJeu[x1][y1].isuse = 2;
			(*jeu).tabJeu[x1][y1].piece.numero = 0;
			(*jeu).tabJeu[x2][y2].isuse = 1;
			(*jeu).tabJeu[x2][y2].piece.numero = c.numJeu;
			(*du).numJeu=c.numJeu;
			(*du).x1=x1;
			(*du).y1=y1;
			(*du).x2=x2;
			(*du).y2=y2;
			return 0;
		}
		else if(p == 2){
			p=caseIsLibreManger(jeu, c.numJeu, x1, x2, y1, y2, &x3, &y3);
			if(p == 0){
				manger(jeu, c.numJeu, x1, x2, y1, y2, &x3, &y3);
				(*du).numJeu=c.numJeu;
				(*du).x1=x1;
				(*du).y1=y1;
				(*du).x2=x2;
				(*du).y2=y2;
				return 0;
			}
			else{
				printf("\nDéplacement impossible !\n");
				return 1;
			}
		}
		else{
			printf("\nCette case est occupée !\n\n");
			return 1;
		}
	}
	else{
		printf("Ce n'est pas votre pion !\n");
		return 1;
	} 
}

//permet de manger un pion
void manger(Jeu *jeu, int numC, int x1, int x2, int y1, int y2, int *x3, int *y3){
	(*jeu).tabJeu[x1][y1].isuse = 2;
	(*jeu).tabJeu[x1][y1].piece.numero = 0;
	(*jeu).tabJeu[x2][y2].isuse = 2;
	(*jeu).tabJeu[x2][y2].piece.numero = 0;
	(*jeu).tabJeu[*x3][*y3].isuse = 1;
	(*jeu).tabJeu[*x3][*y3].piece.numero = numC;
}

//permet  de manger automatiquement un pion si c'est possible
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

//permet le deplacement lors d'un manger automatique
int deplacerAuto(Jeu *jeu, Client c,struct DeplacementUser * du){
	int i, j, p, x1, x2, y1, y2;
	for(i = 0; i < 10; i++){
		for(j = 0; j < 10; j++){
			if((*jeu).tabJeu[i][j].isuse == 1 && (*jeu).tabJeu[i][j].piece.numero == c.numJeu){
				x1 = i;
				y1 = j;
				x2 = x1-1;
				y2 = y1+1;
				p=mangerAuto(jeu, x1, x2, y1, y2, c.numJeu);
				(*du).numJeu=c.numJeu;
				(*du).x1=x1;
				(*du).y1=y1;
				(*du).x2=x2;
				(*du).y2=y2;
				if (p == 0)
				{
					return 0;
				}
				x2 = x1+1;
				y2 = y1+1;
				p=mangerAuto(jeu, x1, x2, y1, y2, c.numJeu);
				(*du).numJeu=c.numJeu;
				(*du).x1=x1;
				(*du).y1=y1;
				(*du).x2=x2;
				(*du).y2=y2;
				if (p == 0)
				{
					return 0;
				}
				x2 = x1-1;
				y2 = y1-1;
				p=mangerAuto(jeu, x1, x2, y1, y2, c.numJeu);
				(*du).numJeu=c.numJeu;
				(*du).x1=x1;
				(*du).y1=y1;
				(*du).x2=x2;
				(*du).y2=y2;
				if (p == 0)
				{
					return 0;
				}
				x2 = x1+1;
				y2 = y1-1;
				p=mangerAuto(jeu, x1, x2, y1, y2, c.numJeu);
				(*du).numJeu=c.numJeu;
				(*du).x1=x1;
				(*du).y1=y1;
				(*du).x2=x2;
				(*du).y2=y2;
				if (p == 0)
				{
					return 0;
				}
			}

		}
	}
	return 1;
}

/***************************************************************************************************************************
	Interaction et echange avec serveur :
***************************************************************************************************************************/

//permet de recuperer par socket une structure client
void recupererClients(int s){
	if(recv(s , &client, sizeof(client) , 0)==-1) {
		perror("recv"); return;//exit(1);
	}
}

//permet de recuperer par socket les différentes information du jeu
void recupererJeu(int s){
	int numero;//non uitile
	int i,j;
	Cellule tabJeu[10][10];
	Piece tabP[40];
	Client client1;
	Client client2;
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
		}
	}
 	afficher(jeu);
}

//permet d'envoyer au serveur par socket les différentes information du jeu
void envoyerStrucJeu(int sock, Jeu jeu,struct DeplacementUser du){
	int i,j;
	char c[REQUEST_MAX];
	strcpy(c,"ojeu");
	if(send(sock , c, REQUEST_MAX , 0)==-1) {
		perror("sendto"); return;//exit(1);
	}
	sleep (1);
	if(send(sock , &jeu.client1, sizeof(jeu.client1) , 0)==-1) {
		perror("sendto"); return;//exit(1);
	}
	sleep (1);
	if(send(sock , &jeu.client2, sizeof(jeu.client2) , 0)==-1) {
		perror("sendto"); return;//exit(1);
	}
	sleep (1);
	if(send(sock , &jeu.tabP, sizeof(jeu.tabP) , 0)==-1) {
		perror("sendto"); return;//exit(1);
	}
	sleep (1);
	for(i = 0; i < 10; i++){
		for(j = 0; j < 10; j++){
			if(send(sock ,&jeu.tabJeu[i][j], sizeof(jeu.tabJeu[i][j]) , 0)==-1) {
				perror("sendto"); return;//exit(1);
			}
			sleep(0.5);
		}
	}
	sleep (1);
	if(send(sock , &du, sizeof(du) , 0)==-1) {
		perror("sendto"); return;//exit(1);
	}
}

//permet d'écouter les differentes informations reçu par socket et de réaliser les actions correspondante (acceptation demande de jeu, update client coté serveur,...)
void *ecouter(void *sock){
	int lg,ret,choix = 1;
	int s= *(int*)sock;
	char msg[LINE_MAX];
	char request[LINE_MAX];
	int boo=0;
	while(1){
	    boo=0;
		if((ret=recv(s, request , LINE_MAX , 0))==-1) {
    			perror("recvfrom !"); exit(1);
   		}
   		pthread_mutex_lock((pthread_mutex_t*)&m);
   		if(request == NULL || sizeof(request) > LINE_MAX || strcmp(request,"") ==0){
				printf("\nMessage null ! DOCONEXION");exit(1);
				exit(0);
		}
		if( strncmp(request," (oui/non)",10) == 0){ //pour debloquer la boucle principale du serveur
			memset(msg, 0, LINE_MAX);
			strcpy(msg,"TEST1");
			if(send(s , msg , LINE_MAX , 0)==-1) {
				perror("sendto"); exit(1);
				boo = 1;
			}
		}
		if( strncmp(request,"client",10) == 0){ //pour debloquer la boucle principale du serveur
			recupererClients(s);
			memset(request, 0, LINE_MAX);
			boo = 1;
		}
		
		if( strncmp(request,"reception demande de jeu",10) == 0){//pour debloquer la boucle principale du serveur
			memset(msg, 0, LINE_MAX);
			strcpy(msg,"TEST2");
			if(send(s , msg , LINE_MAX , 0)==-1) {
				perror("sendto"); exit(1);
			}
			memset(request, 0, LINE_MAX);
			boo = 1;
		}
		//reception de la structure Jeu quand on est spectateur
		if( strncmp(request,"spJeu",10) == 0){
			recupererJeu(s);
			afficher(jeu);
			boo = 1;
		}
		//reception de la structure Jeu quand on est joueur
		if( strncmp(request,"xJeu",10) == 0){
			recupererJeu(s);
			if(strcmp(client.nom,jeu.client1.nom)==0){
				printf("\n\t   (Vous êtes les x)\n");
			}else{
				printf("\n\t   (Vous êtes les o)\n");
			}
			choix = 1;
			struct DeplacementUser du;
			while(choix != 0){
				choix = deplacerAuto(&jeu, client,&du);
				if(choix == 0){
					du.isAuto=1;
					printf("\nDéplacement automatique effectué car vous pouviez manger un pion !\n");
				}else{
					choix = deplacerPion(&jeu, client,&du);
					du.isAuto=0;
				}
			}
			choix = 1;
			afficher(jeu);
			
			if(client.numJeu == 1){
				jeu.tour = 2;
			}else{
				jeu.tour = 1;
			}
			memset(msg, 0, LINE_MAX);
			strcpy(msg,"ELTEST");
			if(send(s , msg , LINE_MAX , 0)==-1) {//pour update le client du serveur
				perror("sendto"); exit(1);
			}
			envoyerStrucJeu(s,jeu,du);
			puts("envoyé");
			memset(request, 0, LINE_MAX);
			boo = 1;
		}
        if(boo ==0){
            printf("%s\n",request);
            boo=0;
        }else{
            //printf("Donnees Reçu !\n");
        }
   		//clear the message buffer
		memset(request, 0, LINE_MAX);
   		sleep (0.1);
   		pthread_mutex_unlock((pthread_mutex_t*)&m);

	}
	pthread_exit (NULL);
}

//permet d'emmettre une reponse texte au serveur reçu par socket
void *emettre(void *sock){
	int lg,ret;
	int s= *(int*)sock;
	char msg[LINE_MAX];
	int boo =0;
	while(1 && strcmp(msg,"quit") !=0 ){
		scanf("%s",&msg);
		//puts("emmission");
		pthread_mutex_lock((pthread_mutex_t*)&m);
		if(msg == NULL || sizeof(msg) > LINE_MAX || strcmp(msg,"") ==0){
				printf("\nMessage null !");exit(1);
		}
		if(strcmp(msg,"help") ==0){
				printf("Commande :\nlist: lister les joueurs connectés\nparties : lister parties en cours\njouer: faire une demande de jeu à un joueur\nvoir: regarder une partie en cours\n");
				boo =1;
		}
		/*if(strcmp(msg,"List") !=0 &&strcmp(msg,"parties") !=0 &&strcmp(msg,"jouer") !=0 &&strcmp(msg,"voir") !=0 ){
				printf("Mauvaise Commande");
				boo =1;
		}*/
		if(boo ==0){
           if(send(s , msg , LINE_MAX , 0)==-1) {
			perror("sendto"); exit(1);
			}
            boo=0;
            printf("************************************************************\n");
        }
		memset(msg, 0, LINE_MAX);
		sleep (0.1);
		pthread_mutex_unlock((pthread_mutex_t*)&m);
   		sleep (0.1);
	}

	pthread_exit (NULL);
}


/***************************************************************************************************************************
	Main :
***************************************************************************************************************************/
int main(int argc, char **argv){
	int s,lg,ret;
	struct sockaddr_in dst_addr;	
	struct hostent * hostent;
	char msg[]="";
	char request[1024];
	s = init(dst_addr,hostent);
	pthread_t thread_1,thread_2;
	//while(1){
	int *sock;
    sock = malloc(sizeof *sock);
  	*sock = s;
	
	if( pthread_create( &thread_1 , NULL ,  ecouter , (void*) sock) < 0)
     {
         perror("could not create thread");
         return 1;
    }
      
	if( pthread_create( &thread_2 , NULL ,  emettre , (void*) sock) < 0)
    {
       perror("could not create thread");
       return 1;
     }
	//g_flush();
    pthread_join (thread_2, NULL); 
    pthread_join (thread_1, NULL); 
    close(s);
    printf("FIN\n");
}