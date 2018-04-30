#pragma once


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
// VECTOR 3 OPERATIONS
/////////////////////////////////////////////////////////////////////////////

inline f32 Vec4_Magnitude(Vec4* v)
{
    return (f32)sqrt((f32)(v->x * v->x) + (v->y * v->y) + (v->z * v->z));
}

inline void Vec4_Normalise(Vec4* v)
{
    f32 vectorMagnitude = Vec4_Magnitude(v);
    v->x /= vectorMagnitude;
    v->y /= vectorMagnitude;
    v->z /= vectorMagnitude;
}

inline void Vec4_SetMagnitude(Vec4* v, f32 newMagnitude)
{
    Vec4_Normalise(v);
    v->x = v->x * newMagnitude;
    v->y = v->y * newMagnitude;
    v->z = v->z * newMagnitude;
}
