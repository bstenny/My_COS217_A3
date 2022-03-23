/*
 * Created by Blake Stenstrom on 3/22/22.
*/

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "symtable.h"
#include <string.h>

static int bucketCounts[] = {509, 1021, 2039, 4093, 8191, 16381, 32749, 65521};

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
    oSymTable->psFirstNode = (struct symTableNode*)malloc(oSymTable->buckets[0]*sizeof(struct symTableNode));
    if (oSymTable->psFirstNode == NULL) {
        return NULL;
    }
    for (i = 0; i < oSymTable->buckets[0]; i++) {
        oSymTable->psFirstNode[i].psNextNode = NULL;
        oSymTable->psFirstNode[i].pvKey = NULL;
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
        psCurrentNode = oSymTable->psFirstNode[i].psNextNode;
        while (psCurrentNode) {
            psNextNode = psCurrentNode;
            psCurrentNode = psCurrentNode->psNextNode;
            if (psNextNode->pvKey) {
                free(psNextNode->pvKey);
            }
                free (psNextNode);
            }
        }
    free(oSymTable->psFirstNode);
    free(oSymTable);
}

size_t SymTable_getLength(SymTable_T oSymTable) {
    assert(oSymTable != NULL);
    return oSymTable->numNodes;
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

static void SymTable_rehash(SymTable_T oSymTable) {
    int i;
    int hash;
    struct symTableNode *oldNode;
    struct symTableNode *tempNode;
    struct symTableNode *psNewNode;
    struct symTableNode *node;
    int oldBuckets;

    oldNode = oSymTable->psFirstNode;
    oldBuckets = oSymTable->buckets[0];
    (oSymTable->buckets)++;

    oSymTable->psFirstNode = (struct symTableNode*)malloc(oSymTable->buckets[0]*sizeof(struct symTableNode));
    if (oSymTable->psFirstNode == NULL) {
        oSymTable->psFirstNode = oldNode;
        (oSymTable->buckets)--;
        return;
    }
    for (i = 0; i < oSymTable->buckets[0]; i++) {
        oSymTable->psFirstNode[i].psNextNode = NULL;
        oSymTable->psFirstNode[i].pvKey = NULL;
    }

    for (i = 0; i < oldBuckets; i++) {
        node = oldNode[i].psNextNode;
        while (node) {
            tempNode = node->psNextNode;
            hash = SymTable_hash(node->pvKey, oSymTable->buckets[0]);
            psNewNode = &(oSymTable->psFirstNode[hash]);
            while (psNewNode->psNextNode) {
                psNewNode = psNewNode->psNextNode;
            }
            psNewNode->psNextNode = node;
            node->psNextNode = NULL;
            node = tempNode;
        }

    }
    free(oldNode);
}

int SymTable_put(SymTable_T oSymTable, const char *pcKey, const void *pvValue)
{
    int hash;
    struct symTableNode *psNewNode;
    char* tempKey;
    tempKey = (char*)malloc(strlen(pcKey) + 1);
    if (tempKey == NULL) {
        return 0;
    }
    assert(oSymTable != NULL);

    psNewNode = (struct symTableNode*)malloc(sizeof(struct symTableNode));
    if (psNewNode == NULL) {
        return 0;
    }
    /* maybe do something different here? Do I need another function? */
    if (oSymTable->numNodes >= oSymTable->buckets[0] && oSymTable->buckets[0] < 65521) {
        SymTable_rehash(oSymTable);
    }
    hash = SymTable_hash(pcKey, oSymTable->buckets[0]);
    psNewNode = &(oSymTable->psFirstNode[hash]);

    if (SymTable_contains(oSymTable, pcKey) == 1) {
        return 0;
    }


    psNewNode->psNextNode = (struct symTableNode*)malloc(sizeof(struct symTableNode));
    if (psNewNode->psNextNode == NULL) {
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
    int hash;
    assert(oSymTable != NULL);
    psNewNode = (struct symTableNode*)malloc(sizeof(struct symTableNode));
    if (psNewNode == NULL) {
        return NULL;
    }
    hash = SymTable_hash(pcKey, oSymTable->buckets[0]);
    psNewNode = &(oSymTable->psFirstNode[hash]);

    /*
    if (!strcmp(psNewNode->pvKey, pcKey)) { /* edge case */ /*
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
    */
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

    int hash;
    struct symTableNode *psNewNode;

    assert(oSymTable != NULL);
    psNewNode = (struct symTableNode*)malloc(sizeof(struct symTableNode));
    
    if (psNewNode == NULL) {
        return 0;
    }
    if (oSymTable->numNodes == 0) {
        return 0;
    }
    hash = SymTable_hash(pcKey, oSymTable->buckets[0]);
    psNewNode = &(oSymTable->psFirstNode[hash]);
    while (psNewNode->psNextNode) {
        if (!strcmp(psNewNode->pvKey, pcKey)) {
            return 1;
        }
        psNewNode = psNewNode->psNextNode;
    }
    return 0;
}

void *SymTable_get(SymTable_T oSymTable, const char *pcKey) {

    int hash;
    struct symTableNode *psNewNode;

    assert(oSymTable != NULL);

    psNewNode = (struct symTableNode*)malloc(sizeof(struct symTableNode));
    if (psNewNode == NULL) {
        return NULL;
    }
    if (oSymTable->numNodes == 0) {
        return 0;
    }
    hash = SymTable_hash(pcKey, oSymTable->buckets[0]);
    psNewNode = &(oSymTable->psFirstNode[hash]);

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
    int hash;
    struct symTableNode *prevNode;
    void *value;
    assert(oSymTable != NULL);
    assert(pcKey != NULL);
    /* assert(oSymTable->psFirstNode != NULL); */

    hash = SymTable_hash(pcKey, oSymTable->buckets[0]);
    if (oSymTable->numNodes == 0) {
        return NULL;
    }
    psNewNode = &(oSymTable->psFirstNode[hash]);
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
    if (oSymTable->numNodes == 1) {
        if(!strcmp(psNewNode->pvKey, pcKey)) { /* edge case */ /*
            nextNode = psNewNode;
            value = psNewNode->pvValue;
            oSymTable->psFirstNode = NULL;
            free(nextNode->pvKey);
            free(nextNode);
            oSymTable->numNodes--;
            return value;
        }
    }
    while (psNewNode->psNextNode){
        if (!strcmp(psNewNode->pvKey, pcKey)) { /* edge case */ /*
            nextNode = psNewNode->psNextNode;
            value = nextNode->pvValue;
            psNewNode->psNextNode = nextNode->psNextNode;
            free(nextNode->pvKey);
            free(nextNode);
            oSymTable->numNodes--;
            return value;
        }
        psNewNode = psNewNode->psNextNode;
    }
    /*
    if (!strcmp(psNewNode->pvKey, pcKey)) { /* edge case */
    /*
        nextNode = psNewNode;
        value = nextNode->pvValue;
        psNewNode->psNextNode = nextNode->psNextNode;
        free(nextNode->pvKey);
        free(nextNode);
        oSymTable->numNodes--;
        return value;
    }
    while (psNewNode->psNextNode) {
        if (!strcmp(psNewNode->psNextNode->pvKey, pcKey)) {
            nextNode = psNewNode->psNextNode;
            value = nextNode->pvValue;
            psNewNode->psNextNode = nextNode->psNextNode;
            free(nextNode->pvKey);
            free(nextNode);
            oSymTable->numNodes--;
            return value;
        }
        psNewNode = psNewNode->psNextNode;
    }
    */
    return NULL;
}

void SymTable_map(SymTable_T oSymTable,
                  void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
                  const void *pvExtra) {

    int i;
    struct symTableNode *psNewNode;
    assert(oSymTable != NULL);
    psNewNode = (struct symTableNode*)malloc(sizeof(struct symTableNode));
    if (psNewNode == NULL) {
        return NULL;
    }
    if (oSymTable->numNodes == 0) {
        return NULL;
    }
    pfApply((const char *) psNewNode->pvKey, (void *)psNewNode->pvValue, (void *)pvExtra); /* edge case */
    for(i = 0; i < oSymTable->buckets[0]; i++) {
        psNewNode = &(oSymTable->psFirstNode[i]);
        while (psNewNode->psNextNode) {
            psNewNode = psNewNode->psNextNode;
            pfApply((const char *) psNewNode->pvKey, (void *)psNewNode->pvValue, (void *)pvExtra);
        }
    }
}









