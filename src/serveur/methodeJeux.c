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
}
Piece;

typedef struct Cellule
{
	struct Piece piece;
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

	int nbPJ1, nbPJ2, choix = 1;

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

	nbPJ1=nbPionClient(&jeu, client1);
	nbPJ2=nbPionClient(&jeu, client2);

	afficher(&jeu);

	while(nbPJ1 != 0 || nbPJ2 != 0){
		while(choix != 0){

			choix = deplacerAuto(&jeu, client1);
			if(choix == 0){
				printf("\nDéplacement automatique effectué car vous pouviez manger un pion !\n");
			}

			else{
				choix = deplacerPion(&jeu, client1);
			}
		}
		choix = 1;
		afficher(&jeu);

		// while(choix != 0){
		// 	choix = deplacerPion(&jeu, client1);
		// }
		// choix = 1;
		// afficher(&jeu);

		while(choix != 0){
			choix = deplacerPion(&jeu, client2);
		}
		choix = 1;
		afficher(&jeu);
	}

	return 0;
}

void misEnPlaceJeu(/*Client client1, Client client2, */Jeu *jeu){

	int i, j;
	// (*jeu).client1 = client1;
	// (*jeu).client2 = client2;

	for(i = 0; i < 10; i++){
		for(j = 0; j < 10; j++){

			// //s'il y a une pièce du joueur 1
			// if((j == 1 || j == 3) && (i == 0 || i == 2 || i == 4 || i == 6 || i == 8)){
			// 	(*jeu).tabJeu[i][j].isuse = 1;
			// 	(*jeu).tabJeu[i][j].piece.numero = 1;
			// }
			// else if((j == 0 || j == 2) && (i == 1 || i == 3 || i == 5 || i == 7 || i == 9)){
			// 	(*jeu).tabJeu[i][j].isuse = 1;
			// 	(*jeu).tabJeu[i][j].piece.numero = 1;
			// }

			// //s'il y a une pièce du joueur 2
			// else if((j == 6 || j == 8) && (i == 1 || i == 3 || i == 5 || i == 7 || i == 9)){
			// 	(*jeu).tabJeu[i][j].isuse = 1;
			// 	(*jeu).tabJeu[i][j].piece.numero = 2;
			// }
			// else if((j == 7 || j == 9) && (i == 0 || i == 2 || i == 4 || i == 6 || i == 8)){
			// 	(*jeu).tabJeu[i][j].isuse = 1;
			// 	(*jeu).tabJeu[i][j].piece.numero = 2;
			// }
			// else (*jeu).tabJeu[i][j].isuse = 0;


			if((i == 1 || i == 3 || i == 5 || i == 7 || i == 9)){
				if(j == 1 || j == 3){
					(*jeu).tabJeu[i][j].isuse = 1;
					(*jeu).tabJeu[i][j].piece.numero = 1;
				}
				else if((j == 7 || j == 9)){
					(*jeu).tabJeu[i][j].isuse = 1;
					(*jeu).tabJeu[i][j].piece.numero = 2;
				}
				else (*jeu).tabJeu[i][j].isuse = 0;
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
					(*jeu).tabJeu[i][j].isuse = 0;
					(*jeu).tabJeu[i][j].piece.numero = 0;
				}
			}
		}
	}
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


	// for(i = 0; i < 10; i++){
	// 	for(j = 0; j < 10; j++){
	// 		(*jeu).tabJeu[i][j].x = i;
	// 		(*jeu).tabJeu[i][j].y = j;
	// 	}
	// }
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

int rechercherPion(Jeu *jeu, int x, int y, int numC){

	int i;

	if(((*jeu).tabJeu[x][y].isuse != 1) && ((*jeu).tabJeu[x][y].isuse != 0)){
		printf("\nErreur Jeton hors du plateau\n");
		exit(1);
	}

	if((*jeu).tabJeu[x][y].isuse == 1){
			return 0;
	}
	return 1;
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
	printf("%d  %d\n", *x3, *y3);
	p=rechercherPion(jeu, *x3, *y3, numC);
	if(p == 1)
		return 0;
	else
		return 1;
}

