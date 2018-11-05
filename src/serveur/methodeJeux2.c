#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <sys/socket.h>
#include <errno.h>

typedef struct Client
{
	int isCo; //si le client est connecté
	int numero;
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
	int isuse;
}
Cellule;

typedef struct
{
	int numero;
	struct Cellule tabJeu[10][10];
	struct Piece tabP[40];
}
Jeu;

void main(int argc, char const *argv[])
{
	Client client1, client2;
	Jeu jeu;
	int nbPionJ1 = 0;
	int nbPionJ2 = 0;

	printf("Veuillez entrer votre nom :\n");
	scanf("%s", client1.nom);
	printf("Veuillez entrer votre mot de passe :\n");
	scanf("%s", client1.mdp);
	client1.numero = 1;
	client1.isCo = 1;

	printf("Veuillez entrer votre nom :\n");
	scanf("%s", client2.nom);
	printf("Veuillez entrer votre mot de passe :\n");
	scanf("%s", client2.mdp);
	client2.numero = 2;
	client2.isCo = 1;

	misEnPlaceJeu(client1, client2, &jeu);

	afficher(&jeu);

	// for (int i = 0; i < 40; i++)
	// {
	// 	printf("%d ", jeu.tabP[i].x);
	// 	printf("%d\n", jeu.tabP[i].y);
	// 	printf("-----\n");
	// }

	nbPionJ1 = nbPionClient(&jeu, client1);
	nbPionJ2 = nbPionClient(&jeu, client2);

	if(nbPionJ1 != 0 || nbPionJ2 != 0){

		deplacerPion(&jeu, client1);
	}

	afficher(&jeu);

	return 0;
}

void misEnPlaceJeu(/*Client client1, Client client2, */Jeu *jeu){

	int i, j, k=0;
	// (*jeu).client1 = client1;
	// (*jeu).client2 = client2;

	for(i = 0; i < 10; i++){
		for(j = 0; j < 10; j++){

			if((i == 0 || i == 2 || i == 4 || i == 6 || i == 8)){
				if(j == 1 || j == 3){
					(*jeu).tabJeu[i][j].isuse = 1;
					(*jeu).tabP[k].numero = 1;
					(*jeu).tabP[k].x = i;
					(*jeu).tabP[k].y = j;
					k++;
				}
				else if((j == 7 || j == 9)){
					(*jeu).tabJeu[i][j].isuse = 1;
					(*jeu).tabP[k].numero = 2;
					(*jeu).tabP[k].x = i;
					(*jeu).tabP[k].y = j;
					k++;
				}
				else (*jeu).tabJeu[i][j].isuse = 0;
			}

			else if(i == 1 || i == 3 || i == 5 || i == 7 || i == 9){
				if((j == 0 || j == 2)){
					(*jeu).tabJeu[i][j].isuse = 1;
					(*jeu).tabP[k].numero = 1;
					(*jeu).tabP[k].x = i;
					(*jeu).tabP[k].y = j;
					k++;
				}
				else if((j == 6 || j == 8)){
					(*jeu).tabJeu[i][j].isuse = 1;
					(*jeu).tabP[k].numero = 2;
					(*jeu).tabP[k].x = i;
					(*jeu).tabP[k].y = j;
					k++;
				}
				else (*jeu).tabJeu[i][j].isuse = 0;
			}
		}
	}

	// for(i = 0; i < 10; i++){
	// 	for(j = 0; j < 10; j++){
	// 		(*jeu).tabJeu[i][j].x = i;
	// 		(*jeu).tabJeu[i][j].y = j;
	// 	}
	// }
}

void afficher(Jeu *jeu){

	int i, j, k;

	for(k = 0; k < 10; k++)
	{
		printf(" %d", k);
	}
	printf("\n");

	for(i = 0; i < 10; i++){
		for(j = 0; j < 10; j++){
			if((*jeu).tabJeu[i][j].isuse == 1){

				for (k = 0; k < 40; k++)
				{
					if(((*jeu).tabP[k].x == i) && ((*jeu).tabP[k].y == j)){
						if((*jeu).tabP[k].numero == 1) 
							printf("|x");
						else if((*jeu).tabP[k].numero == 2) 
							printf("|o");
					}
				}

				// if((*jeu).tabJeu[j][i].piece.numero == 1) 
				// 	printf("|x");
				// else if((*jeu).tabJeu[j][i].piece.numero == 2) 
				// 	printf("|o");
			}
			else printf("| ");
		}
		printf("|%d\n", i);
	}
}

int nbPionClient(Jeu *jeu, Client c){

	int nbPion = 0;
	int i, j;

	for(i = 0; i < 40; i++){
		if((*jeu).tabP[i].numero == c.numero){
			nbPion = nbPion + 1;
		}
	}

	return nbPion;
}

int rechercherPion(Jeu *jeu, int x, int y, int numC){

	int i;

	for (i = 0; i < 40; i++){
		// printf("%d ", (*jeu).tabP[i].x);
		// printf("%d\n", (*jeu).tabP[i].y);
		if(((*jeu).tabP[i].x == x) && ((*jeu).tabP[i].y == y)){
			if((*jeu).tabP[i].numero == numC){
				return 0;
			}
			else
				return 1;
		}
	}
	return 1;
}

void deplacerPion(Jeu *jeu, Client c){

	int x, y, p;

	printf("Choisissez un pion a déplacer :\nDonnez moi sa position x (abscisse) :\n");
	scanf("%d", &y);
	printf("Choisissez un pion a déplacer :\nDonnez moi sa position y (ordonnée) :\n");
	scanf("%d", &x);

	p=rechercherPion(jeu, x, y, c.numero);
	if(p == 0){
		printf("Choisissez un pion a déplacer :\nDonnez moi sa position x (abscisse) :\n");
		scanf("%d", &y);
		printf("Choisissez un pion a déplacer :\nDonnez moi sa position y (ordonnée) :\n");
		scanf("%d", &x);

		p=rechercherPion(jeu, x, y, c.numero);
		if(p == 1){
			(*jeu).tabP[29].x = x;
			(*jeu).tabP[29].y = y;
		}
	}
}