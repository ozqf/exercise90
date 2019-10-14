#ifndef TEST_BLOB_LIST_H
#define TEST_BLOB_LIST_H

//#include "../ze_blob_list.h"
#include "../ze_lookup_table.h"
#include "../ze_blob_array.h"

struct Blob
{
    ZEBlobHeader header;
    i32 foo;
    i32 bar;
};

static void Test_PrintBlobArray(ZEBlobArray* arr)
{
    printf("--- Blob array (%d/%d capacity, %d blob size) ---\n",
        arr->m_numBlobs, arr->m_maxBlobs, arr->m_blobUserSize);
    for (i32 i = 0; i < arr->m_maxBlobs; ++i)
    {
        Blob* blob = (Blob*)arr->GetByIndexUnchecked(i);
        if (blob->header.valid == YES)
        {
            printf("%d: id %d foo %d bar %d\n", i, blob->header.id, blob->foo, blob->bar);
        }
        else if (i < arr->m_numBlobs)
        {
            printf("%d: id %d awaiting removal\n", i, blob->header.id);
        }
        else
        {
            printf("%d: --\n", i);
        }
    }
}

static void Test_AddToBlobArray(ZEBlobArray* arr, i32 id, i32 foo, i32 bar)
{
    ZEBlobHeader* h;
    ErrorCode err = arr->GetFree(&h, id);
    ((Blob*)h)->foo = foo;
    ((Blob*)h)->bar = bar;
}

static void Test_RemoveFromBlobArray(ZEBlobArray* arr, i32 index)
{
    ErrorCode err = arr->MarkForFree(index);
    printf("Remove at %d\n", index);
}

static void Test_BlobArray()
{
    ErrorCode err;
    printf("Test blob array\n");
    i32 capacity = 6;
    ZEBlobArray* arr = NULL;
    err = ZE_CreateBlobArray(&arr, capacity, sizeof(Blob), 0);
    if (err != COM_ERROR_NONE)
    {
        printf("Error %d creating blob array\n", err);
        return;
    }
    printf("Created Blob array. Capacity %d, total bytes %d\n",
        capacity, arr->m_end - arr->m_blobs);
    
    //ZEBlobHeader* h;
    Test_AddToBlobArray(arr, 1, 111, 222);
    Test_AddToBlobArray(arr, 2, 333, 444);
    Test_AddToBlobArray(arr, 3, 555, 666);
    Test_PrintBlobArray(arr);
    
    Test_RemoveFromBlobArray(arr, 0);
    Test_PrintBlobArray(arr);
    printf("Truncate array\n");
    arr->Truncate();
    Test_PrintBlobArray(arr);

    Test_RemoveFromBlobArray(arr, 1);
    Test_RemoveFromBlobArray(arr, 0);
    printf("Truncate again\n");
    arr->Truncate();
    Test_PrintBlobArray(arr);
}

static void Test_PrintLookupKeys(ZELookupTable* table)
{
    printf("--- KEYS (%d)---\n", table->m_numKeys);
    for (i32 i = 0; i < table->m_maxKeys; ++i)
    {
        ZELookupKey* key = &table->m_keys[i];
        if (key->id == ZE_LT_INVALID_ID)
        {
            printf("%d: -\n", i);
            continue;
        }
        printf("%d: id %d, data %d, collisions: %d, hash %u\n",
            i, key->id, key->data, key->collisionsOnInsert, key->idHash
        );
    }
}

static void Test_FindLookupData(ZELookupTable* table, i32 id)
{
    i32 data = table->FindData(id);
    if (data == table->m_invalidDataValue)
    {
        printf("No data for id %d found\n", id);
        return;
    }
    printf("Data id %d: %d\n", id, data);
}

static void Test_RemoveLookupKey(ZELookupTable* table, i32 id)
{
    i32 err = table->Remove(id);
    if (err != COM_ERROR_NONE)
    {
        printf("Error %d removing id %d\n", err, id);
        return;
    }
    printf("Removed id %d - num keys %d\n", id, table->m_numKeys);
}

static void Test_LookupTable()
{
    i32 capacity = 6 * 2;
    //ZELookupTable* table = ZE_LT_Create(capacity, 0, NULL);
    ZELookupTable* table = ZE_LT_Create(capacity, 0, (u8*)malloc(ZE_LT_CalcBytesForTable(capacity)));
    i32 testId = 1;
    while (testId <= 6)
    {
        table->Insert(testId, 999);
        testId++;
    }
    Test_PrintLookupKeys(table);
    Test_FindLookupData(table, 2);
    Test_FindLookupData(table, 3);
    Test_FindLookupData(table, 1);

    Test_RemoveLookupKey(table, 2);
    Test_RemoveLookupKey(table, 4);

    table->Insert(testId++, 999);
    table->Insert(testId++, 999);

    Test_RemoveLookupKey(table, 6);
    Test_RemoveLookupKey(table, 1);
    Test_RemoveLookupKey(table, 3);
    table->Insert(testId++, 999);
    Test_RemoveLookupKey(table, 5);
    Test_RemoveLookupKey(table, 9);
    Test_PrintLookupKeys(table);

    Test_FindLookupData(table, 5);
    Test_FindLookupData(table, 8);
}

static void Test_BlobsAndLookupTable()
{
    Test_LookupTable();
    printf("\n");
    Test_BlobArray();
}

#endif // TEST_BLOB_LIST_H