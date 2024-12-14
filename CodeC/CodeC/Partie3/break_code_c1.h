#ifndef BREAK_CODE_H
#define BREAK_CODE_H

#include <stdbool.h>

/* Effectue l'algorithme d'émondage sur le fichier input avec 
    une clé de longueur key_length
    Renvoie le nombre de clé potentielle, -1 si echec, -2 si erreur */
long break_code_c1(FILE *input, int key_length);

#endif // BREAK_CODE_H
