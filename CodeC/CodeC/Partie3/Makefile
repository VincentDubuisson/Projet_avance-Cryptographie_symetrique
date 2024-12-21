# Nom des exécutables
TARGET1 = logiciel
TARGET2 = crack

# Compilateur
CC = gcc

# Options du compilateur
CFLAGS = -Wall -Wextra -std=gnu99

# Liste des fichiers sources pour chaque exécutable
SRCS1 = sym_crypt_final.c cypher.c list.c break_code_c1.c xor.c
SRCS2 = crack_mask.c xor.c

# Liste des fichiers objets (générés à partir des sources)
OBJS1 = $(SRCS1:.c=.o)
OBJS2 = $(SRCS2:.c=.o)

# Règle par défaut : compilation des deux exécutables
all: $(TARGET1) $(TARGET2)

# Règle pour créer l'exécutable "logiciel"
$(TARGET1): $(OBJS1)
	$(CC) $(CFLAGS) -o $(TARGET1) $(OBJS1)

# Règle pour créer l'exécutable "crack"
$(TARGET2): $(OBJS2)
	$(CC) $(CFLAGS) -o $(TARGET2) $(OBJS2)

# Règle pour compiler les fichiers objets
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage des fichiers objets et des exécutables
clean:
	rm -f $(OBJS1) $(OBJS2) $(TARGET1) $(TARGET2)

# Exécuter le programme "logiciel" après compilation
run: $(TARGET1)
	./$(TARGET1)

# Exécuter le programme "crack" après compilation
run-crack: $(TARGET2)
	./$(TARGET2)
