#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "xor.c"

#define BLOCK_SIZE 16  // Taille des blocs (16 octets)

// Fonction d'affichage en hexadécimal
void print_block_as_hex(char *block, int length) {
    for (int i = 0; i < length; i++) {
        printf("%02x ", (unsigned char)block[i]);
    }
    printf("\n");
}

void write_hex(FILE *output, char *block, size_t block_size) {
    // Buffer temporaire pour stocker la représentation hexadécimale
    char hex_output[block_size * 2 + 1]; // 2 caractères hexadécimaux par octet + 1 pour '\0'

    for (size_t i = 0; i < block_size; i++) {
        // Convertir chaque octet en 2 caractères hexadécimaux
        sprintf(hex_output + i * 2, "%02x", (unsigned char)block[i]);
    }

    // Écrire la chaîne hexadécimale dans le fichier
    fwrite(hex_output, sizeof(char), block_size * 2, output);
}


// Fonction de chiffrement CBC
void cbc_crypt(FILE *input, FILE *output, char *key, char *iv) {
    char block[BLOCK_SIZE];
    char previous_block[BLOCK_SIZE];
    char crypted[BLOCK_SIZE];

    memcpy(previous_block, iv, BLOCK_SIZE);

    size_t bytesRead;
    while ((bytesRead = fread(block, 1, BLOCK_SIZE, input)) > 0) {
        char xor_result[BLOCK_SIZE];

        fprintf(stdout, "Block clair : ");
        print_block_as_hex(block, bytesRead);

        fprintf(stdout, "Block précédent : ");
        print_block_as_hex(previous_block, BLOCK_SIZE);

        // Si le dernier bloc est plus court, remplir avec des zéros
        if (bytesRead < BLOCK_SIZE) {
            memset(block + bytesRead, 0, BLOCK_SIZE - bytesRead);
        }

        // 1. XOR du block avec le block précédent
        xor(block, previous_block, BLOCK_SIZE, xor_result);

        // 2. Chiffrement du résultat avec la clé
        xor(xor_result, key, BLOCK_SIZE, crypted);


        // Afficher le résultat du XOR
        fprintf(stdout, "Bloc chiffré : ");
        print_block_as_hex(crypted, BLOCK_SIZE);

        // Écrire le bloc chiffré dans le fichier de sortie
        fwrite(crypted, 1, bytesRead, output);
        fprintf(stdout, "\n");

        memcpy(previous_block, crypted, BLOCK_SIZE);
    }

    if (ferror(input)) {
        perror("Erreur lors de la lecture du fichier d'entrée");
    }
}

// Fonction de déchiffrement CBC
void cbc_uncrypt(FILE *input, FILE *output, char *key, char *iv) {

    char block[BLOCK_SIZE];
    char previous_block[BLOCK_SIZE];
    char decrypted[BLOCK_SIZE];

    memcpy(previous_block, iv, BLOCK_SIZE);

    size_t bytesRead;
    while ((bytesRead = fread(block, 1, BLOCK_SIZE, input)) > 0) {
        char xor_result[BLOCK_SIZE];

        printf("Block chiffré : ");
        print_block_as_hex(block, bytesRead);

        printf("Block précédent : ");
        print_block_as_hex(previous_block, BLOCK_SIZE);

        // 1. Déchiffrement du bloc avec la clé
        xor(block, key, BLOCK_SIZE, xor_result);

        // 2. XOR avec le bloc précédent pour obtenir le texte clair
        xor(xor_result, previous_block, BLOCK_SIZE, decrypted);

        // Afficher le résultat du XOR
        printf("Bloc déchiffré : ");
        print_block_as_hex(decrypted, BLOCK_SIZE);

        // Écrire le bloc déchiffré dans le fichier de sortie
        fwrite(decrypted, 1, bytesRead, output);
        printf("\n");

        // Mettre à jour le bloc précédent
        memcpy(previous_block, block, BLOCK_SIZE);
    }

    if (ferror(input)) {
        perror("Erreur lors de la lecture du fichier d'entrée");
    }
}
