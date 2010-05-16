# Code Vivapoly pour Robopoly

Ce repo servira à partager le code pour commander les voitures du stand
Vivapoly. Il est organisé de la façon suivante :

## main.c

* Lit les infos du nunchuk
* Envoie les tensions appropriées à la télécommande de la voiture
* Reçoit les commandes du module maître pour gérer la vitesse max et
éventuellement bloquer la commande.

## dac.c

* Permet d'envoyer des commandes analogiques dans les seuils tolérés par la
télécommande de la voiture.
* Fournit des commandes pour régler l'offset (tension de repos) à partir de
potentiomètres branchés sur le port A

## nunchuk.c ( Thibault Dupont )

* Fonctions pour lire l'état du nunchuk et stocker dans un "objet" nunchuk

## robopoly_i2c.c ( Christophe Auvigne )

* Fonctions générales pour l'I2C

