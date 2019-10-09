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

#define BL_INVALID_INDEX -1
#define BL_INVALID_ID 0

#define BL_HASH_TABLE_SCALE 1

#ifndef BL_MALLOC
#define BL_MALLOC(numBytesToAlloc) \
(u8*)malloc(##numBytesToAlloc##)
#endif

#ifndef BL_FREE
#define BL_FREE(ptrToFree) \
free(##ptrToFree##)
#endif

#ifndef BL_SET_ZERO
#define BL_SET_ZERO(ptrToSet, numBytesToZero) \
memset(##ptrToSet##, 0, numBytesToZero##)
#endif

#ifndef BL_GET_CAST
#define BL_GET_CAST(blobListPtr, blobIndex, typeForCast) \
(##typeForCast##*)BL_GetByIndex(blobListPtr, blobIndex);
#endif

#ifndef BL_COPY
#define BL_COPY(destBlob, sourceBlob, blobSize) \
memcpy((void*)##destBlob##, (void*)##sourceBlob##, blobSize##);
#endif

// for alignment checking
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
static u32 BL_HashUint(u32 a)
{
	a = (a ^ 61) ^ (a >> 16);
	a = a + (a << 3);
    a = a ^ (a >> 4);
    a = a * 0x27d4eb2d;
    a = a ^ (a >> 15);
    return a;
}

//////////////////////////////////////////////////////////////
// Create/Destruction
//////////////////////////////////////////////////////////////
static ErrorCode BL_Create(u32 sizeOfABlob, u32 initialNumBlobs, BlobList** result)
{
    if (result == NULL) { return COM_ERROR_NULL_ARGUMENT; }
    if (sizeOfABlob == 0) { return COM_ERROR_BAD_ARGUMENT; }
    if (initialNumBlobs == 0) { return COM_ERROR_BAD_ARGUMENT; }
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

static ErrorCode BL_Destroy(BlobList* list)
{
    if (list == NULL) { return COM_ERROR_NULL_ARGUMENT; }
    void* mem = (void*)list;
    BL_FREE(mem);
}

//////////////////////////////////////////////////////////////
// Locate
//////////////////////////////////////////////////////////////
static BlobHeader* BL_GetByIndex(BlobList* list, i32 index)
{
    if (index < 0) { return NULL; }
    if (index >= list->maxBlobs) { return NULL; }
    u8* addr = list->blobs + (list->blobUserSize * index);
    return (BlobHeader*)addr;
}

static i32 BL_GetKeyIndexById(BlobList* list, i32 id)
{
    u32 hash = BL_HashUint(id);
    i32 keyIndex = hash % list->maxKeys;
    //printf("Searching for id %d, hash %u. Starting at %d\n",
    //    id, hash, keyIndex);
    i32 escape = 0;
    do
    {
        BlobLookupKey* key = &list->keys[keyIndex];
        if (key->id == id)
        {
            // bingo
            //printf("  Found at keyIndex %d - result at %d\n",
            //    keyIndex, key->index);
            return keyIndex;
        }
        else if (key->id == BL_INVALID_ID)
        {
            // Blank key, stop probing. item is not in the list
            return BL_INVALID_INDEX;
        }
        else
        {
            // probe forward
            keyIndex++;
            if (keyIndex >= list->maxKeys)
            {
                keyIndex = 0;
            }
        }
    } while (++escape < list->maxKeys);
    return BL_INVALID_INDEX;
}

static BlobHeader* BL_GetById(BlobList* list, i32 id)
{
    i32 keyIndex = BL_GetKeyIndexById(list, id);
    if (keyIndex == BL_INVALID_INDEX) { return NULL; }
    return BL_GetByIndex(list, list->keys[keyIndex].index);
}

//////////////////////////////////////////////////////////////
// Insert
//////////////////////////////////////////////////////////////
static ErrorCode BL_InsertLookupKey(BlobList* list, i32 id, u32 hash, i32 index)
{
    i32 keyIndex = hash % list->maxKeys;
    i32 escape = 0;
    u32 numCollisions = 0;
    do
    {
        BlobLookupKey* key = &list->keys[keyIndex];
        if (key->id == BL_INVALID_ID)
        {
            // Free slot. Easy
            // Set lookup key
            key->id = id;
            key->hash = hash;
            key->index = index;
            key->collisionsOnInsert = numCollisions;
            printf("Assigning key id %d to index %d. Hash %u. Key index %d\n",
                key->id, key->index, key->hash, keyIndex
            );
            
            return COM_ERROR_NONE;
        }
        else if (key->id == id)
        {
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

static ErrorCode BL_AssignNewBlob(
    BlobList* list, i32 newId, BlobHeader** result)
{
    if (list == NULL) { return COM_ERROR_NULL_ARGUMENT; }
    if (result == NULL) { return COM_ERROR_NULL_ARGUMENT; }
    if (newId == BL_INVALID_ID) { return COM_ERROR_BAD_ARGUMENT; }
    // TODO: realloc:
    if (list->numBlobs == list->maxBlobs) { return COM_ERROR_NO_SPACE; }

    // assign new index
    i32 newBlobIndex = list->numBlobs++;

    // Setup blob header
    u32 hash = BL_HashUint(newId);
    
    BlobHeader* blob = BL_GetByIndex(list, newBlobIndex);
    // this clears the header but the rest of memory is still trash
    *blob = {};
    blob->id = newId;
    blob->hash = hash;
    blob->sentinel = BLOB_SENTINEL;
    *result = blob;

    // insert to lookup table
    ErrorCode err = BL_InsertLookupKey(list, newId, hash, newBlobIndex);
    if (err != COM_ERROR_NONE)
    {
        printf("Error %d inserting to lookup table\n", err);
    }
    return err;
}

//////////////////////////////////////////////////////////////
// Removal
//////////////////////////////////////////////////////////////
#if 0
static ErrorCode BL_RemoveLookupKey(BlobList* list, i32 id)
{
    u32 hash = BL_HashUint(id);
    i32 keyIndex = hash % list->maxKeys;
    // keys from initial index may have been probed forward in collisions
    // so they must all be reinserted
    i32 initialIndex = keyIndex;
    i32 escape = 0;
    do
    {
        BlobLookupKey* key = &list->keys[keyIndex];
        
    } while (++escape < list->maxKeys);
}
#endif
static ErrorCode BL_RemoveById(BlobList* list, i32 id)
{
    i32 keyIndex = BL_GetKeyIndexById(list, id);
    if (keyIndex == BL_INVALID_INDEX) { return COM_ERROR_NOT_FOUND; }
    i32 itemIndex = list->keys[keyIndex].index;

    if (list->numBlobs == 1)
    {
        // easy
        list->keys[keyIndex] = {};
        list->numBlobs--;
        return COM_ERROR_NONE;
    }

    // Swap the removed blob with this one
    // > find the blob and it's key.
    i32 swapItemIndex = list->numBlobs - 1;
    BlobHeader* swapBlob = BL_GetByIndex(list, swapItemIndex);
    i32 swapKeyIndex = BL_GetKeyIndexById(list, swapBlob->id);

    // > set the key's index to the index of the item we are deleting
    list->keys[swapKeyIndex].index = swapItemIndex;
    printf(">>> Copying blob id %d (index %d) over id %d (index %d) -- %d bytes\n",
        swapBlob->id, swapItemIndex, id, itemIndex, list->blobUserSize);
    
    // > copy swap blob over the deleted blob
    u8* itemAddr = (u8*)(list->blobs + (list->blobUserSize * itemIndex));
    u8* swapAddr = (u8*)(list->blobs + (list->blobUserSize * swapItemIndex));
    printf("\t\taddr %u to %u\n", (u32)swapAddr, (u32)itemAddr);
    BL_COPY(itemAddr, swapAddr, list->blobUserSize);

    // > decrement blob count
    list->numBlobs--;

    // Clear keys
    // > Wipe deleted item key
    BlobLookupKey* key = &list->keys[keyIndex];
    *key = {};
    // probe forward reinserting until a key with an invalid id is found
    keyIndex++;
    if (keyIndex >= list->maxKeys) { keyIndex = 0; }
    key = &list->keys[keyIndex];
    while(key->id != BL_INVALID_ID)
    {
        i32 correctIndex = key->hash % list->maxKeys;
        if (correctIndex != keyIndex)
        {
            // previous collision
            BlobLookupKey copy = *key;
            *key = {};
            BL_InsertLookupKey(list, copy.id, copy.hash, copy.index);
        }
        keyIndex++;
        if (keyIndex >= list->maxKeys) { keyIndex = 0; }
        key = &list->keys[keyIndex];
    }

    return COM_ERROR_NOT_IMPLEMENTED;
}

#endif // BLOB_LIST_H