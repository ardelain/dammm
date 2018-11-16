# dammm
jeu de dame en réseaux
</br>
<img src="/ressource/jeu de dammm.jpg" alt="My cool logo"/>
</br>


Réalisé Par Arthur Delain et Floran Chazelas

Le serveur damm : 

executer coté serveur :
cd src/serveur/
gcc -o serveur serveur.c -lpthread

executer coté client :
cd src/client/
gcc -o client client.c -lpthread


C'est un serveur TCP multiclient ecrit en C avec les socket.

Déroulement du  programme cote client : (apres avoir rentrer l'adresse ip)</br>
</br>
1) choix entre plusieurs action pour definir ça connexion</br>
		1.connexion</br>
		2.inscription (connection  et enregistrer un client, la liste de ceux ci sera sauver et charger depuis un fichier)</br>
		3.inviter (connection sans enregistrement)</br>

2) on rentre une commande :</br>
	list: lister les joueurs connectés</br>
	parties : lister parties en cours</br>
	jouer: faire une demande de jeu à un joueur</br>
	voir: regarder une partie en cours</br>

3) lors d'un partie:</br>
		on nous indique si c'est notre tour et on nous indique la demarche pour selectionner un pion (numero abcysse et ordonné)</br>
		-> quand on a joué on envoie les informations du "nouveau" plateau au serveur ainsi que les différentes valeurs de notre mouvement; cela afin que le serveur rejout le tour et verifie son integrité (il verifie aussi l'identité de l'envoyeur et bloque les probèmes (mauvais client, pas sont tour, mauvais mouvement,...))</br>

4) on peut regardé une partie et partir quand on le souhaite (commande : voir   / quitter)</br>



L'implementation du jeu de damme n'est pas integré en totalité, mais toutes les regles et methodes restantent sont dans methodedeJeux.c (je le jeu reste optionnel) 