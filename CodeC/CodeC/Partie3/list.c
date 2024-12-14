#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "list.h"

/* Structure de données provenant de l'UE ALGO3 (Ref: Mathias Paulin IRIT),
  modifiée pour convenir aux usages de l'algorithme d'émondage */

typedef struct s_LinkedElement {
	unsigned char value;
	struct s_LinkedElement* previous;
	struct s_LinkedElement* next;
} LinkedElement;

/* Use of a sentinel for implementing the list :
 The sentinel is a LinkedElement* whose next pointer refer always to the head of the list and previous pointer to the tail of the list
 */
struct s_List {
	LinkedElement* sentinel;
	int size;
};

/*-----------------------------------------------------------------*/

List* list_create(void) {
	List* l;
  LinkedElement* sentinel;

  // Allocation mémoire de la list et de la sentinelle
  l = malloc(sizeof(struct s_List));
  sentinel = malloc(sizeof(struct s_LinkedElement));

  // Initialisation de la sentinelle et de la liste
  sentinel->previous = sentinel->next = sentinel;
  l->sentinel = sentinel;
  l->size = 0;

	return l;
}

/*-----------------------------------------------------------------*/

List* list_push_back(List* l, unsigned char v) {

  //Initialisation de l'élément à ajouter
  LinkedElement* element = malloc(sizeof(struct s_LinkedElement));
  element->value = v;

  //Gestion du double chainage
  element->next = l->sentinel;
  element->previous = element->next->previous;
  element->previous->next = element;
  element->next->previous = element;
  ++(l->size);

	return l;
}

/*-----------------------------------------------------------------*/

void list_delete(List** l) {
    if (l == NULL || *l == NULL) {
        return;
    }

    LinkedElement* current = (*l)->sentinel->next;
    LinkedElement* next;

    // Parcours de la liste et libération des éléments
    while (current != (*l)->sentinel) {
        next = current->next;
        free(current);
        current = next;
    }
    free((*l)->sentinel);
    free(*l);

    *l = NULL;
}

/*-----------------------------------------------------------------*/

List* list_remove_at(List* l, int p) {
  //Itérateur
  LinkedElement *e = l->sentinel->next;

  //Boucle de parcours de la liste jusqu'a l'indice p
  for (int i = 0; i < p; i++) {
    e = e->next;
  }

  //On est à l'indice p, gestion du double chainage
  e->previous->next = e->next;
  e->next->previous = e->previous;

  free(e);
  --(l->size);

	return l;
}

/*-----------------------------------------------------------------*/

unsigned char list_at(const List* l, int p) {
	//Itérateur
	LinkedElement *e = l->sentinel->next;

	//Boucle de parcours de la liste jusqu'a l'indice p
	for (int i = 0; i < p; i++) {
		e = e->next;
	}
	return e->value;
}

/*-----------------------------------------------------------------*/

bool list_is_empty(const List* l) {
	return l->size <= 0;
}

/*-----------------------------------------------------------------*/

int list_size(const List* l) {
	return l->size;
}