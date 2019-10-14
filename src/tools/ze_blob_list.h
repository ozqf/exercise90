#ifndef ZE_BLOB_LIST_H
#define ZE_BLOB_LIST_H

#include "../common/common.h"
#include "ze_lookup_table.h"
#include "ze_blob_array.h"

struct ZEBlobList
{
    ZEBlobArray* array;
};

#endif // ZE_BLOB_LIST_H