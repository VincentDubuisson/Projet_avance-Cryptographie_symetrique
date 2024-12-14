# Nom de l'exécutable
TARGET = break_code

# Compilateur
CC = gcc

# Options du compilateur
CFLAGS = -Wall -Wextra -std=gnu99

# Liste des fichiers sources
SRCS = break_code_main.c break_code_c1.c list.c

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