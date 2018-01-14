#pragma once

#include "shared.h"

#define VEC_X 0
#define VEC_Y 1
#define VEC_Z 2
#define VEC_W 3

/* Matrix use
OpenGL uses column major, y/x matrices
/   0   4   8   12  \
|   1   5   9   13  |
|   2   6   10  14  |
\   3   7   11  15  /

*/

#define TRANSFORM_MATRIX_SIZE 16
#define X0 0
#define Y0 1
#define Z0 2
#define W0 3

#define X1 4
#define Y1 5
#define Z1 6
#define W1 7

#define X2 8
#define Y2 9
#define Z2 10
#define W2 11

#define X3 12
#define Y3 13
#define Z3 14
#define W3 15

#define abs(value) { if (value < ) return -value; }

void COM_SetMatrix_X(f32* m, f32 x0, f32 x1, f32 x2, f32 x3)
{
    m[X0] = x0; m[X1] = x1; m[X2] = x2; m[X3] = x3;
}

void COM_SetMatrix_Y(f32* m, f32 y0, f32 y1, f32 y2, f32 y3)
{
    m[Y0] = y0; m[Y1] = y1; m[Y2] = y2; m[Y3] = y3;
}

void COM_SetMatrix_Z(f32* m, f32 z0, f32 z1, f32 z2, f32 z3)
{
    m[Z0] = z0; m[Z1] = z1; m[Z2] = z2; m[Z3] = z3;
}

void COM_SetMatrix_W(f32* m, f32 w0, f32 w1, f32 w2, f32 w3)
{
    m[W0] = w0; m[W1] = w1; m[W2] = w2; m[W3] = w3;
}

void COM_MultiplyMatrices(f32* m0, f32* m1, f32* result)
{
    result[X0] = m0[X0] * m1[X0];   result[X1] = m0[X1] * m1[X1];   result[X2] = m0[X2] * m1[X2];   result[X3] = m0[X3] * m1[X3];
    result[Y0] = m0[Y0] * m1[Y0];   result[Y1] = m0[Y1] * m1[Y1];   result[Y2] = m0[Y2] * m1[Y2];   result[Y3] = m0[Y3] * m1[Y3];
    result[Z0] = m0[Z0] * m1[Z0];   result[Z1] = m0[Z1] * m1[Z1];   result[Z2] = m0[Z2] * m1[Z2];   result[Z3] = m0[Z3] * m1[Z3];
    result[W0] = m0[W0] * m1[W0];   result[W1] = m0[W1] * m1[W1];   result[W2] = m0[W2] * m1[W2];   result[W3] = m0[W3] * m1[W3];
}

void COM_CopyMatrix(f32* src, f32* tar)
{
    tar[X0] = src[X0];  tar[X1] = src[X1];  tar[X2] = src[X2];  tar[X3] = src[X3];
    tar[Y0] = src[Y0];  tar[Y1] = src[Y1];  tar[Y2] = src[Y2];  tar[Y3] = src[Y3];
    tar[Z0] = src[Z0];  tar[Z1] = src[Z1];  tar[Z2] = src[Z2];  tar[Z3] = src[Z3];
    tar[W0] = src[W0];  tar[W1] = src[W1];  tar[W2] = src[W2];  tar[W3] = src[W3];

}

void COM_SetToIdentityMatrix(f32* matrix)
{
    matrix[X0] = 1; matrix[X1] = 0; matrix[X2] = 0; matrix[X3] = 0;
    matrix[Y0] = 0; matrix[Y1] = 1; matrix[Y2] = 0; matrix[Y3] = 0;
    matrix[Z0] = 0; matrix[Z1] = 0; matrix[Z2] = 1; matrix[Z3] = 0;
    matrix[W0] = 0; matrix[W1] = 0; matrix[W2] = 0; matrix[W3] = 1;
}

void COM_SetAsScaleMatrix(f32* matrix, f32 scaleX, f32 scaleY, f32 scaleZ)
{
    matrix[X0] = scaleX;
    matrix[Y1] = scaleY;
    matrix[Z2] = scaleZ;
}

void COM_SetMoveMatrix(f32* matrix, f32 deltaX, f32 deltaY, f32 deltaZ)
{
    matrix[X3] = deltaX;
    matrix[Y3] = deltaY;
    matrix[Z3] = deltaZ;
}

void COM_MultiplyVectorByMatrix(f32* m, f32* v)
{
    v[VEC_X] =   (m[X0] * v[VEC_X]) + (m[X1] * v[VEC_Y]) + (m[X2] * v[VEC_Z]) + (m[X3] * v[VEC_W]);
    v[VEC_Y] =   (m[Y0] * v[VEC_X]) + (m[Y1] * v[VEC_Y]) + (m[Y2] * v[VEC_Z]) + (m[Y3] * v[VEC_W]);
    v[VEC_Z] =   (m[Z0] * v[VEC_X]) + (m[Z1] * v[VEC_Y]) + (m[Z2] * v[VEC_Z]) + (m[Z3] * v[VEC_W]);
    v[VEC_W] =   (m[W0] * v[VEC_X]) + (m[W1] * v[VEC_Y]) + (m[W2] * v[VEC_Z]) + (m[W3] * v[VEC_W]);
}
