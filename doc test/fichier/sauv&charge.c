#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#define TAILLE 50
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