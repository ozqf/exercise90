#pragma once


/////////////////////////////////////////////////////////////////////////////
// MATRIX 4x4
/////////////////////////////////////////////////////////////////////////////
struct M4x4
{
    union
    {
        /* Careful! Column order!
            X   Y   Z   W
            0   4   8   12
            1   5   9   13
            2   6   10  14
            3   7   11  15
            */
        struct
        {
            f32
            x0, x1, x2, x3,
            y0, y1, y2, y3,
            z0, z1, z2, z3,
            w0, w1, w2, w3;
        };
        struct
        {
            f32
            // xAxisX, yAxisX, zAxisX, posX,
            // xAxisY, yAxisY, zAxisY, posY,
            // xAxisZ, yAxisZ, zAxisZ, posZ,
            // xAxisW, yAxisW, zAxisW, posW;

            xAxisX, xAxisY, xAxisZ, xAxisW,
            yAxisX, yAxisY, yAxisZ, yAxisW,
            zAxisX, zAxisY, zAxisZ, zAxisW,
            posX, posY, posZ, posW;
        };
        struct
        {
            Vec4 xAxis;
            Vec4 yAxis;
            Vec4 zAxis;
            Vec4 wAxis;
        };
        f32 cells[16];
    };
};

/////////////////////////////////////////////////////////////////////////////
// 4x4 MATRIX OPERATIONS
/////////////////////////////////////////////////////////////////////////////
inline void M4x4_SetX(f32* m, f32 x0, f32 x1, f32 x2, f32 x3)
{
    m[X0] = x0; m[X1] = x1; m[X2] = x2; m[X3] = x3;
}

inline void M4x4_SetY(f32* m, f32 y0, f32 y1, f32 y2, f32 y3)
{
    m[Y0] = y0; m[Y1] = y1; m[Y2] = y2; m[Y3] = y3;
}

inline void M4x4_SetZ(f32* m, f32 z0, f32 z1, f32 z2, f32 z3)
{
    m[Z0] = z0; m[Z1] = z1; m[Z2] = z2; m[Z3] = z3;
}

inline void M4x4_SetW(f32* m, f32 w0, f32 w1, f32 w2, f32 w3)
{
    m[W0] = w0; m[W1] = w1; m[W2] = w2; m[W3] = w3;
}

