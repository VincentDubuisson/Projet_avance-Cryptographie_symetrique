#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "cypher.h"
#include "xor.h"
#include "break_code_c1.h"
#include "list.h"

#define MAX_KEYS 100
#define BLOCK_SIZE 16

#define KEYS_FILE "keys.txt"

typedef struct {
    char *key;
    int used;
} Key;

Key keys[MAX_KEYS];
int key_count = 0;

//Fonctions utilitaires aux clefs
void check_and_update_key(const char *key);
void save_keys_to_file();
void load_keys_from_file();

//Fonctions relatives aux logiciel final
void print_help();
void list_keys();
void generate_key(int n);
void delete_key(const char *key);
void encrypt(const char *in, const char *out, const char *key, const char *method, const char *iv);
void decrypt(const char *in, const char *out, const char *key, const char *method, const char *iv);
void crack(const char *in, const char *out, int length);


// Fonctions de chiffrement
void encrypt_cbc(const char *in, const char *out, const char *key, const char *iv);
void encrypt_xor(const char *in, const char *out, const char *key);
void encrypt_mask(const char *in, const char *out);

// Fonctions de déchiffrement
void decrypt_cbc(const char *in, const char *out, const char *key, const char *iv);
void decrypt_xor(const char *in, const char *out, const char *key);

//Fonction de log
void log_action(const char *action, const char *details, const char *method, const char *in_file, const char *out_file, const char* extra_info);

int main() {

    load_keys_from_file();
    srand(time(NULL));
    char command[256];

    printf("Bienvenue dans le programme de cryptographie. Tapez 'help' pour voir les commandes disponibles.\n");

    while (1) {
        printf("> ");
        if (fgets(command, sizeof(command), stdin) == NULL) {
            break;
        }

        // Supprimer le saut de ligne
        command[strcspn(command, "\n")] = '\0';

        if (strcmp(command, "help") == 0) {
            print_help();
        } else if (strncmp(command, "list-keys", 9) == 0) {
            list_keys();
        } else if (strncmp(command, "gen-key", 7) == 0) {
            int n;
            if (sscanf(command + 8, "%d", &n) == 1) {
                generate_key(n);
            } else {
                fprintf(stderr, "Erreur: spécifiez une longueur pour la clé.\n");
            }
        } else if (strncmp(command, "del-key", 7) == 0) {
            char key[256];
            if (sscanf(command + 8, "%s", key) == 1) {
                delete_key(key);
            } else {
                fprintf(stderr, "Erreur: spécifiez une clé à supprimer.\n");
            }
        } else if (strncmp(command, "encrypt", 7) == 0) {
            char in[256], out[256], key[256], method[256], iv[256];
            iv[0] = '\0';
            if (sscanf(command + 8, "%s %s %s %s %s", in, out, key, method, iv) >= 4) {
                encrypt(in, out, key, method, iv[0] ? iv : NULL);
            } else {
                fprintf(stderr, "Erreur: paramètres incorrects pour 'encrypt'.\n");
            }
        } else if (strncmp(command, "decrypt", 7) == 0) {
            char in[256], out[256], key[256], method[256], iv[256];
            iv[0] = '\0';
            if (sscanf(command + 8, "%s %s %s %s %s", in, out, key, method, iv) >= 4) {
                decrypt(in, out, key, method, iv[0] ? iv : NULL);
            } else {
                fprintf(stderr, "Erreur: paramètres incorrects pour 'decrypt'.\n");
            }
        } else if (strncmp(command, "crack", 5) == 0) {
            char in[256], out[256];
            int length;
            if (sscanf(command + 6, "%s %s %d", in, out, &length) == 3) {
                crack(in, out, length);
            } else {
                fprintf(stderr, "Erreur: paramètres incorrects pour 'crack'.\n");
            }
        } else if (strcmp(command, "quit") == 0) {
            printf("Au revoir !\n");
            break;
        } else {
            fprintf(stderr, "Commande inconnue. Tapez 'help' pour voir la liste des commandes disponibles.\n");
        }
    }

    return 0;
}


