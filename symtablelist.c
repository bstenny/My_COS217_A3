/*
 * Created by Blake Stenstrom on 3/15/22.
*/

#include <assert.h>
#include <stdlib.h>
#include "symtable.h"
#include <string.h>

struct symTableNode
{
    /* The key. */
    const void *pvKey;

    /* The value */
    const void *pvValue;

    /* The address of the next symTableNode. */
    struct SymTableNode *psNextNode;
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

    assert(oSymTable != NULL);

    psNewNode = (struct SymTableNode*)malloc(sizeof(struct SymTableNode));
    if (psNewNode == NULL)
        return 0;

    psNewNode->pvKey = pvKey;
    psNewNode->pvValue = pvValue;
    psNewNode->psNextNode = oSymTable->psFirstNode;
    oSymTable->psFirstNode = psNewNode;
    oSymTable->numNodes++;
    return 1;
}

void *SymTable_replace(SymTable_T oSymTable, const char *pcKey,
                       const void *pvValue) {
    struct symTableNode *psNewNode;
    assert(oSymTable != NULL);
    psNewNode = oSymTable->psFirstNode;
    while(psNewNode->psNextNode) {
        psNewNode = psNewNode->psNextNode;
        if (!strcmp(psNewNode->pvKey, pcKey)) {
            void *val = psNewNode->pvValue;
            psNewNode->pvValue = (void *)pvValue;
            return val;
        }
    }
    return NULL;
}

int SymTable_contains(SymTable_T oSymTable, const char *pcKey) {

    struct symTableNode *psNewNode;
    assert(oSymTable != NULL);
    psNewNode = oSymTable->psFirstNode;
    while (psNewNode->psNextNode) {
        psNewNode = psNewNode->psNextNode;
        if (!strcmp(psNewNode->key, pcKey)) {
            return 1;
        }
    }
    return 0;
}

void *SymTable_get(SymTable_T oSymTable, const char *pcKey) {

    struct symTableNode *psNewNode;
    assert(oSymTable != NULL);
    psNewNode = oSymTable->psFirstNode;
    while (psNewNode->psNextNode) {
        psNewNode = psNewNode->psNextNode;
        if (!strcmp(psNewNode->key, pcKey)) {
            return (void *) psNewNode->pvValue;
        }
    }
    return NULL;
}

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey) {

    struct symTableNode *psNewNode;
    struct symTableNode *node;
    void *value;
    assert(oSymTable != NULL);
    psNewNode = oSymTable->psFirstNode;
    while (psNewNode->psNextNode) {
        if (!strcmp(psNewNode->psNextNode->pvKey, pcKey)) {
            node = psNewNode->psNextNode;
            value = node->pvValue;
            psNewNode->psNextNode = node->psNextNode;
            free(node->pvKey);
            free(node);
            oSymTable->numNodes--;
            return value;
        }
        psNewNode = psNewNode->psNextNode;
    }
    return NULL;
}

void SymTable_map(SymTable_T oSymTable,
                  void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
                  const void *pvExtra) {

    struct symTableNode *psNewNode;
    assert(oSymTable != NULL);
    psNewNode = oSymTable->psFirstNode;
    while (psNewNode->psNextNode) {
        psNewNode = psNewNode->psNextNode;
        pfApply((const char *) psNewNode->pvKey, (void *)psNewNode->pvValue, (void *)pvExtra);
    }
}











