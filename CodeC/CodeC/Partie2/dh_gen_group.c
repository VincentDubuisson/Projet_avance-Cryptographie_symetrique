#include <stdio.h>
#include <stdlib.h>
#include "dh_prime.h"

//fonction ecrire le nombre premier et le generateur dans un fichier 
void ecrire_dans_fichier(const char * nomFichier , long prime , long generateur){
    FILE * file = fopen(nomFichier, "w");
    if (file == NULL){
        printf("erreur : impossible d'ouvrire le fichier %s \n ", nomFichier );
        exit(1);
    }
    fprintf(file , "p:%ld\ng:%ld\n",prime,generateur);
    fclose(file);
    printf("parametre ecrit dans %s : p = %ld , g = %ld \n",nomFichier,prime,generateur);

    }

int main (int argc , char * argv[]){
    if (argc < 3){
        printf("Usage : %s -o <fichier de sortie>\n", argv[0]);
        exit(1);

    }

    long min= 100000000;
    long max= 1000000000;
    int count = 0; //essais

    long prime = genPrimeSophieGermain(min,max,&count);
    
    if (prime ==-1){
        printf("Erreur generation nombre premier SophieGermain ");
        exit(1);
    }
    long generateur = seek_generator(2,prime);//cherche generateur pour le nombre premier 2
    if (generateur == -1){
        printf("Erreur recherche de generateur pour p %ld" , prime);
        exit(1);
    }
    ecrire_dans_fichier(argv[2],prime,generateur);
    return 0;
}