// Fonction pour sauvegarder les clés dans un fichier
void save_keys_to_file() {
    FILE *file = fopen(KEYS_FILE, "wb");
    if (!file) {
        fprintf(stderr, "Erreur: Impossible d'ouvrir le fichier pour sauvegarder les clés.\n");
        return;
    }

    for (int i = 0; i < key_count; i++) {
        fprintf(file, "%s %d\n", keys[i].key, keys[i].used);
    }

    fclose(file);
    printf("Clés sauvegardées dans le fichier '%s'.\n", KEYS_FILE);
}


// Fonction pour charger les clés depuis un fichier
void load_keys_from_file() {
    FILE *file = fopen(KEYS_FILE, "rb");
    if (!file) {
        printf("Aucun fichier de clés trouvé. Un nouveau sera créé lors de la sauvegarde.\n");
        return;
    }

    char key[256];
    int used;

    while (fscanf(file, "%255s %d", key, &used) == 2) {
        if (key_count < MAX_KEYS) {
            keys[key_count].key = strdup(key);
            keys[key_count].used = used;
            key_count++;
        } else {
            fprintf(stderr, "Erreur: Nombre maximum de clés atteint en chargeant les données.\n");
            break;
        }
    }

    fclose(file);
    printf("Clés chargées depuis le fichier '%s'.\n", KEYS_FILE);
}

//Fonction permettant de mettre à jour l'état des clefs (disponible ou pas) présentes dans la lsite de clef ded "list-keys"
void check_and_update_key(const char *key) {
    int found = 0;

    // Vérification si la clé existe déjà dans la liste
    for (int i = 0; i < key_count; i++) {
        if (strcmp(keys[i].key, key) == 0) {
            // Si la clé est trouvée et non utilisée, on la marque comme utilisée
            if (keys[i].used == 0) {
                keys[i].used = 1;
                printf("Clé '%s' marquée comme utilisée.\n", key);
            } else {
                printf("Clé '%s' déjà utilisée.\n", key);
            }
            found = 1;
            break;
        }
    }

    // Si la clé n'a pas été trouvée, on l'ajoute à la liste avec l'état utilisé
    if (!found) {
        if (key_count < MAX_KEYS) {
            keys[key_count].key = strdup(key); // Duplique la chaîne pour l'ajouter à la liste
            keys[key_count].used = 1; // Marque la clé comme utilisée
            key_count++;
            printf("Clé '%s' ajoutée et marquée comme utilisée.\n", key);
        } else {
            fprintf(stderr, "Erreur: Nombre maximum de clés atteint.\n");
        }
    }

    save_keys_to_file();
}

//Fonction qui liste les commandes disponible ainsi que leurs paramètres
void print_help() {
    printf("Commandes disponibles:\n");
    printf("  help: Affiche cette aide\n");
    printf("  list-keys: Liste les clés générées et leur statut\n");
    printf("  gen-key <n>: Génère une clé de longueur n\n");
    printf("  del-key <key>: Supprime une clé\n");
    printf("  encrypt <in> <out> <key> <method> [<iv> taille = 16]: Chiffre un fichier (method = cbc [iv oblig.], xor [iv = null], mask [key=null, iv = null])\n");
    printf("  decrypt <in> <out> <key> <method> [<iv> taille = 16]: Déchiffre un fichier (method = cbc [iv oblig.], xor [iv = null])\n");
    printf("  crack <in> <out> <length>: Determine les clefs potentielles\n");
    printf("  quit: Quitte le programme\n");
    log_action("help", "Consultation de l'aide.", NULL, NULL, NULL, NULL);
}