int deplacerPion(Jeu *jeu, Client c){
	int x1, y1, x2, y2, *x3, *y3, p, d;

	printf("\nChoisissez un pion a déplacer :\nDonnez moi sa position x (abscisse) :\n");
	scanf("%d", &x1);
	printf("Donnez moi sa position y (ordonnée) :\n");
	scanf("%d", &y1);

	p=rechercherPion(jeu, x1, y1, c.numero);
	if(p == 0 && ((*jeu).tabJeu[x1][y1].piece.numero == c.numero)){
		printf("\nChoisissez une case de destination :\nDonnez moi sa position x (abscisse) :\n");
		scanf("%d", &x2);
		printf("Donnez moi sa position y (ordonnée) :\n");
		scanf("%d", &y2);

		p=rechercherPion(jeu, x2, y2, c.numero);
		if(p == 1){
			d=deplacement(c.numero, x1, x2, y1, y2);
			if(d == 1){
				printf("\nDéplacement non valide !\n\n");
				return 1;
			}
			(*jeu).tabJeu[x1][y1].isuse = 0;
			(*jeu).tabJeu[x1][y1].piece.numero = 0;
			(*jeu).tabJeu[x2][y2].isuse = 1;
			(*jeu).tabJeu[x2][y2].piece.numero = c.numero;
			return 0;
		}
		else{
			if((*jeu).tabJeu[x2][y2].piece.numero == c.numero){
				printf("\nCette case est occupée !\n\n");
				return 1;
			}
			else{
				p=caseIsLibreManger(jeu, c.numero, x1, x2, y1, y2, &x3, &y3);
				if(p == 0){
					manger(jeu, c.numero, x1, x2, y1, y2, &x3, &y3);
					return 0;
				}
				else{
					printf("\nDéplacement impossible !\n");
					return 1;
				}
			}
		}
	}
	else
		printf("Ce n'est pas votre pion !\n");
}

void manger(Jeu *jeu, int numC, int x1, int x2, int y1, int y2, int *x3, int *y3){

	(*jeu).tabJeu[x1][y1].isuse = 0;
	(*jeu).tabJeu[x1][y1].piece.numero = 0;
	(*jeu).tabJeu[x2][y2].isuse = 0;
	(*jeu).tabJeu[x2][y2].piece.numero = 0;
	(*jeu).tabJeu[*x3][*y3].isuse = 1;
	(*jeu).tabJeu[*x3][*y3].piece.numero = numC;
}

int deplacerAuto(Jeu *jeu, Client c){

	int i, j, p, x1, x2, *x3, y1, y2, *y3;

	for(i = 0; i < 10; i++){
		for(j = 0; j < 10; j++){
			if((*jeu).tabJeu[i][j].isuse == 1 && (*jeu).tabJeu[i][j].piece.numero == c.numero){
				x1 = i;
				y1 = j;
				x2 = x1-1;
				y2 = y1+1;
				if(((*jeu).tabJeu[x2][y2].isuse != 1) && ((*jeu).tabJeu[x2][y2].isuse != 0)){
					if((*jeu).tabJeu[x2][y2].isuse == 1 && (*jeu).tabJeu[x2][y2].piece.numero != c.numero){
						p=caseIsLibreManger(jeu, c.numero, x1, x2, y1, y2, &x3, &y3);
						if(p == 0){
							manger(jeu, c.numero, x1, x2, y1, y2, &x3, &y3);
							return 0;
						}
					}
				}

				x2 = x1+1;
				y2 = y1+1;

				if(((*jeu).tabJeu[x2][y2].isuse != 1) && ((*jeu).tabJeu[x2][y2].isuse != 0)){
					if((*jeu).tabJeu[x2][y2].isuse == 1 && (*jeu).tabJeu[x2][y2].piece.numero != c.numero){
						p=caseIsLibreManger(jeu, c.numero, x1, x2, y1, y2, &x3, &y3);
						if(p == 0){
							manger(jeu, c.numero, x1, x2, y1, y2, &x3, &y3);
							return 0;
						}
					}
				}

				x2 = x1-1;
				y2 = y1-1;

				if(((*jeu).tabJeu[x2][y2].isuse != 1) && ((*jeu).tabJeu[x2][y2].isuse != 0)){
					if((*jeu).tabJeu[x2][y2].isuse == 1 && (*jeu).tabJeu[x2][y2].piece.numero != c.numero){
						p=caseIsLibreManger(jeu, c.numero, x1, x2, y1, y2, &x3, &y3);
						if(p == 0){
							manger(jeu, c.numero, x1, x2, y1, y2, &x3, &y3);
							return 0;
						}
					}
				}

				x2 = x1+1;
				y2 = y1-1;

				if(((*jeu).tabJeu[x2][y2].isuse != 1) && ((*jeu).tabJeu[x2][y2].isuse != 0)){
					if((*jeu).tabJeu[x2][y2].isuse == 1 && (*jeu).tabJeu[x2][y2].piece.numero != c.numero){
						p=caseIsLibreManger(jeu, c.numero, x1, x2, y1, y2, &x3, &y3);
						if(p == 0){
							manger(jeu, c.numero, x1, x2, y1, y2, &x3, &y3);
							return 0;
						}
					}
				}
			}

		}
	}

	return 1;
}