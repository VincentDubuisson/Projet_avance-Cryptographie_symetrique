#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "xor.h"

/*
  Applique l'opérande xor (^) sur le message 'msg' de longueur 'msg_length'
  avec la clé 'key' dans le message 'crypted'
*/
void xor(char *msg, char* key, int msg_length, char *crypted) {
  if (msg == NULL || key == NULL || crypted == NULL) {
      fprintf(stderr, "Erreur : msg, key ou crypted est nul.\n");
      return;
  }

  // Ajouter un caractère nul à la fin de la clé si elle n'est pas déjà présente
  int key_length = strlen(key);
  if (key_length == 0) {
      fprintf(stderr, "Erreur : La clé ne peut pas être vide\n ");
      return;
  }

  for (int i = 0; i < msg_length; i++) {
      crypted[i] = msg[i] ^ key[i % key_length]; // Réalisation du XOR pour chaque caractère du message
  }
  crypted[msg_length] = '\0';

}

/*
  Génère une clé aléatoire d'une longueur donnée
*/
void gen_key(int length, char *key) {

    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    int charset_size = sizeof(charset) - 1; // -1 pour exclure le '\0'

    for (int i = 0; i < length; i++) {
        int random_index = rand() % charset_size;
        key[i] = charset[random_index];
    }

    key[length] = '\0';  // Terminaison de la chaîne
}

/*
  Applique le chiffrage mask sur le message msg et le renvoie dans le message 'masked'
*/
void mask(char *msg, int msg_length, char *masked) {

    // Génération de la clé aléatoir
    char key[msg_length + 1];
    gen_key(msg_length, key);  // Génère une clé aléatoire aussi longue que le message

    // Écriture de la clé dans un fichier
    FILE *key_file = fopen("../Source/key.txt", "w");
    if (key_file == NULL) {
        perror("Erreur lors de l'ouverture du fichier de clé");
        return;
    }
    fprintf(key_file, "%s", key); // Écriture de la clé dans le fichier
    fclose(key_file); // Fermeture du fichie

    // Utiliser la fonction xor pour chiffrer le message
    xor(msg, key, msg_length, masked);  // Chiffrement par XOR avec la clé générée

    // Ajouter le caractère de fin de chaîne
    masked[msg_length] = '\0';
}