//Fonction qui liste les clefs créer par le logiciel lors de l'appel de "list-keys"
void list_keys() {
    printf("Liste des clés:\n");

    // Construire la chaîne de caractères pour extra_info
    char extra_info[1024] = ""; // Taille suffisante pour contenir les clés (ajustez si besoin)
    for (int i = 0; i < key_count; i++) {
        char key_info[256];
        snprintf(key_info, sizeof(key_info), "%s (utilisée: %s)\n", keys[i].key, keys[i].used ? "oui" : "non");
        strcat(extra_info, key_info);
        printf("%s", key_info);
    }

    // Appeler log_action avec la liste des clés comme extra_info
    log_action("list-keys", "Consultation des clés disponibles", NULL, NULL, NULL, extra_info);
}

//Fonction pour générer une clef aléatoire de longueur n lors de l'appel de "gen-key"
void generate_key(int n) {
    if (key_count >= MAX_KEYS) {
        fprintf(stderr, "Erreur: Nombre maximum de clés atteint.\n");
        return;
    }
    char *new_key = malloc(n + 1);
    if (new_key == NULL) {
        fprintf(stderr, "Erreur: Impossible d'allouer de la mémoire pour la clé.\n");
        return;
    }
    gen_key(n, new_key);

    keys[key_count].key = new_key;
    keys[key_count].used = 0;
    key_count++;

    printf("Clé générée: %s\n", new_key);
    log_action("gen-key", "Génération d'une clef", NULL, NULL, NULL, new_key);
    save_keys_to_file();
}


//Fonction pour supprimer la clef passé en argument lors de l'appel de "del-key"
void delete_key(const char *key) {
    for (int i = 0; i < key_count; i++) {
        if (strcmp(keys[i].key, key) == 0) {
            free(keys[i].key);
            keys[i] = keys[key_count - 1];
            key_count--;
            printf("Clé supprimée: %s\n", key);
            log_action("del-key", "Suppression d'une clef", NULL, NULL, NULL, key);
            save_keys_to_file();
            return;
        }
    }
    fprintf(stderr, "Erreur: Clé introuvable.\n");
}


//Fonction qui cible la fonction de chiffrement adapter selon la methode utilisé lors de l'appel de "encrypt"
void encrypt(const char *in, const char *out, const char *key, const char *method, const char *iv) {

    if (strcmp(method, "cbc") == 0) {
        check_and_update_key(key);
        encrypt_cbc(in, out, key, iv);
    } else if (strcmp(method, "xor") == 0) {
        check_and_update_key(key);
        encrypt_xor(in, out, key);
    } else if (strcmp(method, "mask") == 0) {
        encrypt_mask(in, out);
    } else {
        fprintf(stderr, "Erreur: Méthode de chiffrement inconnue '%s'.\n", method);
    }
    log_action("encrypt", "Chiffrement effectué", method, in, out, NULL);
}


//Fonction qui cible la fonction de dechiffrement adapter selon la methode utilisé lors de l'appel de "decrypt"
void decrypt(const char *in, const char *out, const char *key, const char *method, const char *iv) {

    if (strcmp(method, "cbc") == 0) {
        check_and_update_key(key);
        decrypt_cbc(in, out, key, iv);
    } else if (strcmp(method, "xor") == 0) {
        check_and_update_key(key);
        decrypt_xor(in, out, key);
    } else {
        fprintf(stderr, "Erreur: Méthode de déchiffrement inconnue '%s'.\n", method);
    }
    log_action("decrypt", "Déchiffrement effectué", method, in, out, NULL);
}


