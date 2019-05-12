#pragma once
/*
Incomplete/abandoned hash table learning exercise
*/
#include "../../common/common.h"

// Prime number!
#define HASH_TABLE_SIZE 17
//#define HASH_TABLE_SIZE 4409

struct HashRecord
{
    i32 used;
    i32 key;
    i32 data;
};

struct PackedRecord
{
    i32 data;
};

HashRecord hashTable[HASH_TABLE_SIZE];

internal void Hash_Reset(HashRecord* records, i32 numRecords)
{
    for(i32 i = 0; i < numRecords; ++i)
    {
        hashTable[i].key = 0;
        hashTable[i].data = -1;
    }
}

inline i32 Hash_Key(i32 key)
{
    return key % HASH_TABLE_SIZE;
}

internal HashRecord* Hash_InsertUnique(
    HashRecord* records, i32 key, i32 data)
{
    // Find slot via key
    i32 index = Hash_Key(key);
    // probe for empty space with no data.
    i32 read = records[index].data;
    while (read != -1)
    {
        index = Hash_Key(index + 1);
        read = records[index].data;
        COM_ASSERT(read < HASH_TABLE_SIZE, "Hash table overflow")
    }
    printf("Storing key %d at index %d\n", key, index);
    records[index].data = data;
    records[index].key = key;
    return &records[index];
}

internal HashRecord* Hash_Search(HashRecord* records, i32 key)
{
    i32 index = Hash_Key(key);
    printf("Searching key %d\n", key);
    while (records[index].data != -1)
    {
        if (records[index].key == key)
        {
            printf("Found key %d at index %d\n", key, index);
            return &records[index];
        }
        index = Hash_Key(index + 1);
    }
    return NULL;
}

void Test_HashPrint(i32 key)
{
    HashRecord* rec = Hash_Search(hashTable, key);
    if (rec != NULL)
    {
        printf("Key %d Value %d\n", key, rec->data);
    }
    else
    {
        printf("Key %d has no assigned value\n", key);
    }
}

void Test_HashTable()
{
    printf("Hash table test\n");

    i32 key = 27;
    i32 index = Hash_Key(key);
    printf("Key %d hashed to index %d\n", key, index);
    key = HASH_TABLE_SIZE + 27;
    index = Hash_Key(key);
    printf("Key %d hashed to index %d\n", key, index);

    Hash_Reset(hashTable, HASH_TABLE_SIZE);
    Hash_InsertUnique(hashTable, 27, 3);
    Hash_InsertUnique(hashTable, 4436, 4);
    Hash_InsertUnique(hashTable, 605, 7);

    Test_HashPrint(27);
    Test_HashPrint(4436);
    Test_HashPrint(42);
    Test_HashPrint(605);
}
