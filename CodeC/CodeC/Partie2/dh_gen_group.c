#include <stdio.h>
#include <stdlib.h>
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
    printf("Parametre ecrit dans %s : p = %ld , g = %ld \n", file_name, prime, generator);

    }

int main (int argc , char * argv[]){
    if (argc < 3){
        printf("%sUsage : %s -o <fichier de sortie>\n%s", RED, argv[0], NRM);
        exit(1);
    }

    long min= 100000000;
    long max= 1000000000;
    int count = 0;
    long prime = genPrimeSophieGermain(min,max,&count);
    
    if (prime ==-1){
        printf("%sErreur generation nombre premier SophieGermain %s", RED, NRM);
        exit(1);
    }

    //Cherche generateur pour le nombre premier 2
    long generator = seek_generator(2, prime); 
    if (generator == -1){
        printf("%sErreur recherche de generateur pour p %ld%s",RED, prime, NRM);
        exit(1);
    }
    write_file(argv[2], prime, generator);
    return 0;
}