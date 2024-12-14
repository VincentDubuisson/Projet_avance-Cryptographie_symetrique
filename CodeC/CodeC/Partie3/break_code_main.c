#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>
#include <time.h>
#include "break_code_c1.h"

#define RED "\x1B[31m"
#define NRM "\x1B[0m"

void print_help() {
    printf("Usage: break_code [OPTIONS]\n");
    printf("Options:\n");
    printf("  -i  Nom du fichier contenant le message chiffré (obligatoire)\n");
    printf("  -k  Longueur de la clé (obligatoire)\n");
    printf("  -l  Fichier de log (facultatif)\n");
    printf("  -h  Affiche cette aide\n");
}

int main(int argc, char* argv[]) {
    int opt;
    char *input_file = NULL;
    int key_length = 0;
    char *log_file = NULL;
    int show_help = 0;

    // Lecture des options avec getopt
    while ((opt = getopt(argc, argv, "i:k:l:h")) != -1) {
        switch (opt) {
            case 'i':
                input_file = optarg;
                break;
            case 'k':
                key_length = atoi(optarg);
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
    if (input_file == NULL || key_length == 0) {
        fprintf(stderr, "%sErreur: Les options -i et -k sont obligatoires.%s\n", RED, NRM);
        print_help();
        return 1;
    }

    // Ouvrir le fichier d'entrée
    FILE *input_fp = fopen(input_file, "rb");
    if (input_fp == NULL) {
        fprintf(stderr, "%sErreur d'ouverture du fichier d'entrée (%s)%s\n", RED, input_file, NRM);
        return 1;
    }

    long nb_key = break_code_c1(input_fp, key_length);

    // Fermeture des fichiers
    fclose(input_fp);

    // Gestion des logs si nécessaire
    if (log_file != NULL) {
        FILE *log_fp = fopen(log_file, "a");
        if (log_fp != NULL) {
            // Obtenir la date et l'heure actuelle
            time_t now = time(NULL);
            struct tm *t = localtime(&now);
            fprintf(log_fp, "-------------------------------%02d/%02d/%04d - %02d:%02d-------------------------------\n",
                t->tm_mday, t->tm_mon + 1, t->tm_year + 1900, t->tm_hour, t->tm_min);
            fprintf(log_fp, "-> input file: %s | key_length: %d\n", input_file, key_length);
            
            if (nb_key == -2) {
                fprintf(log_fp, "\t[ERREUR] Veuillez utiliser un texte sans accent pour le fichier crypté!\n\n");
            } else if (nb_key == -1) {
                fprintf(log_fp, "\t[ECHEC] Le crack n'a pas pu se terminer correctement!\n\n");
            } else {
                if (nb_key >= 1000000000000) {
                    fprintf(log_fp, "\tNombre total de clé potentielles > 1 000 000 000 000 (mille milliards)\n\n");
                } else {
                    fprintf(log_fp, "\tNombre de clé potentielles: %ld\n\n", nb_key);
                }
                
            }

            fclose(log_fp);
        }
    }

    return 0;
}