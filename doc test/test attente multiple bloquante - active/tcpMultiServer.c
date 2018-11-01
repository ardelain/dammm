#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define PORT 2020

int main(int argc, char *argv[])
{
	int sock_cnx;        /* Socket pour ouverture de connexion */
	struct sockaddr_in serveraddr;
	struct sockaddr_in clientaddr;
	int newfd;
	char buf[1024];
	int nbytes;
	int addrlen;
	int i, j;
	fd_set surveil_fds;  /* Ensemble des descripteurs qu’on souhaite surveiller en lecture */
	fd_set read_fds;     /* Ensemble des descripteurs qu’on va utiliser dans SELECT */
	int fdmax;           /* Memorise le plus grand descripteur : `a utiliser dans SELECT*/
	
	addrlen = sizeof(clientaddr);
	/* Ouverture de la socket du serveur */
	if ((sock_cnx = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Erreur socket");
		exit(1);
	}
	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(atoi(argv[1]));
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	bzero(&(serveraddr.sin_zero), 8);
	
	/* Bind */
	if (bind(sock_cnx, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1) {
		perror("Erreur Bind");
		exit(1);
	}
	
	/* Ecoute sur la socket, on autorise jusqu’a 10 requetes clients en attente */
	if (listen(sock_cnx, 10) == -1) {
		perror("Erreur Listen");
		exit(1);
	}
	
	/* On initialize surveil_fds comme  ́etant un ensemble vide */
	FD_ZERO(&surveil_fds);
	/* Positionne le bit associ ́e `a soc_cnx a 1 */
	/* Grace `a SELECT on pourra surveiller en ecoute soc_cnx sans `a voir `a utiliser la primitive bloquante accept */
	FD_SET(sock_cnx, &surveil_fds);
	fdmax = sock_cnx;
	while (1){
		read_fds = surveil_fds;
		if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1)
		{
			perror("Erreur select");
			exit(1);
		}
		/* Nous allons parcourir l’ensemble des descripteurs pour savoir qui a d ́ebloqu ́e select */
		for (i = 0; i <= fdmax; i++){
			if (FD_ISSET(i, &read_fds)){
				/* si le descripteur qui a debloqu ́e select est soc_cnx */
				/* alors on accepte l’ouvertur d’une connexion avec le nouveau client */
				if (i == sock_cnx){
					if ((newfd = accept(sock_cnx, (struct sockaddr *)&clientaddr, (int *)&addrlen)) == -1)
						perror("Erreur accept");
					else{
						/* On ajoute le nouveau descripteur dans l’ensemble des descripteurs */
						FD_SET(newfd, &surveil_fds);
						/* On met `a jour la valeur du plus grand descripteur */
						if (newfd > fdmax) fdmax = newfd;
						printf("%s: Nouvelle connexion de %s \n", argv[0], inet_ntoa(clientaddr.sin_addr));
					}
				}
				/* Sinon c’est un ecriture d’un client sur la socket associ ́e au descripteur i*/
				/* Auquel cas, on va lire ce message et l’envoyer sur toutes les autres sockets */
				/* except ́e sur sock_cnx et sur i */
				else{
					if ((nbytes = recv(i, buf, sizeof(buf), 0)) <= 0){
						/* si nbytes = 0 c’est que le client a ferm ́e la connexion */
						/* si nbytes < 0 alors erreur d’ ́ecriture sur la socket */
						/* dans les deux cas on ferme la connexion */
						close(i);
						/* on enleve le descripteur i de l’ensemble des descripteurs */
						FD_CLR(i, &surveil_fds);
					}
					else{
						for (j = 0; j <= fdmax; j++)
						if (FD_ISSET(j, &surveil_fds) &&  (j != sock_cnx) && (j != i))
						if (send(j, buf, nbytes, 0) == -1)
						perror("Erreur send");
					}
				}
			}
		}
	}
	return 0;
}
