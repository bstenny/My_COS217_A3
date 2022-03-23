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
    psNewNode = (struct symTableNode*)malloc(sizeof(struct symTableNode));
    if (psNewNode == NULL) {
        return NULL;
    }
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
    psNewNode = (struct symTableNode*)malloc(sizeof(struct symTableNode));
    if (psNewNode == NULL) {
        return 0;
    }
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
    psNewNode = (struct symTableNode*)malloc(sizeof(struct symTableNode));
    if (psNewNode == NULL) {
        return NULL;
    }
    if (oSymTable->numNodes == 0) {
        return 0;
    }
    psNewNode = oSymTable->psFirstNode;
    if(!strcmp(psNewNode->pvKey, pcKey)) { /* edge case */
        return (void *) psNewNode->pvValue;
    }
    while (psNewNode->psNextNode) {
        psNewNode = psNewNode->psNextNode;
        if (!strcmp(psNewNode->pvKey, pcKey)) {
            return (void *) psNewNode->pvValue;
        }
    }
    return NULL;
}

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey) {

    struct symTableNode *psNewNode;
    struct symTableNode *nextNode;
    void *value;
    assert(oSymTable != NULL);
    /* assert(oSymTable->psFirstNode != NULL); */
    psNewNode = (struct symTableNode*)malloc(sizeof(struct symTableNode));
    if (psNewNode == NULL) {
        return NULL;
    }
    nextNode = (struct symTableNode*)malloc(sizeof(struct symTableNode));
    if (nextNode == NULL) {
        return NULL;
    }
    if (oSymTable->numNodes == 0) {
        return NULL;
    }
    psNewNode = oSymTable->psFirstNode;
    if (oSymTable->numNodes == 1) {
        if(!strcmp(psNewNode->pvKey, pcKey)) { /* edge case */
            nextNode = psNewNode;
            value = psNewNode->pvValue;
            oSymTable->psFirstNode = NULL;
            free(nextNode->pvKey);
            free(nextNode);
            oSymTable->numNodes--;
            return value;
        }
    }
    if (!strcmp(psNewNode->pvKey, pcKey)) { /* edge case */
        nextNode = psNewNode;
        value = nextNode->pvValue;
        psNewNode->psNextNode = nextNode->psNextNode;
        free(nextNode->pvKey);
        free(nextNode);
        oSymTable->numNodes--;
        return value;
    }
    while (psNewNode->psNextNode) {
        psNewNode = psNewNode->psNextNode;
        if (!strcmp(psNewNode->psNextNode->pvKey, pcKey)) {
            nextNode = psNewNode->psNextNode;
            value = nextNode->pvValue;
            psNewNode->psNextNode = nextNode->psNextNode;
            free(nextNode->pvKey);
            free(nextNode);
            oSymTable->numNodes--;
            return value;
        }

    }
    return NULL;
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











