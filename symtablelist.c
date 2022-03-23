/*
 * Created by Blake Stenstrom on 3/15/22.
*/

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "symtable.h"
#include <string.h>



struct symTableNode
{
    /* The key. */
    const void *pvKey;

    /* The value */
    const void *pvValue;

    /* The address of the next symTableNode. */
    struct symTableNode *psNextNode;
};

struct symTable
{
    /* The address of the first symTableNode. */
    struct symTableNode *psFirstNode;
    /* number of nodes in the symbol table */
    int numNodes;
};

SymTable_T SymTable_new(void)
{
    SymTable_T oSymTable;
    oSymTable = (SymTable_T)malloc(sizeof(struct symTable));
    if (oSymTable == NULL) {
        return NULL;
    }
    oSymTable->psFirstNode = NULL;
    oSymTable->numNodes = 0;
    return oSymTable;
}

void SymTable_free(SymTable_T oSymTable)
{
    struct symTableNode *psCurrentNode;
    struct symTableNode *psNextNode;

    assert(oSymTable != NULL);

    for (psCurrentNode = oSymTable->psFirstNode;
         psCurrentNode != NULL;
         psCurrentNode = psNextNode)
    {
        psNextNode = psCurrentNode->psNextNode;
        free(psCurrentNode);
    }

    free(oSymTable);
}

size_t SymTable_getLength(SymTable_T oSymTable) {
    assert(oSymTable != NULL);
    return oSymTable->numNodes;
}


int SymTable_put(SymTable_T oSymTable, const char *pcKey, const void *pvValue)
{
    struct symTableNode *psNewNode;
    char* tempKey;
    tempKey = (char *)malloc(strlen(pcKey));
    if (tempKey == NULL) {
        return 0;
    }
    assert(oSymTable != NULL);

    psNewNode = (struct symTableNode*)malloc(sizeof(struct symTableNode));
    if (psNewNode == NULL) {
        return 0;
    }

    if (SymTable_contains(oSymTable, pcKey) == 1) {
        return 0;
    }

    strcpy(tempKey, pcKey);
    psNewNode->pvKey = tempKey;
    psNewNode->pvValue = pvValue;
    psNewNode->psNextNode = oSymTable->psFirstNode;
    oSymTable->psFirstNode = psNewNode;
    oSymTable->numNodes++;
    return 1;
}

void *SymTable_replace(SymTable_T oSymTable, const char *pcKey,
                       const void *pvValue) {
    struct symTableNode *psNewNode;
    void *val;
    assert(oSymTable != NULL);
    assert(pcKey != NULL);
    psNewNode = oSymTable->psFirstNode;
    if (!strcmp(psNewNode->pvKey, pcKey)) { /* edge case */
        if (pvValue == NULL) {
            val = psNewNode->pvValue;
            psNewNode->pvValue = NULL;
            return val;
        }
        else {
            val = psNewNode->pvValue;
            psNewNode->pvValue = (void *) pvValue;
            return val;
        }
    }
    while(psNewNode->psNextNode) {
        psNewNode = psNewNode->psNextNode;
        if (!strcmp(psNewNode->pvKey, pcKey)) {
            if (pvValue == NULL) {
                val = psNewNode->pvValue;
                psNewNode->pvValue = NULL;
                return val;
            }
            else {
                val = psNewNode->pvValue;
                psNewNode->pvValue = (void *) pvValue;
                return val;
            }
        }
    }
    return NULL;
}

int SymTable_contains(SymTable_T oSymTable, const char *pcKey) {

    struct symTableNode *psNewNode;
    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    if (oSymTable->numNodes == 0) {
        return 0;
    }
    psNewNode = oSymTable->psFirstNode;
    if(!strcmp(psNewNode->pvKey, pcKey)) { /* edge case */
        return 1;
    }
    while (psNewNode->psNextNode) {
        psNewNode = psNewNode->psNextNode;
        if (!strcmp(psNewNode->pvKey, pcKey)) {
            return 1;
        }
    }
    return 0;
}

void *SymTable_get(SymTable_T oSymTable, const char *pcKey) {

    struct symTableNode *psNewNode;
    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    if (oSymTable->numNodes == 0) {
        return 0;
    }
    psNewNode = oSymTable->psFirstNode;
    if(psNewNode != NULL && strcmp(psNewNode->pvKey, pcKey) == 0) { /* edge case */
        return (void *) psNewNode->pvValue;
    }
    while (psNewNode != NULL && strcmp(psNewNode->pvKey, pcKey) != 0) {
        psNewNode = psNewNode->psNextNode;
    }
    if (psNewNode == NULL) {
        return NULL;
    }
    else {
        return (void *) psNewNode->pvValue;
    }
}

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey) {

    struct symTableNode *psNewNode;
    struct symTableNode *prevNode;
    void *value;
    assert(oSymTable != NULL);
    assert(pcKey != NULL);
    /* assert(oSymTable->psFirstNode != NULL); */
    if (oSymTable->numNodes == 0) {
        return NULL;
    }

    psNewNode = oSymTable->psFirstNode;
    if (psNewNode != NULL && strcmp(psNewNode->pvKey, pcKey) == 0) {
        value = psNewNode->pvValue;
        oSymTable->psFirstNode = psNewNode->psNextNode;
        free(psNewNode->pvKey);
        free(psNewNode);
        oSymTable->numNodes--;
        return value;
    }
    prevNode = psNewNode;
    psNewNode = psNewNode->psNextNode;
    while (psNewNode != NULL && strcmp(psNewNode->pvKey, pcKey) != 0) {
        prevNode = psNewNode;
        psNewNode = psNewNode->psNextNode;
    }
    if (psNewNode == NULL) {
        return NULL;
    }
    else {
        value = psNewNode->pvValue;
        prevNode->psNextNode = psNewNode->psNextNode;
        free(psNewNode->pvKey);
        free(psNewNode);
        oSymTable->numNodes--;
        return value;
    }


    /*
     * newnode = symtable->first
     * if newnode not null and strcmp(osymtable->first->key, pcKey) == 0:
     *      remove that node:
     *      storing info from symtable first node
     *      osymtable->first = noewnode->next
     *
     *
     * prev = newnode
     * newnode = newnode->next
     * while newnode is not NULL and while strcmp(newnode->key, pcKey) != 0 {
     *  prev = newnode
     *  newnode = newnode->next
     *
     * }
     *
     * if newnode is null:
     *    return null (we didnt find te key)
     *
     * else:
     *     store info from newnode
     *     prev->next = newnode->next
     *     free newnode
     *
     *
     *
     *
     *
     *
     *
     *
     */
}

void SymTable_map(SymTable_T oSymTable,
                  void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
                  const void *pvExtra) {

    struct symTableNode *psNewNode;
    assert(oSymTable != NULL);
    psNewNode = (struct symTableNode*)malloc(sizeof(struct symTableNode));
    if (psNewNode == NULL) {
        return NULL;
    }
    if (oSymTable->numNodes == 0) {
        return NULL;
    }
    psNewNode = oSymTable->psFirstNode;
    pfApply((const char *) psNewNode->pvKey, (void *)psNewNode->pvValue, (void *)pvExtra); /* edge case */
    while (psNewNode->psNextNode) {
        psNewNode = psNewNode->psNextNode;
        pfApply((const char *) psNewNode->pvKey, (void *)psNewNode->pvValue, (void *)pvExtra);
    }
}