inline void M4x4_Multiply(f32* m0, f32* m1, f32* result)
{
    // utterly wrong haha
    // result[X0] = m0[X0] * m1[X0];   result[X1] = m0[X1] * m1[X1];   result[X2] = m0[X2] * m1[X2];   result[X3] = m0[X3] * m1[X3];
    // result[Y0] = m0[Y0] * m1[Y0];   result[Y1] = m0[Y1] * m1[Y1];   result[Y2] = m0[Y2] * m1[Y2];   result[Y3] = m0[Y3] * m1[Y3];
    // result[Z0] = m0[Z0] * m1[Z0];   result[Z1] = m0[Z1] * m1[Z1];   result[Z2] = m0[Z2] * m1[Z2];   result[Z3] = m0[Z3] * m1[Z3];
    // result[W0] = m0[W0] * m1[W0];   result[W1] = m0[W1] * m1[W1];   result[W2] = m0[W2] * m1[W2];   result[W3] = m0[W3] * m1[W3];
    /*
                    0   4   8   12
                    1   5   9   13
                    2   6   10  14
                    3   7   11  15
                    |   |   |   |
    0   4   8   12-
    1   5   9   13-
    2   6   10  14-
    3   7   11  15-
    */
    // 0 = 
    // Copy original
    f32 r[16];
    for (i32 i = 0; i < 16; ++i)
    {
        r[i] = m0[i];
    }
    r[0] = (m0[0] * m1[0]) + (m0[4] * m1[1]) + (m0[8] * m1[2]) + (m0[12] * m1[3]);
    r[1] = (m0[1] * m1[0]) + (m0[5] * m1[1]) + (m0[9] * m1[2]) + (m0[13] * m1[3]);
    r[2] = (m0[2] * m1[0]) + (m0[6] * m1[1]) + (m0[10] * m1[2]) + (m0[14] * m1[3]);
    r[3] = (m0[3] * m1[0]) + (m0[7] * m1[1]) + (m0[11] * m1[2]) + (m0[15] * m1[3]);

    r[4] = (m0[0] * m1[4]) + (m0[4] * m1[5]) + (m0[8] * m1[6]) + (m0[12] * m1[7]);
    r[5] = (m0[1] * m1[4]) + (m0[5] * m1[5]) + (m0[9] * m1[6]) + (m0[13] * m1[7]);
    r[6] = (m0[2] * m1[4]) + (m0[6] * m1[5]) + (m0[10] * m1[6]) + (m0[14] * m1[7]);
    r[7] = (m0[3] * m1[4]) + (m0[7] * m1[5]) + (m0[11] * m1[6]) + (m0[15] * m1[7]);
    
    r[8] = (m0[0] * m1[8]) + (m0[4] * m1[9]) + (m0[8] * m1[10]) + (m0[12] * m1[11]);
    r[9] = (m0[1] * m1[8]) + (m0[5] * m1[9]) + (m0[9] * m1[10]) + (m0[13] * m1[11]);
    r[10] = (m0[2] * m1[8]) + (m0[6] * m1[9]) + (m0[10] * m1[10]) + (m0[14] * m1[11]);
    r[11] = (m0[3] * m1[8]) + (m0[7] * m1[9]) + (m0[11] * m1[10]) + (m0[15] * m1[11]);
    
    r[12] = (m0[0] * m1[12]) + (m0[4] * m1[13]) + (m0[8] * m1[14]) + (m0[12] * m1[15]);
    r[13] = (m0[1] * m1[12]) + (m0[5] * m1[13]) + (m0[9] * m1[14]) + (m0[13] * m1[15]);
    r[14] = (m0[2] * m1[12]) + (m0[6] * m1[13]) + (m0[10] * m1[14]) + (m0[14] * m1[15]);
    r[15] = (m0[3] * m1[12]) + (m0[7] * m1[13]) + (m0[11] * m1[14]) + (m0[15] * m1[15]);
    
    // f32 r[16];
    // r[0] = (m0[0] * m1[0]) + (m0[4] * m1[1]) + (m0[8] * m1[2]) + (m0[12] * m1[3]);
    // r[1] = (m0[0] * m1[4]) + (m0[4] * m1[5]) + (m0[8] * m1[6]) + (m0[12] * m1[7]);
    // r[2] = (m0[0] * m1[8]) + (m0[4] * m1[9]) + (m0[8] * m1[10]) + (m0[12] * m1[11]);
    // r[3] = (m0[0] * m1[12]) + (m0[4] * m1[13]) + (m0[8] * m1[14]) + (m0[12] * m1[15]);

    // r[4] = (m0[1] * m1[0]) + (m0[5] * m1[1]) + (m0[9] * m1[2]) + (m0[14] * m1[3]);
    // r[5] = (m0[1] * m1[4]) + (m0[5] * m1[5]) + (m0[9] * m1[6]) + (m0[14] * m1[7]);
    // r[6] = (m0[1] * m1[8]) + (m0[5] * m1[9]) + (m0[9] * m1[10]) + (m0[14] * m1[11]);
    // r[7] = (m0[1] * m1[12]) + (m0[5] * m1[13]) + (m0[9] * m1[14]) + (m0[14] * m1[15]);

    // r[8] = (m0[2] * m1[0]) + (m0[6] * m1[1]) + (m0[10] * m1[2]) + (m0[14] * m1[3]);
    // r[9] = (m0[2] * m1[4]) + (m0[6] * m1[5]) + (m0[10] * m1[6]) + (m0[14] * m1[7]);
    // r[10] = (m0[2] * m1[8]) + (m0[6] * m1[9]) + (m0[10] * m1[10]) + (m0[14] * m1[11]);
    // r[11] = (m0[2] * m1[12]) + (m0[6] * m1[13]) + (m0[10] * m1[14]) + (m0[14] * m1[15]);

    // r[12] = (m0[3] * m1[0]) + (m0[7] * m1[1]) + (m0[11] * m1[2]) + (m0[15] * m1[3]);
    // r[13] = (m0[3] * m1[4]) + (m0[7] * m1[5]) + (m0[11] * m1[6]) + (m0[15] * m1[7]);
    // r[14] = (m0[3] * m1[8]) + (m0[7] * m1[9]) + (m0[11] * m1[10]) + (m0[15] * m1[11]);
    // r[15] = (m0[3] * m1[12]) + (m0[7] * m1[13]) + (m0[11] * m1[14]) + (m0[15] * m1[15]);
    for (i32 i = 0; i < 16; ++i)
    {
        result[i] = r[i];
    }
}

inline void M4x4_Copy(f32* src, f32* tar)
{
    tar[X0] = src[X0];  tar[X1] = src[X1];  tar[X2] = src[X2];  tar[X3] = src[X3];
    tar[Y0] = src[Y0];  tar[Y1] = src[Y1];  tar[Y2] = src[Y2];  tar[Y3] = src[Y3];
    tar[Z0] = src[Z0];  tar[Z1] = src[Z1];  tar[Z2] = src[Z2];  tar[Z3] = src[Z3];
    tar[W0] = src[W0];  tar[W1] = src[W1];  tar[W2] = src[W2];  tar[W3] = src[W3];
}

// inline f32 M4x4_GetScaleX(f32* m)
// {
//     f32 x = fabsf(m[X0] * m[X0]);
//     f32 y = fabsf(m[Y0] * m[Y0]);
//     f32 z = fabsf(m[Z0] * m[Z0]);
//     return (f32)sqrt(x + y + z);
// }

// inline f32 M4x4_GetScaleY(f32* m)
// {
//     f32 x = fabsf(m[X1] * m[X1]);
//     f32 y = fabsf(m[Y1] * m[Y1]);
//     f32 z = fabsf(m[Z1] * m[Z1]);
//     return (f32)sqrt(x + y + z);
// }

