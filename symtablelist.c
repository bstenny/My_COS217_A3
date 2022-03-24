/*
 * Created by Blake Stenstrom on 3/15/22.
*/

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "symtable.h"
#include <string.h>


/* node for this symbol table */
struct symTableNode
{
    /* The key. */
    const void *pvKey;

    /* The value */
    const void *pvValue;

    /* The address of the next symTableNode. */
    struct symTableNode *psNextNode;
};

/* symbol table structure */
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
    return (size_t)oSymTable->numNodes;
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
    assert(pcKey != NULL);

    psNewNode = (struct symTableNode*)malloc(sizeof(struct symTableNode));
    if (psNewNode == NULL) {
        free(tempKey);
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
    void *value;
    assert(oSymTable != NULL);
    assert(pcKey != NULL);
    psNewNode = oSymTable->psFirstNode;

    if (psNewNode != NULL && strcmp(psNewNode->pvKey, pcKey) == 0) { /* first node case */
        if (pvValue == NULL) {
            value = (void*)(psNewNode->pvValue);
            psNewNode->pvValue = NULL;
            return value;
        }
        else {
            value = (void*)(psNewNode->pvValue);
            psNewNode->pvValue = (void *) pvValue;
            return value;
        }
    }
    while(psNewNode != NULL && strcmp(psNewNode->pvKey, pcKey) != 0) {
        psNewNode = psNewNode->psNextNode;
    }
    if (psNewNode == NULL) { /*we did not find it */
        return NULL;
    }
    else { /* we found it */
        if (pvValue == NULL) {
            value = (void*)(psNewNode->pvValue);
            psNewNode->pvValue = NULL;
            return value;
        }
        else {
            value = (void*)(psNewNode->pvValue);
            psNewNode->pvValue = (void *) pvValue;
            return value;
        }
    }
}

int SymTable_contains(SymTable_T oSymTable, const char *pcKey) {

    struct symTableNode *psNewNode;
    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    if (oSymTable->numNodes == 0) {
        return 0;
    }
    psNewNode = oSymTable->psFirstNode;
    if(strcmp(psNewNode->pvKey, pcKey) == 0) { /* edge case */
        return 1;
    }

    while(psNewNode != NULL && strcmp(psNewNode->pvKey, pcKey) != 0) {
        psNewNode = psNewNode->psNextNode;
    }
    if (psNewNode == NULL) { /* it does not contain */
        return 0;
    }
    else { /* it contains */
        return 1;
    }
}

void *SymTable_get(SymTable_T oSymTable, const char *pcKey) {

    struct symTableNode *psNewNode;
    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    if (oSymTable->numNodes == 0) {
        return NULL;
    }
    psNewNode = oSymTable->psFirstNode;
    if(psNewNode != NULL && strcmp(psNewNode->pvKey, pcKey) == 0) { /* edge case */
        return (void *) psNewNode->pvValue;
    }
    while (psNewNode != NULL && strcmp(psNewNode->pvKey, pcKey) != 0) {
        psNewNode = psNewNode->psNextNode;
    }
    if (psNewNode == NULL) { /* it was not in the symtable */
        return NULL;
    }
    else { /* we found it */
        return (void *) psNewNode->pvValue;
    }
}

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey) {

    struct symTableNode *psNewNode;
    struct symTableNode *prevNode;
    void *value;
    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    if (oSymTable->numNodes == 0) {
        return NULL;
    }

    psNewNode = oSymTable->psFirstNode;
    if (psNewNode == NULL) {
        return NULL;
    }
    if (strcmp(psNewNode->pvKey, pcKey) == 0) { /* first node case */
        value = (void*)(psNewNode->pvValue);
        oSymTable->psFirstNode = psNewNode->psNextNode;
        free((void*)(psNewNode->pvKey));
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
    if (psNewNode == NULL) { /* we did not find the node with pcKey */
        return NULL;
    }
    else { /* we found it */
        value = (void*)(psNewNode->pvValue);
        prevNode->psNextNode = psNewNode->psNextNode;
        free((void*)(psNewNode->pvKey));
        free(psNewNode);
        oSymTable->numNodes--;
        return value;
    }
}

void SymTable_map(SymTable_T oSymTable,
                  void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
                  const void *pvExtra) {

    struct symTableNode *psNewNode;
    assert(oSymTable != NULL);
    assert(pfApply != NULL);
    psNewNode = (struct symTableNode*)malloc(sizeof(struct symTableNode));
    if (psNewNode == NULL) {
        return;
    }
    if (oSymTable->numNodes == 0) {
        return;
    }
    psNewNode = oSymTable->psFirstNode;
    pfApply((const char *) psNewNode->pvKey, (void *)psNewNode->pvValue, (void *)pvExtra); /* edge case */
    while (psNewNode->psNextNode) {
        psNewNode = psNewNode->psNextNode;
        pfApply((const char *) psNewNode->pvKey, (void *)psNewNode->pvValue, (void *)pvExtra);
    }
}











