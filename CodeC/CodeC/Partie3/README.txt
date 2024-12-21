README.txt
==========

Ce document fournit des instructions sur l'utilisation du logiciel.

Prérequis :
------------
- Assurez-vous d'avoir compilé le programme correctement.
- Vous devez disposer des droits nécessaires pour exécuter l'application dans votre terminal.

Compiler le programme :
------------------------
Pour compiler le programme, exécutez dans votre terminal la
commande suivante :

make logiciel


Lancer le programme :
----------------------
Pour démarrer le programme, exécutez-le depuis votre terminal avec la commande suivante :

./logiciel

Une fois le logiciel exécuté plusieurs commandes sont à votre disposition.


Commandes disponibles :
------------------------

1. **help**
   Affiche une aide détaillée listant toutes les commandes disponibles.


2. **list-keys**
Liste toutes les clés générées et leur statut.


3. **gen-key <n>**
Génère une clé de longueur `n` (en bits). Par exemple, pour générer une clé de 9 bits :

gen-key 9


4. **del-key <key>**
Supprime la clé spécifiée par `<key>`.

Exemple :
del-key 12345abcd


5. **encrypt <in> <out> <key> <method> [<iv> taille = 16]**
Chiffre un fichier selon une méthode spécifiée.
- `<in>` : fichier d'entrée.
- `<out>` : fichier de sortie.
- `<key>` : clé de chiffrement (uniquement pour `cbc`, `xor`).
- `<method>` : méthode de chiffrement (`cbc`, `xor`, `mask`).
- `[<iv>]` : vecteur d'initialisation (requis pour uniquement`cbc`).

Exemple :
encrypt input.txt output.txt mykey123 cbc 1234567890123456



6. **decrypt <in> <out> <key> <method> [<iv> taille = 16]**
Déchiffre un fichier selon une méthode spécifiée.
- `<in>` : fichier chiffré.
- `<out>` : fichier de sortie.
- `<key>` : clé de déchiffrement (uniquement pour `cbc`, `xor`).
- `<method>` : méthode de déchiffrement (`cbc`, `xor`).
- `[<iv>]` : vecteur d'initialisation (requis pour uniquement`cbc`).

Exemple :
decrypt output.txt decrypted.txt mykey123 cbc 1234567890123456



7. **crack <in> <out> <length>**
Tente de déterminer les clés potentielles pour déchiffrer un fichier sans caractère spéciaux.
- `<in>` : fichier chiffré.
- `<out>` : fichier de sortie pour les clés potentielles.
- `<length>` : longueur de la clé à tester.

Exemple :
crack output.txt potential_keys.txt 9


8. **quit**
Quitte le programme.

==========

Les méthodes cbc crypt et uncrypt ne fonctionnent que pour des fichiers textes.

