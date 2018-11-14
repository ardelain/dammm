#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <errno.h>
#include <ctype.h>

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

	int nbPJ1, nbPJ2, choix = 1, i = 0;

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

	misEnPlaceJeu(/*client1, client2, */&jeu);

	nbPJ1=nbPionClient(&jeu, client1);
	nbPJ2=nbPionClient(&jeu, client2);

	afficher(jeu);

	while(nbPJ1 != 0 || nbPJ2 != 0){
		while(choix != 0){

			choix = deplacerAuto(&jeu, client1);
			while(choix == 0){
				printf("\nDéplacement automatique effectué car vous pouviez manger un pion !\n\n");
				afficher(jeu);
				choix = deplacerAuto(&jeu, client1);
				i=i+1;
			}
			if(i == 0){
				choix = deplacerPion(&jeu, client1);
				afficher(jeu);
			}
			else{
				choix=0;
			}
			i=0;
		}
		choix = 1;

		while(choix != 0){

			choix = deplacerAuto(&jeu, client2);
			while(choix == 0){
				printf("\nDéplacement automatique effectué car vous pouviez manger un pion !\n\n");
				afficher(jeu);
				choix = deplacerAuto(&jeu, client2);
				i=i+1;
			}
			if(i == 0){
				choix = deplacerPion(&jeu, client2);
				afficher(jeu);
			}
			else{
				choix=0;
			}
			i=0;
		}
		choix = 1;
	}

	// 		choix = deplacerAuto(&jeu, client2);
	// 		if(choix == 0){
	// 			printf("\nDéplacement automatique effectué car vous pouviez manger un pion !\n\n");
	// 		}

	// 		else{
	// 			choix = deplacerPion(&jeu, client2);
	// 		}
	// 	}
	// 	choix = 1;
	// 	afficher(&jeu);
	// }

	if(nbPJ1 == 0){
		printf("\nLe joueur %s a gagné!\n", client1.nom);
	}
	else{
		printf("\nLe joueur %s a gagné!\n", client2.nom);
	}

	return 0;
}

void misEnPlaceJeu(/*Client client1, Client client2, */Jeu *jeu){

	int i, j;
	// (*jeu).client1 = client1;
	// (*jeu).client2 = client2;

	for(i = 0; i < 10; i++){
		for(j = 0; j < 10; j++){

			if((i == 1 || i == 3 || i == 5 || i == 7 || i == 9)){
				if(j == 1 || j == 3){
					(*jeu).tabJeu[i][j].isuse = 1;
					(*jeu).tabJeu[i][j].piece.numero = 1;
					(*jeu).tabJeu[i][j].piece.type = 1;
				}
				else if((j == 7 || j == 9)){
					(*jeu).tabJeu[i][j].isuse = 1;
					(*jeu).tabJeu[i][j].piece.numero = 2;
					(*jeu).tabJeu[i][j].piece.type = 1;
				}
				else{
					(*jeu).tabJeu[i][j].isuse = 2;
					(*jeu).tabJeu[i][j].piece.numero = 0;
					(*jeu).tabJeu[i][j].piece.type = 0;
				}
			}

			else if(i == 0 || i == 2 || i == 4 || i == 6 || i == 8){
				if((j == 0 || j == 2)){
					(*jeu).tabJeu[i][j].isuse = 1;
					(*jeu).tabJeu[i][j].piece.numero = 1;
					(*jeu).tabJeu[i][j].piece.type = 1;
				}
				else if((j == 6 || j == 8)){
					(*jeu).tabJeu[i][j].isuse = 1;
					(*jeu).tabJeu[i][j].piece.numero = 2;
					(*jeu).tabJeu[i][j].piece.type = 1;
				}
				else {
					(*jeu).tabJeu[i][j].isuse = 2;
					(*jeu).tabJeu[i][j].piece.numero = 0;
					(*jeu).tabJeu[i][j].piece.type = 0;
				}
			}
		}
	}
}

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
			if(jeu.tabJeu[i][j].isuse == 1){
				if(jeu.tabJeu[i][j].piece.numero == 1){
					if(jeu.tabJeu[i][j].piece.type == 1)
						printf("|x");
					else if(jeu.tabJeu[i][j].piece.type == 2)
						printf("|X");
				}
				else if(jeu.tabJeu[i][j].piece.numero == 2){
					if(jeu.tabJeu[i][j].piece.type == 1)
						printf("|o");
					else if(jeu.tabJeu[i][j].piece.type == 2)
						printf("|O");
				}
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

	int i, j;

	for(i = 0; i < 10; i++){
		for(j = 0; j < 10; j++){
			if(x == i && y == j){
				return 0;
			}
		}
	}

	return 1;

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

	printf("Donnez moi sa position indiqué sur la gauche du plateau :\n");
	scanf("%s", &pos);

	while(sscanf(&pos, "%d", &valeur) != 1){
		printf("\nEntrez un int pour continuer\n");
		vider_buffer();
		
		printf("Donnez moi sa position indiqué sur la gauche du plateau :\n");
		scanf("%s", &pos);
	}

	return valeur;
}

