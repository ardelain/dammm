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

Déroulement du  programme cote client : (apres avoir rentrer l'adresse ip)
1er) choix entre plusieurs action pour definir ça connexion
		1.connexion
		2.inscription (connection  et enregistrer un client, la liste de ceux ci sera sauver et charger depuis un fichier)
		3.inviter (connection sans enregistrement)

2)on rentre une commande :
	list: lister les joueurs connectés
	parties : lister parties en cours
	jouer: faire une demande de jeu à un joueur
	voir: regarder une partie en cours

3) lors d'un partie:
		on nous indique si c'est notre tour et on nous indique la demarche pour selectionner un pion (numero abcysse et ordonné)
		-> quand on a joué on envoie les information du "nouveau" plateau au serveur ainsi que les différentes valeurs de notre mouvement; cela afin que le serveur rejout le tour et verifie son integrité (il verifie aussi l'identité de l'envoyeur et bloque les probèmes (mauvais client, pas sont tour, mauvais mouvement,...))

4)on peu regardé une partie et partir quand on le souhaite (commande : voir   / quitter)