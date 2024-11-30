#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <time.h>
#include "cypher.c"
#include "xor.h"

void print_help() {
    printf("Usage: sym_crypt [OPTIONS]\n");
    printf("Options:\n");
    printf("  -i  Nom du fichier contenant le message à chiffrer (obligatoire)\n");
    printf("  -o  Nom du fichier où écrire le message chiffré (obligatoire)\n");
    printf("  -k  Clé pour le chiffrement (obligatoire)\n");
    printf("  -f  Fichier contenant la clé\n");
    printf("  -m  Méthode de chiffrement (xor, cbc-crypt, cbc-uncrypt, mask)\n");
    printf("  -v  Fichier contenant le vecteur d'initialisation (obligatoire pour CBC)\n");
    printf("  -l  Fichier de log (facultatif)\n");
    printf("  -h  Affiche cette aide\n");
}

int main(int argc, char* argv[]) {
    int opt;
    char *input_file = NULL;
    char *output_file = NULL;
    char *key = NULL;
    char *key_file = NULL;
    char *method = NULL;
    char *iv_file = NULL;
    char *log_file = NULL;
    int show_help = 0;

    // Lecture des options avec getopt
    while ((opt = getopt(argc, argv, "i:o:k:f:m:v:l:h")) != -1) {
        switch (opt) {
            case 'i':
                input_file = optarg;
                break;
            case 'o':
                output_file = optarg;
                break;
            case 'k':
                key = optarg;
                break;
            case 'f':
                key_file = optarg;
                break;
            case 'm':
                method = optarg;
                break;
            case 'v':
                iv_file = optarg;
                break;
            case 'l':
                log_file = optarg;
                break;
            case 'h':
                show_help = 1;
                break;
            default:
                show_help = 1;
                break;
        }
    }

    // Si l'aide est demandée
    if (show_help) {
        print_help();
        return 1;
    }

    // Vérification des paramètres obligatoires
    if (input_file == NULL || output_file == NULL || (key == NULL && key_file == NULL) || method == NULL) {
        fprintf(stderr, "Erreur: Les options -i, -o, -k ou -f, et -m sont obligatoires.\n");
        print_help();
        return 1;
    }

    // Chargement de la clé depuis le fichier si nécessaire
    if (key_file != NULL) {
        FILE *key_fp = fopen(key_file, "rb");

        if (key_fp == NULL) {
            perror("Erreur d'ouverture du fichier de clé");
            return 1;
        }
        // Lire la clé dans le fichier
        char buffer[256];
        if (fgets(buffer, sizeof(buffer), key_fp) != NULL) {
            // Supprimer le saut de ligne si présent
            buffer[strcspn(buffer, "\n")] = '\0';
            key = malloc(strlen(buffer) + 1);
            if (key != NULL) {
                strcpy(key, buffer);
            }
        }
        fclose(key_fp);
    }


    // Ouvrir les fichiers d'entrée et de sortie
    FILE *input_fp = fopen(input_file, "rb");
    if (input_fp == NULL) {
        fprintf(stderr, "Erreur d'ouverture du fichier d'entrée (%s)\n", input_file);
        return 1;
    }

    FILE *output_fp = fopen(output_file, "wb");
    if (output_fp == NULL) {
        perror("Erreur d'ouverture du fichier de sortie");
        fclose(input_fp);
        return 1;
    }

    // Détermination de la taille du message
    fseek(input_fp, 0L, SEEK_END);
    int msg_length = ftell(input_fp);
    fseek(input_fp, 0L, SEEK_SET);

    // Méthode XOR
    if (strcmp(method, "xor") == 0) {
        // Allocation de mémoire pour le contenu du fichier d'entrée
        char *input_data = malloc(msg_length);
        if (input_data == NULL) {
            fprintf(stderr, "Erreur d'allocation de mémoire pour le fichier d'entrée\n");
            fclose(input_fp);
            fclose(output_fp);
            return 1;
        }
        // Lecture du contenu du fichier d'entrée
        fread(input_data, 1, msg_length, input_fp);

        char *crypted = malloc(msg_length + 1);
        xor(input_data, key, msg_length, crypted);
        fprintf(stdout, "Méthode xor réalisée avec succès !\n");

        // Écriture du message chiffré binaire dans le fichier de sortie
        fwrite(crypted, 1, msg_length, output_fp);

        free(crypted);
        free(input_data);

    // Méthode CBC CRYPT
    } else if (strcmp(method, "cbc-crypt") == 0) {
        char iv[BLOCK_SIZE];
        FILE *iv_fp = fopen(iv_file, "rb");
        if (iv_fp == NULL) {
          perror("Erreur d'ouverture du fichier de vecteur d'initialisation");
          return 1;
        }
        fread(iv, 1, BLOCK_SIZE, iv_fp);
        fclose(iv_fp);

        cbc_crypt(input_fp, output_fp, key, iv);
        fprintf(stdout, "Méthode CBC crypt réalisée avec succès !\n");

    // Méthode CBC UNCRYPT
    } else if (strcmp(method, "cbc-uncrypt") == 0) {
        char iv[BLOCK_SIZE];
        FILE *iv_fp = fopen(iv_file, "rb");
        if (iv_fp == NULL) {
          perror("Erreur d'ouverture du fichier de vecteur d'initialisation");
          return 1;
        }
        fread(iv, 1, BLOCK_SIZE, iv_fp);
        fclose(iv_fp);

        cbc_uncrypt(input_fp, output_fp, key, iv);
        fprintf(stdout, "Méthode CBC uncrypt réalisée avec succès !\n");

    // Méthode MASK
    } else if (strcmp(method, "mask") == 0) {
        char *masked = malloc(msg_length);
        mask(input_file, msg_length, masked);
        fwrite(masked, 1, msg_length, output_fp);
        fprintf(stdout, "Méthode mask réalisée avec succès !\n");
        free(masked);

    // Affichage de l'aide
    } else {
        fprintf(stderr, "Erreur: Méthode de chiffrement non reconnue.\n");
        print_help();
        return 1;
    }

    // Fermeture des fichiers
    fclose(input_fp);
    fclose(output_fp);

    // Libération de la clé
    if (key_file != NULL) {
        free(key);
    }

    // Gestion des logs si nécessaire
    if (log_file != NULL) {
        FILE *log_fp = fopen(log_file, "a");
        if (log_fp != NULL) {
            // Obtenir la date et l'heure actuelle
            time_t now = time(NULL);
            struct tm *t = localtime(&now);
            fprintf(log_fp, "-------------------------------%02d/%02d/%04d - %02d:%02d-------------------------------\n",
                t->tm_mday, t->tm_mon + 1, t->tm_year + 1900, t->tm_hour, t->tm_min);
            fprintf(log_fp, "-> method: %s | input file: %s | output file: %s\n",
                method, input_file, output_file);

            // Contenue du fichier en entrée
            fprintf(log_fp, "\n-> %s content:\n", input_file);
            input_fp = fopen(input_file, "rb");
            if (input_fp) {
                int ch;
                while ((ch = fgetc(input_fp)) != EOF) {
                    fputc(ch, log_fp);
                }
                fclose(input_fp);
            } else {
                fprintf(log_fp, "Erreur lors de la lecture du fichier d'entrée\n");
            }

            // Contenue du fichier en sortie
            fprintf(log_fp, "\n-> %s content:\n", output_file);
            output_fp = fopen(output_file, "rb");
            if (output_fp) {
                int ch;
                while ((ch = fgetc(output_fp)) != EOF) {
                    fputc(ch, log_fp);
                }
                fclose(output_fp);
            } else {
                fprintf(log_fp, "Erreur lors de la lecture du fichier de sortie\n");
            }

            fprintf(log_fp, "\n\n");
            fclose(log_fp);
        }
    }

    return 0;
}
