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
    printf("Adding blob Id %d\n", list->nextId);
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
            //printf("%d: --\n", i);
            return;
        }
        printf("%d: Id %d, hash %u, index %d\n",
            i, key->id, key->hash, key->index);
    }
}

static void BlobTest_PrintAll(BlobList* list)
{
    printf("===== Blob List Contents =====\n");
    BlobTest_PrintKeys(list);
    printf("--- Blobs ---\n");
    for (i32 i = 0; i < list->numBlobs; ++i)
    {
        Blob* blob = (Blob*)BL_GetByIndex(list, i);
        printf("Index %d, Id %d, hash %u - Foo %d Bar %d\n",
            i, blob->header.id, blob->header.hash,
            blob->foo, blob->bar
        );
    }
}

static void TestBlobList()
{
    ErrorCode err;
    printf("Test blob list\n");
    BlobList* list;
    i32 capacity = 12;
    err = BL_Create(sizeof(Blob), capacity, &list);
    if (err != COM_ERROR_NONE)
    {
        printf("Error %d creating blob list\n", err);
        return;
    }
    printf("Created blob list, capacity %d\n", capacity);
    BlobTest_Add(list, 7, 17);
    //BlobTest_Add(list, 27, 37);
    //BlobTest_Add(list, 47, 57);
    BlobTest_PrintAll(list);
    BlobHeader* header = BL_GetById(list, 2);
    if (header != NULL)
    {
        Blob* blob = (Blob*)header;
        printf("Found 2: foo %d, bar %d\n", blob->foo, blob->bar);
    }
}

#endif // TEST_BLOB_LIST_H