#ifndef BVH_H
#define BVH_H

#include "../common/common.h"

#include <math.h>

#define BVH_EMPTY_ID 0
#define BVH_MAX_NODES 8

inline AABB Bvh_Combine(AABB* a, AABB* b)
{
    AABB r = {};
    r.min.x = __min(a->min.x, b->min.x);
    r.min.y = __min(a->min.y, b->min.y);
    r.min.z = __min(a->min.z, b->min.z);
    r.max.x = __max(a->max.x, b->max.x);
    r.max.y = __max(a->max.y, b->max.y);
    r.max.z = __max(a->max.z, b->max.z;
    return r;
}

struct BvhNode
{
    i32 storeIndex;
    i32 left;
    i32 right;
    AABB box;
    i32 objectId;
};

static i32 nextNodeIndex = 0;
static BvhNode nodes[BVH_MAX_NODES];

static BvhNode* root = NULL;

static BvhNode* BVH_AssignFreeNode()
{
    i32 i = nextNodeIndex++;
    if (i >= BVH_MAX_NODES) { /* Do'h */ return NULL; }
    nodes[i] = {};
    nodes[i].storeIndex = i;
    return &nodes[i];
}

static void BVH_Insert(i32 index)
{

}

static void BVH_Test()
{
    BvhNode* root = BVH_AssignFreeNode();
    BvhNode* a = BVH_AssignFreeNode();
    BvhNode* b = BVH_AssignFreeNode();
    a->box.min = { 0, 0, 0 };
    a->box.max = { 10, 10, 10 };
    a->box.min = { 10, 0, 0 };
    a->box.max = { 20, 10, 10 };
    
    root->box.max = { 0, 0, 0 };
    root->box.max = { 20, 10, 10 };

}

#endif //BVH_H