// inline f32 M4x4_GetScaleZ(f32* m)
// {
//     f32 x = fabsf(m[X2] * m[X2]);
//     f32 y = fabsf(m[Y2] * m[Y2]);
//     f32 z = fabsf(m[Z2] * m[Z2]);
//     return (f32)sqrt(x + y + z);
// }

// inline Vec4 M4x4_GetScale(f32* m)
// {
//     Vec4 result;
//     result.x = fabsf((f32)sqrt(m[X0] * m[X0] + m[Y0] * m[Y0] + m[Z0] * m[Z0]));
//     result.y = fabsf((f32)sqrt(m[X1] * m[X1] + m[Y1] * m[Y1] + m[Z1] * m[Z1]));
//     result.z = fabsf((f32)sqrt(m[X2] * m[X2] + m[Y2] * m[Y2] + m[Z2] * m[Z2]));
//     result.w = 1;
//     return result;
// }

// inline void M4x4_SetScale(f32* m, f32 x, f32 y, f32 z)
// {
//     M4x4* mat = (M4x4*)m;
//     Vec4_SetMagnitude(&mat->xAxis, x);
//     Vec4_SetMagnitude(&mat->yAxis, y);
//     Vec4_SetMagnitude(&mat->zAxis, z);
// }

inline void M4x4_RotateX(f32* m, f32 degreesY)
{
	M4x4 rotM = {};
    rotM.xAxisX = 1;
    rotM.yAxisY = (f32)cos(degreesY * DEG2RAD);
    rotM.yAxisZ = (f32)sin(degreesY * DEG2RAD);
    rotM.zAxisY = (f32)-sin(degreesY * DEG2RAD);
    rotM.zAxisZ = (f32)cos(degreesY * DEG2RAD);
    rotM.posW = 1;
    M4x4_Multiply(m, rotM.cells, m);
}

inline void M4x4_RotateY(f32* m, f32 degreesY)
{
	M4x4 rotM = {};
    rotM.yAxisY = 1;
    rotM.xAxisX = (f32)cos(degreesY * DEG2RAD);
    rotM.xAxisZ = (f32)-sin(degreesY * DEG2RAD);
    rotM.zAxisX = (f32)sin(degreesY * DEG2RAD);
    rotM.zAxisZ = (f32)cos(degreesY * DEG2RAD);
    rotM.posW = 1;
    M4x4_Multiply(m, rotM.cells, m);
}

inline void M4x4_RotateZ(f32* m, f32 degreesY)
{
	M4x4 rotM = {};
    rotM.zAxisZ = 1;
    rotM.xAxisX = (f32)cos(degreesY * DEG2RAD);
    rotM.xAxisY = (f32)sin(degreesY * DEG2RAD);
    rotM.yAxisX = (f32)-sin(degreesY * DEG2RAD);
    rotM.yAxisY = (f32)cos(degreesY * DEG2RAD);
    rotM.posW = 1;
    M4x4_Multiply(m, rotM.cells, m);
}

inline f32 M4x4_GetAngleX(f32* m)
{
    //return (f32)atan2(m[8], m[10]);
    return (f32)-asinf(m[9]);
}

inline f32 M4x4_GetAngleY(f32* m)
{
    return (f32)atan2(m[8], m[10]);
    //return (f32)-asinf(m[9]);
}

inline f32 M4x4_GetAngleZ(f32* m)
{
    return (f32)atan2(m[1], m[5]);
}

inline void M4x4_SetPosition(f32* m, f32 x, f32 y, f32 z)
{
    m[W0] = x, m[W1] = y; m[W2] = z;
}

inline Vec4 M4x4_GetPosition(f32* m)
{
    Vec4 result;
    result.x = m[W0];
    result.y = m[W1];
    result.z = m[W2];
    result.w = 1;
    return result;
}

inline Vec4 M4x4_GetEulerAnglesRadians(f32* m)
{
    Vec4 result;
    result.x = (f32)-asinf(m[9]);
    result.y = (f32)atan2(m[8], m[10]);
    result.z = (f32)atan2(m[1], m[5]);
    result.w = 1;
    return result;
}

inline Vec4 M4x4_GetEulerAnglesDegrees(f32* m)
{
    Vec4 result;
    result.x = (f32)-asinf(m[9]) * RAD2DEG;
    result.y = ((f32)atan2(m[8], m[10])) * RAD2DEG;
    result.z = ((f32)atan2(m[1], m[5])) * RAD2DEG;
    // result.y = (f32)-asinf(m[9]) * RAD2DEG;
    // result.x = ((f32)atan2(m[8], m[10])) * RAD2DEG;
    // result.z = ((f32)atan2(m[1], m[5])) * RAD2DEG;
    result.w = 1;
    return result;
}

inline void M4x4_SetEulerAnglesByRadians(f32* m, f32 roll, f32 pitch, f32 yaw)
{
    // TODO: Implement
}
