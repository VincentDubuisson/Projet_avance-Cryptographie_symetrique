#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "xor.h"

#define BLOCK_SIZE 16  // Taille des blocs (16 octets)

// Fonction de chiffrement CBC
void cbc_crypt(FILE *input, FILE *output, char *key, char *iv);

// Fonction de déchiffrement CBC
void cbc_uncrypt(FILE *input, FILE *output, char *key, char *iv);