//Fonction permettant de trouver l'ensemble des clefs possible et donc de cracker la clef utilisé pour chiffré in lors de l'appel de "crack"
void crack(const char *in, const char *out, int length) {
    // Ouvrir le fichier d'entrée
    FILE *input = fopen(in, "rb");
    if (!input) {
        printf("[ERREUR] Impossible d'ouvrir le fichier d'entrée : %s\n", in);
        return;
    }

    long nb_keys = break_code_c1(input, length); // break_code_c1 renvoie le nombre de clés potentielles
    fclose(input); // Fermer le fichier d'entrée après utilisation

    // Gérer les cas d'échec
    if (nb_keys == -1) {
        printf("[ECHEC] Une liste de caractères de la clé est vide. Annulation du processus.\n");
        return;
    } else if (nb_keys == -2) {
        printf("[ERREUR] Caractère accentué détecté. Utilisez un fichier crypté sans accents.\n");
        return;
    }

    // Ouvrir le fichier de sortie
    FILE *output = fopen(out, "a");
    if (!output) {
        printf("[ERREUR] Impossible d'ouvrir le fichier de sortie : %s\n", out);
        return;
    }

    // --> (à compléter pour stoker la liste de caractère de la clef par element de cette derniere)
    //Écrire les informations dans le fichier de sortie
    fprintf(output, "%ld\n", nb_keys);

    fclose(output);
    printf("[SUCCÈS] Le craquage est terminé. Résultats enregistrés dans : %s\n", out);

    // Loguer l'action
    log_action("crack", in, NULL, NULL, NULL, out);

}


//Implémentation de la méthode de chiffrement CBC
void encrypt_cbc(const char *in, const char *out, const char *key, const char *iv) {
    FILE *input = fopen(in, "rb");
    FILE *output = fopen(out, "wb");
    if (!input || !output) {
        fprintf(stderr, "Erreur: Impossible d'ouvrir les fichiers %s ou %s.\n", in, out);
        if (input) fclose(input);
        if (output) fclose(output);
        return;
    }

    printf("Chiffrement CBC de %s -> %s avec clé %s et IV %s\n", in, out, key, iv);

    // Appel à cbc_crypt
    cbc_crypt(input, output, (char *)key, (char *)iv);

    fclose(input);
    fclose(output);
}


//Implémentation de la méthode de chiffrement XOR
void encrypt_xor(const char *in, const char *out, const char *key) {
    FILE *input = fopen(in, "rb");
    FILE *output = fopen(out, "wb");
    if (!input || !output) {
        fprintf(stderr, "Erreur: Impossible d'ouvrir les fichiers %s ou %s.\n", in, out);
        if (input) fclose(input);
        if (output) fclose(output);
        return;
    }

    printf("Chiffrement XOR de %s -> %s avec clé %s\n", in, out, key);

    // Lecture et traitement du fichier par blocs
    char buffer[BLOCK_SIZE];
    char encrypted[BLOCK_SIZE];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, BLOCK_SIZE, input)) > 0) {
        // Appliquer la fonction XOR
        xor(buffer, (char *)key, bytes_read, encrypted);
        fwrite(encrypted, 1, bytes_read, output);
    }

    fclose(input);
    fclose(output);
}


//Implémentation de la méthode de chiffrement MASK
void encrypt_mask(const char *in, const char *out) {
    printf("Chiffrement Mask de %s -> %s\n", in, out);

    FILE *input = fopen(in, "rb");
    FILE *output = fopen(out, "wb");
    if (!input || !output) {
        fprintf(stderr, "Erreur: Impossible d'ouvrir les fichiers %s ou %s.\n", in, out);
        if (input) fclose(input);
        if (output) fclose(output);
        return;
    }

    // Lecture du contenu d'entrée
    fseek(input, 0, SEEK_END);
    long file_size = ftell(input);
    rewind(input);

    char *buffer = malloc(file_size);
    if (!buffer) {
        fprintf(stderr, "Erreur: Allocation mémoire échouée\n");
        fclose(input);
        return;
    }

    fread(buffer, 1, file_size, input);
    fclose(input);

    // Allocation pour le contenu masqué
    char *masked = malloc(file_size);
    if (!masked) {
        fprintf(stderr, "Erreur: Allocation mémoire échouée\n");
        free(buffer);
        return;
    }

    // Application de la fonction mask
    mask(buffer, file_size, masked);

    // Écriture dans le fichier de sortie
    fwrite(masked, 1, file_size, output);
    fclose(output);

    // Libération des ressources
    free(buffer);
    free(masked);

    printf("Chiffrement Mask terminé avec succès.\n");
}


