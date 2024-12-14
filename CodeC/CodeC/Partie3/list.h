#ifndef __LIST_H__
#define __LIST_H__

#include <stdbool.h>

/* Structure de données provenant de l'UE ALGO3 (Ref: Mathias Paulin IRIT),
  modifiée pour convenir aux usages de l'algorithme d'émondage */

/*-----------------------------------------------------------------*/

/** \defgroup ADTList List
 Documentation of the implementation of the abstract data type List.
 @{
*/

/** \defgroup Type Type definition.
  @{
*/
/** Opaque definition of type List.
*/
typedef struct s_List List;
/** Définition of type ptrList : pointer to a List.
*/
typedef List* ptrList;
/** @}*/

/*-----------------------------------------------------------------*/

/** \defgroup Constructors Contructors and destructors of the TAD.
 @{
*/
/** Implementation of the the constructor \c list from the specification.
*/
List* list_create(void);

/** Implementation of the the constructor \c push_back from the specification.
 @param l The list to modify
 @param v The value to add
 @return The modified list
 Add the value v at the end of the list l.
 @note This function acts by side effect on the parameter l. The returned value is the same as the parameter l that is modified by the function.
*/
List* list_push_back(List* l, unsigned char v);

/** Destructor.
	Added by the implementation. Free ressources allocated by constructors.
 	@param l the adress of the list.
 	After calling this function, the list l becomes NULL.
*/
void list_delete(ptrList* l);
/** @}*/


/** Remove an element at a given position.
	 @param l The list to modify.
	 @param p The position of the element to be removed.
	 @return The modified list.
	 Remove the element located at position .
	 @pre 0 <= p < list_size(l)
	 @note This function acts by side effect on the parameter l. The returned value is the same as the parameter l that is modified by the function.
*/
List* list_remove_at(List* l, int p);

/** Acces to an element at a given position.
	 @param l The list to acces.
	 @param p The position to acces.
	 @return The value of the element at position p.
	 @pre 0 <= p < list_size(l)
*/
unsigned char list_at(const List* l, int p);
/** @}*/

/*-----------------------------------------------------------------*/

/** \defgroup UtilityOperators Operators allowing to access some properties or apply some processing on the whole list.
 @{
*/
/** Test if a list is empty.
*/
bool list_is_empty(const List* l);

/** Give the number of elements of the list.
*/
int list_size(const List* l);

/** @}*/

/** @}*/

#endif

