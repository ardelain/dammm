#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <sys/socket.h>
#include <errno.h>

/***************************************************************************************************************************
	struct
***************************************************************************************************************************/
typedef struct
{
	struct sockaddr_in addr;
	int numero;
	char nom[30];
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
	struct Piece piece;//si = null pas de piece presente
	int isuse;
	int x;
	int y
}
Cellule;


/***************************************************************************************************************************
	Variable grobale
***************************************************************************************************************************/
#define LINE_MAX 1024  /* taille MAX en réception */
#define GROUP   "230.0.0.0"
#define PORT	"5000"

Cellule jeux[];
Client clients;
/***************************************************************************************************************************
	main
***************************************************************************************************************************/
int main(int argc, char **argv){
	struct sockaddr_in dst_addr;	
	struct hostent * hostent;
	init(dst_addr,hostent);
	connexion();
	while(1){
		while (strcmp(msg,"quit") !=0 ){
		scanf("%s",&msg);
		if(msg == NULL){
				printf("\nMessage null !");exit(1);
		}
		if(sendto(s, msg, strlen(msg)+1, 0, \
		   (struct sockaddr*) &dst_addr, sizeof(dst_addr))==-1) {
			perror("sendto"); exit(1);
		}
	}
	}
}
/***************************************************************************************************************************
	connection
***************************************************************************************************************************/
void init(struct sockaddr_in dst_addr,struct hostent * hostent){
	struct sockaddr_in dst_addr;	
	struct hostent * hostent;
	int s, ret;
	socklen_t len_src_addr;
 
	char response[LINE_MAX];
 
	char msg[]=" ";


	if((s=socket(AF_INET, SOCK_DGRAM, 0))==-1) {
		perror("socket"); exit(1);
	}

	if((hostent=gethostbyname(GROUP))==NULL) {
		herror("gethostbyname"); exit(1);
	}

	dst_addr.sin_family = AF_INET;
	dst_addr.sin_port = htons(atoi(PORT));  
	dst_addr.sin_addr = *((struct in_addr *)hostent->h_addr);
	memset(dst_addr.sin_zero, '\0', sizeof(dst_addr.sin_zero));
	
	int ttl=2;
	if(setsockopt(s, IPPROTO_IP, IP_MULTICAST_TTL, 
		(void *) &ttl, sizeof(ttl)) == -1)
	   perror("set IP_MULTICAST_TTL");

}

void connexion(){

}

void deconnexion(){

}
/***************************************************************************************************************************
	interaction
***************************************************************************************************************************/
void listerJoueurDiponible(){

}

//
void listerPartie(){
	
}

//
void jouerAvecJoueur(int joueur){
	
}

//
void voirpartie(int partie){
	
}
/***************************************************************************************************************************
	jeu
***************************************************************************************************************************/

void jeu(){
	
}

void recupererplateau(){

}

void actionjoueur(Cellule celluleDepart,Cellule celluleArrive){

}

