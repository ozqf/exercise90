#ifndef SWEEP_AND_PRUNE_H
#define SWEEP_AND_PRUNE_H

#include "../../common/common.h"

struct SAPAABB
{
    i32 id;
    f32 min[3];
    f32 max[3];
};

struct SAPMarker
{
    i32 id;
    i32 isMax;
    f32 pos;
};

struct SAPAxis
{
    i32 axisIndex; // x/y/z == 0/1/2
    SAPMarker* markers;
    i32 numMarkers;
    i32 maxMarkers;
};

// Globals for testing
#define SAP_MAX_SHAPES 8
// Require two markers per shape
SAPMarker sap_markers[SAP_MAX_SHAPES * 2];
SAPAxis sapX = {};

internal void SAP_Init(SAPAxis* axis)
{
    const int maxShapes = 32;
    *axis = {};
    axis->axisIndex = 0;
    axis->numMarkers = 0;
    axis->maxMarkers = SAP_MAX_SHAPES * 2;
    axis->markers = sap_markers;
    COM_SET_ZERO(sap_markers, sizeof(SAPMarker) * axis->maxMarkers)
}

#endif //SWEEP_AND_PRUNE_H