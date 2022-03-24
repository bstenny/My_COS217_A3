/*
 * Created by Blake Stenstrom on 3/15/22.
*/

#ifndef MY_COS217_A3_SYMTABLE_H
#define MY_COS217_A3_SYMTABLE_H
#include <stdlib.h>

/* symTable object for SymTable_T*/
typedef struct symTable *SymTable_T;

/* creates a new symTable
 * takes no values
 * returns new Symtable_T
 * */
SymTable_T SymTable_new(void);

/* frees a given symtable
 * takes a SymTable_T
 * no return (void)
 * */
void SymTable_free(SymTable_T oSymTable);

/* gets the length of a given symtable
 * takes a symTable_T
 * returns a size_t which is the length
 * */
size_t SymTable_getLength(SymTable_T oSymTable);

/* inserts a key value pair in a symtable if that key is not already in the symtable
 * takes a symtable_T, a const char *pcKey, and a const void *pvValue
 * returns 1 if the key value pair was put in the symtable and 0 if not
 * */
int SymTable_put(SymTable_T oSymTable, const char *pcKey, const void *pvValue);

/* replaces the value of a given key with the value given to the function
 * takes a symtable_T, a const char *pcKey, and a const void *pvValue
 * no return (void)
 * */
void *SymTable_replace(SymTable_T oSymTable, const char *pcKey,
                       const void *pvValue);
/* figures out if a symtable contains a certain key value pair based on a given key
 * takes a symtable_T, a const char *pcKey
 * returns 1 if the key value pair is in the symtable and 0 if not
 * */
int SymTable_contains(SymTable_T oSymTable, const char *pcKey);

void *SymTable_get(SymTable_T oSymTable, const char *pcKey);

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey);

void SymTable_map(SymTable_T oSymTable,
                  void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
                  const void *pvExtra);

#endif
