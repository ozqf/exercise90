#pragma once

#include <math.h>
#include <stdint.h>

/*****************************************************
PRIMITIVE TYPES
*****************************************************/
#define f32 float
#define f64 double

#define i8 int8_t
#define i16 int16_t
#define i32 int32_t
#define i64 int64_t

#define u8 uint8_t
#define uChar uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t

#define bool32 uint32_t

/*****************************************************
MACROS
*****************************************************/
#define internal static
#define local_persist static 
#define global_variable static

#define pi32 3.14159265359f
#define DEG2RAD 3.141593f / 180.0f
#define RAD2DEG 57.2958f

internal i32 g_z_inf = 0x7F800000;
inline f32 ZINFINITY() { return *(float*)&g_z_inf; }

internal i32 g_z_nan = 0x7F800001;
inline f32 ZNaN() { return *(float*)&g_z_nan; }

#define ZABS(value) { if (value < ) return -value; }
#define ZMIN(x, y) ((x) < (y) ? (x) : (y))
#define ZMAX(x, y) ((x) > (y) ? (x) : (y))

inline f32 ZMAX3(f32 a, f32 b, f32 c)
{
    f32 result = a;
    if (b > result) { result = b; }
    if (c > result) { result = c;}
    return result;
}

inline f32 ZMIN3(f32 a, f32 b, f32 c)
{
    f32 result = a;
    if (b < result) { result = b; }
    if (c < result) { result = c;}
    return result;
}

inline void ZSWAPF(f32* x, f32* y)
{
    f32 temp = *x;
    *x = *y;
    *y = temp;
}

#define ArrayCount(array) (sizeof(array) / sizeof(array)[0]))

// Quick cross platform assert. Read from address zero if expression is false
// TODO: Message box assert with a print of __FILE__, __LINE__ and __TIME__ possible?
#if PARANOID
#define Assert(expression) if(!(expression)) {*(int *)0 = 0; }
#else
#define Assert(expression)
#endif
#define AssertAlways(expression) if(!(expression)) { *(int *)0 = 0; }

#define IllegalCodePath if (true) { *(int *)0 = 0; }

#define KiloBytes(bytes) ((bytes) * 1024LL)
#define MegaBytes(bytes) (KiloBytes(bytes) * 1024LL)

#define DLL_EXPORT __declspec(dllexport)

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

/*****************************************************
MATHS TYPES
*****************************************************/

struct Vec2
{
    union
    {
        struct
        {
            f32 x, y;
        };
        f32 e[2];
    };
};

struct Vec3
{
    union 
    {
        struct
        {
            f32 x, y, z;
        };    
        f32 e[3];
    };
    // f32 x, y, z, w;
    // // overload array operator to return a pointer to x + index
    // f32 &operator[](int index) { return ((&x)[index]); }
};

internal Vec3 global_vec3_Up = { 0, 1, 0 };
internal Vec3 global_vec3_Forward = { 0, 0, 1 };

struct Vec4
{
    union 
    {
        struct
        {
            f32 x, y, z, w;
        };    
        f32 e[4];
    };
    // f32 x, y, z, w;
    // // overload array operator to return a pointer to x + index
    // f32 &operator[](int index) { return ((&x)[index]); }
};

internal Vec4 global_vec4_Up = { 0, 1, 0, 1 };
internal Vec4 global_vec4_Forward = { 0, 0, 1, 1 };

/////////////////////////////////////////////////////////////////////////////
// VECTOR 3 OPERATIONS
/////////////////////////////////////////////////////////////////////////////

inline f32 Vec3_Magnitude(Vec3* v)
{
    return (f32)sqrt((f32)(v->x * v->x) + (v->y * v->y) + (v->z * v->z));
}

inline void Vec3_Normalise(Vec3* v)
{
    f32 vectorMagnitude = Vec3_Magnitude(v);
    v->x /= vectorMagnitude;
    v->y /= vectorMagnitude;
    v->z /= vectorMagnitude;
}

inline void Vec3_SetMagnitude(Vec3* v, f32 newMagnitude)
{
    Vec3_Normalise(v);
    v->x = v->x * newMagnitude;
    v->y = v->y * newMagnitude;
    v->z = v->z * newMagnitude;
}

inline void Vec3_CrossProduct(Vec3* a, Vec3* b, Vec3* result)
{
    result->x = (a->y * b->z) - (a->z * b->y);
    result->z = (a->x * b->y) - (a->y * b->x);
    result->y = (a->z * b->x) - (a->x * b->z);
}

inline f32 Vec3_DotProduct(Vec3* a, Vec3* b)
{
    return (a->x * b->x) + (a->y * b->y) + (a->z * b->z);
}

inline void Vec3_NormaliseOrForward(Vec3* v)
{
    f32 mag = Vec3_Magnitude(v);
    if (mag == 0)
    {
        v->x = 0;
        v->y = 0;
        v->z = 1;
        return;
    }
    v->x = v->x /= mag;
    v->y = v->y /= mag;
    v->z = v->z /= mag;
}
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
    result[X0] = m0[X0] * m1[X0];   result[X1] = m0[X1] * m1[X1];   result[X2] = m0[X2] * m1[X2];   result[X3] = m0[X3] * m1[X3];
    result[Y0] = m0[Y0] * m1[Y0];   result[Y1] = m0[Y1] * m1[Y1];   result[Y2] = m0[Y2] * m1[Y2];   result[Y3] = m0[Y3] * m1[Y3];
    result[Z0] = m0[Z0] * m1[Z0];   result[Z1] = m0[Z1] * m1[Z1];   result[Z2] = m0[Z2] * m1[Z2];   result[Z3] = m0[Z3] * m1[Z3];
    result[W0] = m0[W0] * m1[W0];   result[W1] = m0[W1] * m1[W1];   result[W2] = m0[W2] * m1[W2];   result[W3] = m0[W3] * m1[W3];
}

