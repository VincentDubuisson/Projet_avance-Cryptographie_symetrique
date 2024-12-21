#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xor.h"

// Fonction pour lire le contenu d'un fichier texte
char *read_text_file(const char *filename) {
    FILE *file = fopen(filename, "r"); 
    if (!file) {
        perror("Erreur lors de l'ouverture du fichier");
        exit(EXIT_FAILURE);
    }

    // Aller à la fin pour déterminer la taille
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);

    // Allouer de la mémoire pour le contenu du fichier
    char *data = malloc(size + 1);  
    if (!data) {
        perror("Erreur d'allocation mémoire");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    // Lire tout le fichier dans le buffer
    fread(data, 1, size, file);
    data[size] = '\0';  
    fclose(file);

    return data;
}

// Fonction pour écrire du texte dans un fichier
void write_text_file(const char *filename, const char *data) {
    FILE *file = fopen(filename, "w");  
    if (!file) {
        perror("Erreur lors de l'ouverture du fichier en écriture");
        exit(EXIT_FAILURE);
    }

    fprintf(file, "%s", data);  
    fclose(file);
}

// Fonction principale pour cracker le masque réutilisé
void crack_C1(const char *file_c1, const char *file_c2, const char *file_clair1, const char *file_clairR) {
    // Lire les fichiers
    char *c1 = read_text_file(file_c1);
    char *c2 = read_text_file(file_c2);
    char *clair1 = read_text_file(file_clair1);

    // Vérifier que les tailles des fichiers sont compatibles
    size_t len_c1 = strlen(c1);
    size_t len_c2 = strlen(c2);
    size_t len_clair1 = strlen(clair1);
    if (len_c1 != len_c2 || len_c1 != len_clair1) {
        fprintf(stderr, "Erreur : Les tailles des fichiers ne correspondent pas.\n");
        free(c1);
        free(c2);
        free(clair1);
        exit(EXIT_FAILURE);
    }

    // Allouer de la mémoire pour stocker le texte clair résultant
    char *xorc1c2 = malloc(len_c1 + 1); 
    char *clairR = malloc(len_c1 + 1);  
    if (!clairR) {
        perror("Erreur d'allocation mémoire");
        free(c1);
        free(c2);
        free(clair1);
        exit(EXIT_FAILURE);
    }

    // Décrypter clairR en utilisant la formule : m2 = (c1 XOR c2) XOR m1
        xor(c1,c2,len_c1,xorc1c2);
        xor(xorc1c2,clair1,len_c1,clairR);

    clairR[len_c1] = '\0';  // Terminer la chaîne

    // Écrire le résultat dans le fichier clairR
    write_text_file(file_clairR, clairR);

    // Libérer la mémoire allouée
    free(c1);
    free(c2);
    free(clair1);
    free(clairR);

    printf("Le message clair a été écrit dans le fichier : %s\n", file_clairR);
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage : %s chif1.txt chif2.txt test1.txt clair.txt\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Lancer le décryptage avec les fichiers spécifiés
    crack_C1(argv[1], argv[2], argv[3], argv[4]);

    return EXIT_SUCCESS;
}