//Implémentation de la méthode de dechiffrement CBC
void decrypt_cbc(const char *in, const char *out, const char *key, const char *iv) {
    FILE *input = fopen(in, "rb");
    FILE *output = fopen(out, "wb");
    if (!input || !output) {
        fprintf(stderr, "Erreur: Impossible d'ouvrir les fichiers %s ou %s.\n", in, out);
        if (input) fclose(input);
        if (output) fclose(output);
        return;
    }

    printf("Chiffrement CBC de %s -> %s avec clé %s et IV %s\n", in, out, key, iv);

    // Appel à cbc_crypt
    cbc_uncrypt(input, output, (char *)key, (char *)iv);

    fclose(input);
    fclose(output);
}


//Implémentation de la méthode de chiffrement XOR
void decrypt_xor(const char *in, const char *out, const char *key) {
  FILE *input = fopen(in, "rb");
  FILE *output = fopen(out, "wb");
  if (!input || !output) {
      fprintf(stderr, "Erreur: Impossible d'ouvrir les fichiers %s ou %s.\n", in, out);
      if (input) fclose(input);
      if (output) fclose(output);
      return;
  }

  printf("Déchiffrement XOR de %s -> %s avec clé %s\n", in, out, key);

  // Lecture et traitement du fichier par blocs
  char buffer[BLOCK_SIZE];
  char uncrypted[BLOCK_SIZE];
  size_t bytes_read;
  while ((bytes_read = fread(buffer, 1, BLOCK_SIZE, input)) > 0) {
      // Appliquer la fonction XOR
      xor(buffer, (char *)key, bytes_read, uncrypted);
      fwrite(uncrypted, 1, bytes_read, output);
  }

  fclose(input);
  fclose(output);
}


//Fonction permettant d'avoir un suivis des commandes utilisées ainsi que leurs resultats
void log_action(const char *action, const char *details, const char *method, const char *in_file, const char *out_file, const char *extra_info) {
    FILE *log_fp = fopen("log.txt", "a");
    if (log_fp) {
        // Ajout de l'horodatage
        time_t now = time(NULL);
        struct tm *t = localtime(&now);
        fprintf(log_fp, "%02d/%02d/%04d %02d:%02d:%02d - %s: %s\n",
                t->tm_mday, t->tm_mon + 1, t->tm_year + 1900, t->tm_hour, t->tm_min, t->tm_sec,
                action, details);

        if (method) fprintf(log_fp, "Méthode: %s\n", method);

        // Affichage spécifique selon la commande
        if (strcmp(action, "gen-key") == 0) {
            // Journalisation pour la commande gen-key
            if (extra_info) {
                fprintf(log_fp, "Clé générée: %s\n", extra_info);
            }
        } else if (strcmp(action, "encrypt") == 0 || strcmp(action, "decrypt") == 0) {
            // Journalisation pour les commandes encrypt et decrypt
            if (in_file) fprintf(log_fp, "Fichier d'entrée: %s\n", in_file);
            if (out_file) fprintf(log_fp, "Fichier de sortie: %s\n", out_file);

        } else if (strcmp(action, "list-keys") == 0) {
            // Journalisation pour la commande list-keys
            fprintf(log_fp, "Liste des clés disponibles:\n");
            if (extra_info) {
                fprintf(log_fp, "%s\n", extra_info);  // Afficher les clés listées
            }
        } else if (strcmp(action, "crack") == 0) {
            // Journalisation pour la commande crack
            if (extra_info) {
                fprintf(log_fp, "Clé trouvée: %s\n", extra_info);
            }
        }

        fprintf(log_fp, "\n");
        fclose(log_fp);
    }
}
