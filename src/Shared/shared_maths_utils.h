#pragma once

#include "shared.h"

#define VEC_X 0
#define VEC_Y 1
#define VEC_Z 2
#define VEC_W 3

// Matrix use
#define TRANSFORM_MATRIX_SIZE 16
#define X0 0
#define X1 1
#define X2 2
#define X3 3

#define Y0 4
#define Y1 5
#define Y2 6
#define Y3 7

#define Z0 8
#define Z1 9
#define Z2 10
#define Z3 11

#define W0 12
#define W1 13
#define W2 14
#define W3 15

#define abs(value) { if (value < ) return -value; }

void COM_SetToIdentityMatrix(f32* matrix)
{
    matrix[X0] = 1;
    matrix[X1] = 0;
    matrix[X2] = 0;
    matrix[X3] = 0;

    matrix[Y0] = 0;
    matrix[Y1] = 1;
    matrix[Y2] = 0;
    matrix[Y3] = 0;

    matrix[Z0] = 0;
    matrix[Z1] = 0;
    matrix[Z2] = 1;
    matrix[Z3] = 0;

    matrix[W0] = 0;
    matrix[W1] = 0;
    matrix[W2] = 0;
    matrix[W3] = 1;
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

void COM_MultiplyVectorByMatrix(f32* matrix, f32* vec)
{
    vec[VEC_X] =   (matrix[X0] * vec[VEC_X]) + 
                    (matrix[X1] * vec[VEC_Y]) + 
                    (matrix[X2] * vec[VEC_Z]) + 
                    (matrix[X3] * vec[VEC_W]);

    vec[VEC_Y] =   (matrix[Y0] * vec[VEC_X]) + 
                    (matrix[Y1] * vec[VEC_Y]) + 
                    (matrix[Y2] * vec[VEC_Z]) + 
                    (matrix[Y3] * vec[VEC_W]);

    vec[VEC_Z] =   (matrix[Z0] * vec[VEC_X]) + 
                    (matrix[Z1] * vec[VEC_Y]) + 
                    (matrix[Z2] * vec[VEC_Z]) + 
                    (matrix[Z3] * vec[VEC_W]);

    vec[VEC_W] =   (matrix[W0] * vec[VEC_X]) + 
                    (matrix[W1] * vec[VEC_Y]) + 
                    (matrix[W2] * vec[VEC_Z]) + 
                    (matrix[W3] * vec[VEC_W]);
}