int choisirPositionDroite(){

	vider_buffer();

	char pos=""; 
	int valeur;

	printf("Donnez moi sa position indiqué au dessus du plateau :\n");
	scanf("%s", &pos);

	while(sscanf(&pos, "%d", &valeur) != 1){
		printf("\nEntrez un int pour continuer\n");
		vider_buffer();
		
		printf("Donnez moi sa position indiqué au dessus du plateau :\n");
		scanf("%s", &pos);
	}

	return valeur;
}

int deplacerPion(Jeu *jeu, Client c){
	int x1=0, y1=0, x2, y2, *x3, *y3, p, d, i;

	printf("\nChoisissez un pion a déplacer :\n");
	x1=choisirPositionGauche();
	y1=choisirPositionDroite();

	p=rechercherPionPlateau(jeu, x1, y1, c.numero);
	if(p == 1){
		printf("\nErreur Jeton hors du plateau\n");
		return 1;
	}

	p=rechercherPionJoueur(jeu, x1, y1, c.numero);
	if(p == 0 && (*jeu).tabJeu[x1][y1].piece.type == 1){
		printf("\nChoisissez une case de destination :\n");
		x2=choisirPositionGauche();
		y2=choisirPositionDroite();

		p=rechercherPionPlateau(jeu, x2, y2, c.numero);
		if(p == 1){
			printf("\nErreur Jeton hors du plateau\n");
			return 1;
		}

		p=rechercherPionJoueur(jeu, x2, y2, c.numero);
		if(p == 1){
			d=deplacement(c.numero, x1, x2, y1, y2);
			if(d == 1){
				printf("\nDéplacement non valide !\n\n");
				return 1;
			}
			(*jeu).tabJeu[x1][y1].isuse = 2;
			(*jeu).tabJeu[x1][y1].piece.numero = 0;
			(*jeu).tabJeu[x1][y1].piece.type = 0;
			(*jeu).tabJeu[x2][y2].isuse = 1;
			(*jeu).tabJeu[x2][y2].piece.numero = c.numero;
			(*jeu).tabJeu[x2][y2].piece.type = 1;
			verifDame(jeu, c.numero, x2, y2);
			return 0;
		}
		// else if(p == 2){
		// 	p=caseIsLibreManger(jeu, c.numero, x1, x2, y1, y2, &x3, &y3);
		// 	if(p == 0){
		// 		manger(jeu, c.numero, x1, x2, y1, y2, &x3, &y3);
		// 		return 0;
		// 	}
		// 	else{
		// 		printf("\nDéplacement impossible !\n");
		// 		return 1;
		// 	}
		// }

		else if(p == 0 && (*jeu).tabJeu[x1][y1].piece.type == 2){

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

void manger(Jeu *jeu, int numC, int x1, int x2, int y1, int y2, int *x3, int *y3){

	(*jeu).tabJeu[x1][y1].isuse = 2;
	(*jeu).tabJeu[x1][y1].piece.numero = 0;
	(*jeu).tabJeu[x1][y1].piece.type = 0;
	(*jeu).tabJeu[x2][y2].isuse = 2;
	(*jeu).tabJeu[x2][y2].piece.numero = 0;
	(*jeu).tabJeu[x2][y2].piece.type = 0;
	(*jeu).tabJeu[*x3][*y3].isuse = 1;
	(*jeu).tabJeu[*x3][*y3].piece.numero = numC;
	(*jeu).tabJeu[*x3][*y3].piece.type = 1;
}

int mangerAuto(Jeu *jeu, int x1, int x2, int y1, int y2, int numC){

	int p, *x3, *y3;

	if((*jeu).tabJeu[x2][y2].piece.numero == 1 || (*jeu).tabJeu[x2][y2].piece.numero == 2){
		if((*jeu).tabJeu[x2][y2].isuse == 1 && (*jeu).tabJeu[x2][y2].piece.numero != numC){
			p=caseIsLibreManger(jeu, numC, x1, x2, y1, y2, &x3, &y3);
			if(p == 0){
				manger(jeu, numC, x1, x2, y1, y2, &x3, &y3);
				verifDame(jeu, numC, x3, y3);
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
			if((*jeu).tabJeu[i][j].isuse == 1 && (*jeu).tabJeu[i][j].piece.numero == c.numero){
				x1 = i;
				y1 = j;

				x2 = x1-1;
				y2 = y1+1;
				p=mangerAuto(jeu, x1, x2, y1, y2, c.numero);
				if (p == 0)
				{
					return 0;
				}

				x2 = x1+1;
				y2 = y1+1;
				p=mangerAuto(jeu, x1, x2, y1, y2, c.numero);
				if (p == 0)
				{
					return 0;
				}

				x2 = x1-1;
				y2 = y1-1;
				p=mangerAuto(jeu, x1, x2, y1, y2, c.numero);
				if (p == 0)
				{
					return 0;
				}

				x2 = x1+1;
				y2 = y1-1;
				p=mangerAuto(jeu, x1, x2, y1, y2, c.numero);
				if (p == 0)
				{
					return 0;
				}
			}

		}
	}

	return 1;
}

void verifDame(Jeu *jeu, int numC, int x, int y){

	int i, j;

	for(i = 0; i < 10; i++){
		for(j = 0; j < 10; j++){
			if(numC == 1 && j == 9 && i == x && j == y){
				(*jeu).tabJeu[i][j].piece.type = 2;
			}
			else if(numC == 2 && j == 0 && i == x && j == y){
				(*jeu).tabJeu[i][j].piece.type = 2;
			}
		}
	}
}