#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include "list.h"

#define RED "\x1B[31m"
#define NRM "\x1B[0m"


// Initialise la liste avec les caractères possible de la clé
void init_list(List* list) {
    for (char c = '0'; c <= '9'; c++) {
        list = list_push_back(list, c);
    }
    for (char c = 'a'; c <= 'z'; c++) {
        list = list_push_back(list, c);
    }
    for (char c = 'A'; c <= 'Z'; c++) {
        list = list_push_back(list, c);
    }
}

bool is_valid_car(unsigned char car) {
    return isalnum(car) || isspace(car) || ispunct(car);
}

bool is_accented_char(unsigned char car) {
    return car >= 192;
}



long break_code_c1(FILE *input, int key_length) {
    unsigned char octet;
    int file_cursor = 0;
    long nb_key = 1;

    List* key_lists[key_length]; // Matrice contenant les listes de caractèred valide

    // Créer autant de listes qu'il y a de caractèred dans la clé et les initialise
    for (int i = 0; i < key_length; i++) {
        key_lists[i] = list_create();
        init_list(key_lists[i]);
    }

    while ((octet = (unsigned char)fgetc(input)) != (unsigned char)EOF) {

        unsigned char decrypted_octet = octet;

        /* On vérifie si on a une suite de caractères correspondant à un caractère spécial (0x00 ou de 0x80 à 0xFF).
        Les caractères spéciaux sont mal convertis dans un fichier txt et apparaissent sous cette forme:
        \00 en trois octets équivalent à 0x00, \8D = 0x8D, etc.
        */
        if (octet == 0x5C) { // 0x5C correspond à '\'
            unsigned char first_digit, second_digit;

            // Vérification spécifique pour la séquence \00
            if (fread(&first_digit, sizeof(unsigned char), 1, input) == 1 && first_digit == '0') {
                if (fread(&second_digit, sizeof(unsigned char), 1, input) == 1 && second_digit == '0') {
                    // On a détecté la séquence \00
                    decrypted_octet = 0x00;
                } else {
                    // Si ce n'est pas \00, revenir à la position initiale
                    fseek(input, -1, SEEK_CUR); // Reculer d'un octet
                }
            } 
            // Vérification pour les séquences hexadécimales \8D, \FF, etc.
            else if ((first_digit >= '8' && first_digit <= '9') || (first_digit >= 'A' && first_digit <= 'F')) {
                if (fread(&second_digit, sizeof(unsigned char), 1, input) == 1 &&
                    ((second_digit >= '0' && second_digit <= '9') || (second_digit >= 'A' && second_digit <= 'F'))) {

                    // Conversion des deux caractères hexadécimaux en un seul octet
                    unsigned char high_nibble = (first_digit >= 'A') ? (first_digit - 'A' + 10) : (first_digit - '0');
                    unsigned char low_nibble = (second_digit >= 'A') ? (second_digit - 'A' + 10) : (second_digit - '0');

                    decrypted_octet = (high_nibble << 4) | low_nibble; // Combinaison en un octet
                } else {
                    // Si le second caractère n'est pas valide, réinitialise la position du fichier
                    fseek(input, -1, SEEK_CUR);
                }
            } else {
                // Si le premier caractère n'est pas valide, réinitialise la position du fichier
                fseek(input, -1, SEEK_CUR);
            }
        }

        int mod = file_cursor % key_length;
        int l_size = list_size(key_lists[mod]);

        // Parcours de chaque caractère valide dans la liste pour cette position modulo
        for (int i = 0; i < l_size; i++) {
            unsigned char key = list_at(key_lists[mod], i);
            unsigned char result = (unsigned char)(decrypted_octet ^ key);

            // Si accent détecté dans fichier servant pour le cryptage, erreur
            if (is_accented_char(result)) {
                printf("\n%s[ERREUR] un caractère accentué a été détecté. Veuillez utiliser un fichier crypté résultant d'un texte sans accents.%s\n\n", RED, NRM);
                // Libére la mémoire allouée à la matrice
                for (int i = 0; i < key_length; i++) {
                    list_delete(&key_lists[i]);
                }
                return -2;
            }

            // Vérifier si le caractère déchiffré est invalide
            if (!is_valid_car(result)) {
                // Si invalide, supprime la clé candidate
                list_remove_at(key_lists[mod], i);
                l_size--;
                i--;
            }
        }
        file_cursor++;

        // Cherche une liste vide
        for (int i = 0; i < key_length; i++) {
            if (list_is_empty(key_lists[i])) {

                // Affichage des liste afin de constater l'echec
                printf("\n");
                for (int i = 0; i < key_length; i++) {
                    printf("Clé [%d] : ", i);
                    for (int j = 0; j < list_size(key_lists[i]); j++) {
                        printf("%c ", list_at(key_lists[i], j));
                    }
                    printf("\n");
                }
                printf("\n%s[ECHEC] une liste de caractère de la clé est vide%s\n\n", RED, NRM);
                
                for (int i = 0; i < key_length; i++) {
                    list_delete(&key_lists[i]);
                }
                return -1;
            }
        }
    }

    printf("\nListe des caractères possible pour chaque caractère de la clé: \n");

    // Affichage des liste afin de vérifier si la clé est possible
    printf("\n");
    for (int i = 0; i < key_length; i++) {
        printf("Clé [%d] : ", i);
        for (int j = 0; j < list_size(key_lists[i]); j++) {
            printf("%c ", list_at(key_lists[i], j));
        }
        printf("\n");
    }

    // Compte le nombre de clé potentielles, produit cartésien des listes
    for (int i = 0; i < key_length; i++) {
        int length = list_size(key_lists[i]);

        if (nb_key >= 1000000000000) {
            nb_key = 1000000000000;
            break;
        }
        nb_key *= length;
    }

    // Libére la mémoire allouée à la matrice
    for (int i = 0; i < key_length; i++) {
        list_delete(&key_lists[i]);
    }

    if (nb_key > 1000000000000) {
        printf("\nNombre total de clé potentielles > 1 000 000 000 000 (mille milliards)\n\n");
    } else {
        printf("\nNombre total de clé potentielles: %ld\n\n", nb_key);
    }
    
    return nb_key;

}
