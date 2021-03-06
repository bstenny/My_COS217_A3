/*
 * Created by Blake Stenstrom on 3/22/22.
*/

#include <assert.h>
#include <stdlib.h>
#include "symtable.h"
#include <string.h>

/* bucket count array */
static int bucketCounts[] = {509, 1021, 2039, 4093, 8191, 16381, 32749, 65521};

/* node for symbol table */
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
    struct symTableNode **psFirstNode;
    /* number of nodes in the symbol table */
    int numNodes;
    /* pointer to the buckets */
    int *buckets;
};

SymTable_T SymTable_new(void)
{
    int i;
    SymTable_T oSymTable;

    oSymTable = (SymTable_T)malloc(sizeof(struct symTable));
    if (oSymTable == NULL) {
        return NULL;
    }
    oSymTable->buckets = bucketCounts;
    oSymTable->psFirstNode = (struct symTableNode**)malloc(oSymTable->buckets[0]*sizeof(struct symTableNode*));
    if (oSymTable->psFirstNode == NULL) {
        free(oSymTable);
        return NULL;
    }
    for (i = 0; i < oSymTable->buckets[0]; i++) {
        oSymTable->psFirstNode[i] = NULL;
    }
    oSymTable->numNodes = 0;
    return oSymTable;
}

void SymTable_free(SymTable_T oSymTable)
{
    struct symTableNode *psCurrentNode;
    struct symTableNode *psNextNode;
    int i;

    assert(oSymTable != NULL);

    for(i = 0; i < oSymTable->buckets[0]; i++) {
        psCurrentNode = oSymTable->psFirstNode[i];
        while (psCurrentNode) {
            psNextNode = psCurrentNode;
            psCurrentNode = psCurrentNode->psNextNode;
            if (psNextNode->pvKey) {
                free((void*)(psNextNode->pvKey));
            }
                free (psNextNode);
            }
        }
    free(oSymTable->psFirstNode);
    free(oSymTable);
}

size_t SymTable_getLength(SymTable_T oSymTable) {
    assert(oSymTable != NULL);
    return (size_t)(oSymTable->numNodes);
}

/* Return a hash code for pcKey that is between 0 and uBucketCount-1,
        inclusive. */
static size_t SymTable_hash(const char *pcKey, size_t uBucketCount)
{
    const size_t HASH_MULTIPLIER = 65599;
    size_t u;
    size_t uHash = 0;

    assert(pcKey != NULL);

    for (u = 0; pcKey[u] != '\0'; u++)
        uHash = uHash * HASH_MULTIPLIER + (size_t)pcKey[u];

    return uHash % uBucketCount;
}


/*static void SymTable_rehash(SymTable_T oSymTable) {
    int i;
    int hash;
    struct symTableNode *oldNode;
    struct symTableNode *tempNode;

    *//* I don't have very good ideas with this. I think I ran out of time to really give this a good shot. *//*
}*/

int SymTable_put(SymTable_T oSymTable, const char *pcKey, const void *pvValue)
{
    int hash;
    struct symTableNode *psNewNode;
    struct symTableNode *tempNode;
    char* tempKey;
    tempKey = (char*)malloc(strlen(pcKey) + 1);
    if (tempKey == NULL) {
        return 0;
    }
    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    /* maybe do something different here? */
    /*if (oSymTable->numNodes >= oSymTable->buckets[0] && oSymTable->buckets[0] < 65521) {
        SymTable_rehash(oSymTable);
    }*/
    hash = SymTable_hash(pcKey, (size_t)(oSymTable->buckets[0]));
    psNewNode = oSymTable->psFirstNode[hash];

    if(psNewNode != NULL && strcmp(psNewNode->pvKey, pcKey) == 0) { /* edge case */
        return 0;
    }
    while (psNewNode != NULL && strcmp(psNewNode->pvKey, pcKey) != 0) {
        psNewNode = psNewNode->psNextNode;
    }
    if (psNewNode == NULL) {
        tempNode = (struct symTableNode*)malloc(sizeof(struct symTableNode));
        if (tempNode == NULL) {
            free(tempKey);
            return 0;
        }

        strcpy(tempKey, pcKey);

        tempNode->pvKey = tempKey;
        tempNode->pvValue = pvValue;
        tempNode->psNextNode = oSymTable->psFirstNode[hash];
        oSymTable->psFirstNode[hash] = tempNode;
        oSymTable->numNodes++;
        return 1;
    }
    else {
        return 0;
    }
}

void *SymTable_replace(SymTable_T oSymTable, const char *pcKey,
                       const void *pvValue) {
    struct symTableNode *psNewNode;
    void *value;
    int hash;
    assert(oSymTable != NULL);
    assert(pcKey != NULL);


    hash = SymTable_hash(pcKey, (size_t)(oSymTable->buckets[0]));
    psNewNode = oSymTable->psFirstNode[hash];


    if (psNewNode != NULL && strcmp(psNewNode->pvKey, pcKey) == 0) { /* edge case */
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
    if (psNewNode == NULL) {
        return NULL;
    }
    else {
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

    int hash;
    struct symTableNode *psNewNode;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    if (oSymTable->numNodes == 0) {
        return 0;
    }

    hash = SymTable_hash(pcKey, (size_t)(oSymTable->buckets[0]));
    psNewNode = oSymTable->psFirstNode[hash];
    if(psNewNode != NULL && strcmp(psNewNode->pvKey, pcKey) == 0) { /* edge case */
        return 1;
    }
    while (psNewNode != NULL && strcmp(psNewNode->pvKey, pcKey) != 0) {
        psNewNode = psNewNode->psNextNode;
    }
    if (psNewNode == NULL) {
        return 0;
    }
    else {
        return 1;
    }
}

void *SymTable_get(SymTable_T oSymTable, const char *pcKey) {

    int hash;
    struct symTableNode *psNewNode;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    if (oSymTable->numNodes == 0) {
        return NULL;
    }
    hash = SymTable_hash(pcKey, (size_t)(oSymTable->buckets[0]));
    psNewNode = oSymTable->psFirstNode[hash];

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
    int hash;
    struct symTableNode *prevNode;
    void *value;
    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    hash = SymTable_hash(pcKey, (size_t)(oSymTable->buckets[0]));
    if (oSymTable->numNodes == 0) {
        return NULL;
    }
    psNewNode = oSymTable->psFirstNode[hash];
    if (psNewNode == NULL) {
        return NULL;
    }
    if (strcmp(psNewNode->pvKey, pcKey) == 0) {
        value = (void*)(psNewNode->pvValue);
        oSymTable->psFirstNode[hash] = psNewNode->psNextNode;
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
    if (psNewNode == NULL) {
        return NULL;
    }
    else {
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

    int i;
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
    for(i = 0; i < oSymTable->buckets[0]; i++) {
        psNewNode = oSymTable->psFirstNode[i];
        if (psNewNode == NULL) {
            continue;
        }
        pfApply((const char *) psNewNode->pvKey, (void *)psNewNode->pvValue, (void *)pvExtra); /* edge case */
        while (psNewNode->psNextNode) {
            psNewNode = psNewNode->psNextNode;
            pfApply((const char *) psNewNode->pvKey, (void *)psNewNode->pvValue, (void *)pvExtra);
        }
    }
}









