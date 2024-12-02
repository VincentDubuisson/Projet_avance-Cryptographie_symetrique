#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "dh_prime.h"

#define RED "\x1B[31m"
#define NRM "\x1B[0m"

//fonction ecrire le nombre premier et le generateur dans un fichier 
void write_file(const char * file_name , long prime , long generator){
    FILE * file = fopen(file_name, "w");

    if (file == NULL){
        printf("%sErreur : impossible d'ouvrir le fichier %s \n %s", RED, file_name, NRM);
        exit(1);
    }

    fprintf(file , "p:%ld\ng:%ld\n", prime, generator);
    fclose(file);

    printf("Paramètres écrit dans %s : p = %ld , g = %ld \n", file_name, prime, generator);

    }

void print_help() {
    printf("Usage: dh_gen_group [-o output_file] [-h]\n");
    printf("Options:\n");
    printf("  -o output_file    Fichier de sortie où seront écrit les paramètres\n");
    printf("  -h                Affiche l'aide\n");
}

int main (int argc , char * argv[]){
    int opt;
    char *output_file = NULL;

    // Analyse des options
    while ((opt = getopt(argc, argv, "o:h")) != -1) {
        switch (opt) {
            case 'o':
                output_file = optarg;
                break;
            case 'h':
                print_help();
                return 0;
            default:
                print_help();
                return 1;
        }
    }

    long min= 100000000;
    long max= 1000000000;
    int count = 0;

    srand(time(NULL));
    long prime = genPrimeSophieGermain(min,max,&count);
    
    if (prime ==-1){
        printf("%sErreur generation nombre premier SophieGermain %s", RED, NRM);
        exit(1);
    }

    //Cherche generateur pour le nombre premier 2
    srand(time(NULL));
    long generator = seek_generator(2, prime); 
    if (generator == -1){
        printf("%sErreur recherche de generateur pour p %ld%s",RED, prime, NRM);
        exit(1);
    }

    write_file(output_file, prime, generator);
    return 0;
}