inline void M4x4_Copy(f32* src, f32* tar)
{
    tar[X0] = src[X0];  tar[X1] = src[X1];  tar[X2] = src[X2];  tar[X3] = src[X3];
    tar[Y0] = src[Y0];  tar[Y1] = src[Y1];  tar[Y2] = src[Y2];  tar[Y3] = src[Y3];
    tar[Z0] = src[Z0];  tar[Z1] = src[Z1];  tar[Z2] = src[Z2];  tar[Z3] = src[Z3];
    tar[W0] = src[W0];  tar[W1] = src[W1];  tar[W2] = src[W2];  tar[W3] = src[W3];
}

inline f32 M4x4_GetScaleX(f32* m)
{
    f32 x = m[X0] * m[X0];
    f32 y = m[Y0] * m[Y0];
    f32 z = m[Z0] * m[Z0];
    return (f32)sqrt(x + y + z);
}

inline f32 M4x4_GetScaleY(f32* m)
{
    f32 x = m[X1] * m[X1];
    f32 y = m[Y1] * m[Y1];
    f32 z = m[Z1] * m[Z1];
    return (f32)sqrt(x + y + z);
}

inline f32 M4x4_GetScaleZ(f32* m)
{
    f32 x = m[X0] * m[X0];
    f32 y = m[Y0] * m[Y0];
    f32 z = m[Z0] * m[Z0];
    return (f32)sqrt(x + y + z);
}

inline f32 M4x4_GetAngleX(f32* m)
{
    return (f32)-asinf(m[9]);
}

inline f32 M4x4_GetAngleY(f32* m)
{
    return (f32)atan2(m[8], m[10]);
}

inline f32 M4x4_GetAngleZ(f32* m)
{
    return (f32)atan2(m[1], m[5]);
}

inline void M4x4_SetPosition(f32* m, f32 x, f32 y, f32 z)
{
    m[W0] = x, m[W1] = y; m[W2] = z;
}

inline void M4x4_SetScale(f32* m, f32 x, f32 y, f32 z)
{
    // TODO: Implement
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

inline Vec4 M4x4_GetEulerAngles(f32* m)
{
    Vec4 result;
    result.x = (f32)-asinf(m[9]);
    result.y = (f32)atan2(m[8], m[10]);
    result.z = (f32)atan2(m[1], m[5]);
    result.w = 1;
    return result;
}

inline void M4x4_SetEulerAnglesByRadians(f32* m, f32 roll, f32 pitch, f32 yaw)
{
    // TODO: Implement
}

inline Vec4 M4x4_GetScale(f32* m)
{
    Vec4 result;
    result.x = (f32)sqrt(m[X0] * m[X0] + m[Y0] * m[Y0] + m[Z0] * m[Z0]);
    result.y = (f32)sqrt(m[X1] * m[X1] + m[Y1] * m[Y1] + m[Z1] * m[Z1]);
    result.z = (f32)sqrt(m[X2] * m[X2] + m[Y2] * m[Y2] + m[Z2] * m[Z2]);
    result.w = 1;
    return result;
}

/****************************************************************
Conversion
****************************************************************/
union f32_union
{
    f32 f;
    u8 bytes[sizeof(f32)];
};

union u32_union
{
    u32 value;
    u8 bytes[sizeof(u32)];
};

/****************************************************************
Graphics
****************************************************************/
struct Mesh
{
	u32 id;

	u32 numVerts;

	f32* verts;
	f32* uvs;
    f32* normals;
};

/*****************************************************
SIMPLE TYPES
*****************************************************/
struct MemoryBlock
{
    void *ptrMemory;
    i32 size;
};

struct ZStringHeader
{
    char* chars;
    i32 length;
    i32 maxLength;
};

struct GameTime
{
    f32 deltaTime;
    u32 frameNumber;
    u32 fixedFrameNumber;
    f32 fixedDeltaTime;
    u32 ticks;
};

struct Transform
{
	// Vec3 pos;
	// Vec3 rot;
	// Vec3 scale;
    M4x4 matrix;
};

struct AngleVectors
{
    Vec3 forward;
    Vec3 up;
    Vec3 left;
};

struct Transform2
{
	Vec3 pos;
	Vec3 forward;
    Vec3 up;
    Vec3 left;
	Vec3 scale;
};

/****************************************************************
Global Functions
****************************************************************/

inline void Transform_SetToIdentity(Transform* t)
{
    *t = {};
    // t->scale.x = 1;
    // t->scale.y = 1;
    // t->scale.z = 1;

    t->matrix.x0 = 1;
    t->matrix.y1 = 1;
    t->matrix.z2 = 1;
    t->matrix.w3 = 1;
}

inline u32 SafeTruncateUInt64(u64 value)
{
	// TODO: Defines for max value
	Assert(value <= 0xFFFFFFFF);
	u32 result = (u32)value;
	return result;
}
