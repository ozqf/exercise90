#ifndef BLOB_LIST_H
#define BLOB_LIST_H

/*
Blob list - generic data for objects of the same size.
> Currently does not support resizing!
> Blobs should have a BlobHeader as their first member for pointer casting, eg:
struct MyStruct
{
	BlobHeader blobHeader;
	int foo;
	int bar;
	etc...
};
> Blobs are stored in a tight array. A separate hash table is maintained
	to map blob Ids to array index.
*/
#include "../common/common.h"
#include "string.h"

#define BL_BAD_INDEX -1
#define BL_INVALID_ID 0

#define BL_HASH_TABLE_SCALE 1

#define BL_MALLOC(numBytesToAlloc) \
(u8*)malloc(##numBytesToAlloc##)

#define BL_FREE(ptrToFree) \
free(##ptrToFree##)

#define BL_SET_ZERO(ptrToSet, numBytesToZero) \
memset(##ptrToSet##, 0, numBytesToZero##)

#define BL_GET_CAST(blobListPtr, blobIndex, typeForCast) \
(##typeForCast##*)BL_GetByIndex(blobListPtr, blobIndex);

#define BLOB_SENTINEL 0xEFBEADDE

struct BlobLookupKey
{
    i32 id;
    u32 hash;
    i32 index;
    i32 collisionsOnInsert;
};

struct BlobHeader
{
    i32 id;
    u32 hash;
    i32 sentinel;
};

struct BlobList
{
    BlobLookupKey* keys;
    i32 maxKeys;
    u8* blobs;
    i32 blobUserSize;
    i32 maxBlobs;
    i32 numBlobs;
    i32 nextId;
};

// Credit: Thomas Wang
// https://burtleburtle.net/bob/hash/integer.html
internal u32 BL_HashUint(u32 a)
{
	a = (a ^ 61) ^ (a >> 16);
	a = a + (a << 3);
    a = a ^ (a >> 4);
    a = a * 0x27d4eb2d;
    a = a ^ (a >> 15);
    return a;
}

// internal u32 BL_MeasureBytesForLookupTable(u32 numBlobs)
// {
//     return (numBlobs * BL_HASH_TABLE_SCALE) * sizeof(BlobLookupKey);
// }

// internal u32 BL_MeasureTotalBytes(u32 sizeOfABlob, u32 numBlobs)
// {
//     u32 bytesForKeysTable = BL_MeasureBytesForLookupTable(numBlobs);
//     return sizeof(BlobList) + bytesForKeysTable + (sizeOfABlob * numBlobs);
// }

internal ErrorCode BL_Create(u32 sizeOfABlob, u32 initialNumBlobs, BlobList** result)
{
    if (result == NULL) { return COM_ERROR_NULL_ARGUMENT; }
    //u32 sizeForLookupTable = BL_MeasureBytesForLookupTable(initialNumBlobs);
    //u32 totalSize = BL_MeasureTotalBytes(sizeOfABlob, initialNumBlobs);
    u32 lookupTableBytes =
        (initialNumBlobs * BL_HASH_TABLE_SCALE) * sizeof(BlobLookupKey);
    u32 blobsBytes = sizeOfABlob * initialNumBlobs;
    u32 totalSize = sizeof(BlobList) + lookupTableBytes + blobsBytes;

    printf("Creating blob list. %d items, %dB each, %dKB\n",
        initialNumBlobs,
        sizeOfABlob,
        totalSize / 1024
    );
    // Layout in memory:
    // |BlobList Struct|ArrayLookupHashTable...|ArrayOfBlobs...|
    u8* mem = BL_MALLOC(totalSize);
    BL_SET_ZERO(mem, totalSize);
    BlobList* list = (BlobList*)mem;
    list->keys = (BlobLookupKey*)(mem + sizeof(BlobList));
    list->maxKeys = initialNumBlobs * BL_HASH_TABLE_SCALE;

    list->blobs = mem + sizeof(BlobList) + lookupTableBytes;
    list->blobUserSize = sizeOfABlob;
    list->numBlobs = 0;
    list->maxBlobs = initialNumBlobs;
    list->nextId = 1;

    *result = list;

    return COM_ERROR_NONE;
}

internal ErrorCode BL_Destroy(BlobList* list)
{
    if (list == NULL) { return COM_ERROR_NULL_ARGUMENT; }
    void* mem = (void*)list;
    BL_FREE(mem);
}
#if 1
internal BlobHeader* BL_GetById(BlobList* list, i32 id)
{
    u32 hash = BL_HashUint(id);
    i32 keyIndex = hash % list->maxKeys;
    printf("Searching for id %d, hash %u. Starting at %d\n",
        id, hash, keyIndex);
    i32 escape = 0;
    do
    {
        BlobLookupKey* key = &list->keys[keyIndex];
        if (key->id == id)
        {
            // bingo
            printf("  Found at keyIndex %d - result at %d\n",
                keyIndex, key->index);
            return (BlobHeader*)list->blobs[key->index];
        }
        if (key->id == BL_INVALID_ID)
        {
            // 
        }
    } while (++escape < list->maxKeys);
    return NULL;
}
#endif
internal BlobHeader* BL_GetByIndex(BlobList* list, i32 index)
{
    if (index < 0) { return NULL; }
    if (index >= list->maxBlobs) { return NULL; }
    u8* addr = list->blobs + (list->blobUserSize * index);
    return (BlobHeader*)addr;
}

internal ErrorCode BL_RemoveBlob(BlobList* list, i32 id)
{
    return COM_ERROR_NOT_IMPLEMENTED;
}

internal ErrorCode BL_AssignNewBlob(
    BlobList* list, i32 newId, BlobHeader** result)
{
    if (list == NULL) { return COM_ERROR_NULL_ARGUMENT; }
    if (result == NULL) { return COM_ERROR_NULL_ARGUMENT; }
    if (newId == BL_INVALID_ID) { return COM_ERROR_BAD_ARGUMENT; }
    // TODO: realloc:
    if (list->numBlobs == list->maxBlobs) { return COM_ERROR_NO_SPACE; }

    // assign new index
    i32 newBlobIndex = list->numBlobs++;

    // insert to lookup table
    u32 hash = BL_HashUint(newId);
    i32 keyIndex = hash % list->maxKeys;
	printf("Key index: %d\n", keyIndex);

    i32 escape = 0;
    u32 numCollisions = 0;
    do
    {
        BlobLookupKey* key = &list->keys[keyIndex];
        if (key->id == BL_INVALID_ID)
        {
            // Free slot. Easy
            // Set lookup key
            key->id = newId;
            key->hash = hash;
            key->index = newBlobIndex;
            key->collisionsOnInsert = numCollisions;
            printf("Assigning blob id %d to index %d. Hash %u. Key index %d\n",
                key->id, key->index, key->hash, keyIndex
            );

            // Setup blob header
            // TODO: This line is bollocks and wrong!
            BlobHeader* blob = BL_GetByIndex(list, newBlobIndex);
            // this clears the header but the rest of memory is still trash
            *blob = {};
            blob->id = newId;
            blob->hash = hash;
            blob->sentinel = BLOB_SENTINEL;
            *result = blob;
            return COM_ERROR_NONE;
        }
        else if (key->id == newId)
        {
            // Item was already in the list...
            // just find and return it
            printf("Item already in list\n");
            BlobHeader* blob = (BlobHeader*)&list->blobs[key->index];
            *result = blob;

            // Decrement the new blob index first!!
            list->numBlobs--;

            return COM_ERROR_NONE;
        }
        else
        {
            printf("Trying next key\n");
            // occupied - move to next slot and try again
            numCollisions++;
            keyIndex++;
            if (keyIndex >= list->maxKeys)
            {
                keyIndex = 0;
            }
        }
        

    } while (++escape < list->maxKeys);
    
    return COM_ERROR_FUNC_RAN_AWAY;
}

#endif // BLOB_LIST_H