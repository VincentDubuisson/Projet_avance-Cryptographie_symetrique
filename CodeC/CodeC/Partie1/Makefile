# Nom de l'exécutable
TARGET = sym_crypt

# Compilateur
CC = gcc

# Options du compilateur
CFLAGS = -Wall -Wextra -std=gnu99

# Liste des fichiers sources
SRCS = sym_crypt.c

# Nom du fichier objet (généré à partir des sources)
OBJS = $(SRCS:.c=.o)

# Règle par défaut : compilation de l'exécutable
all: $(TARGET)

# Règle pour créer l'exécutable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Règle pour compiler les fichiers objets
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage des fichiers objets et de l'exécutable
clean:
	rm -f $(OBJS) $(TARGET)

# Exécuter le programme après compilation
run: all
	./$(TARGET)