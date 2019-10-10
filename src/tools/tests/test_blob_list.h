#ifndef TEST_BLOB_LIST_H
#define TEST_BLOB_LIST_H

#include "../ze_blob_list.h"

struct Blob
{
    BlobHeader header;
    i32 foo;
    i32 bar;
};

static void BlobTest_Add(BlobList* list, i32 foo, i32 bar)
{
    BlobHeader* header = NULL;
    //printf("Adding blob Id %d\n", list->nextId);
    ErrorCode err = BL_AssignNewBlob(list, list->nextId++, &header);
    if (err != COM_ERROR_NONE)
    {
        printf("Error %d adding to blob list\n", err);
        return;
    }
    Blob* blob = (Blob*)header;
    blob->foo = foo;
    blob->bar = bar;
}

static void BlobTest_PrintKeys(BlobList* list)
{
    printf("--- Keys ---\n");
    for (i32 i = 0; i < list->maxKeys; ++i)
    {
        BlobLookupKey* key = &list->keys[i];
        if (key->id == BL_INVALID_ID)
        {
            printf("%d: --\n", i);
            continue;
        }
        printf("%d: Id %d, hash %u, index %d, collisions %d\n",
            i, key->id, key->hash, key->index, key->collisionsOnInsert);
    }
}

static void BlobTest_PrintAll(BlobList* list)
{
    printf("===== Blob List Contents =====\n");
    BlobTest_PrintKeys(list);
    printf("--- Blobs ---\n");
    for (i32 i = 0; i < list->maxBlobs; ++i)
    {
        if (i < list->numBlobs)
        {
            Blob* blob = (Blob*)BL_GetByIndex(list, i);
            printf("Index %d, Id %d, hash %u - Foo %d Bar %d\n",
                i, blob->header.id, blob->header.hash,
                blob->foo, blob->bar
            );
        }
        else
        {
            printf("Index %d --\n", i);
        }
        
    }
}

static void TestBlobList()
{
    ErrorCode err;
    printf("Test blob list\n");
    BlobList* list;
    i32 capacity = 6;
    err = BL_Create(sizeof(Blob), capacity, &list);
    if (err != COM_ERROR_NONE)
    {
        printf("Error %d creating blob list\n", err);
        return;
    }
    printf("Created blob list, capacity %d lookup table size %d\n",
		capacity, list->maxKeys);
	
    u32 foo = 7;
    u32 bar = 3;
    for (i32 i = 1; i <= capacity; ++i)
    {
        BlobTest_Add(list, foo * i, bar * i);
    }
    BlobTest_PrintAll(list);
    printf("\n");
    i32 searchForId = 12;
    BlobHeader* header = BL_GetById(list, searchForId);
    if (header != NULL)
    {
        Blob* blob = (Blob*)header;
        printf("Found %d: foo %d, bar %d\n",
            searchForId, blob->foo, blob->bar);
    }
    //i32 removeId = 7;
    printf("===========================\nRemove test\n");
    // TODO: Deleting six here but ended up with only 5 in the blob list!
	err = BL_RemoveById(list, 2);
    err = BL_RemoveById(list, 5);
    err = BL_RemoveById(list, 1);
    for (i32 i = 0; i < 3; ++i)
    {
        BlobTest_Add(list, 0, 0);
    }
    err = BL_RemoveById(list, 4);
    err = BL_RemoveById(list, 3);
    BlobTest_PrintAll(list);
    printf("\n");
}

#endif // TEST_BLOB_LIST_H