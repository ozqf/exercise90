#ifndef TESTS_MODULE_CPP
#define TESTS_MODULE_CPP

#include "../../common/com_module.h"

#define TEST_ASSERT(expression, msg) \
if (!(expression)) { printf("Failed: %s\n", msg##); } \
else { printf("Passed: %s\n", msg##); }

internal void PrintM4x4(f32* m, i32 rotationOnly, char* label)
{
    printf("--- %s ---\n", label);
    if (rotationOnly)
    {
        printf("%.3f, %.3f, %.3f\n", m[0], m[4], m[8]);
        printf("%.3f, %.3f, %.3f\n", m[1], m[5], m[9]);
        printf("%.3f, %.3f, %.3f\n", m[2], m[6], m[10]);
    }
    else
    {
        printf("%.3f, %.3f, %.3f, %.3f\n", m[0], m[4], m[8], m[12]);
        printf("%.3f, %.3f, %.3f, %.3f\n", m[1], m[5], m[9], m[13]);
        printf("%.3f, %.3f, %.3f, %.3f\n", m[2], m[6], m[10], m[14]);
        printf("%.3f, %.3f, %.3f, %.3f\n", m[3], m[7], m[11], m[15]);
    }
    printf("----------------\n");
}

internal void PrintVec3(Vec3* v, char* label)
{
    printf("%s: %.3f, %.3f, %.3f\n", label, v->x, v->y, v->z);
}

void Tests_Run(i32 argc, char* argv[])
{
    M4x4 m = {};
    COM_SetupDefault3DProjection(m.cells, 16.0f / 9.0f);
    M4x4 I;
    M4x4_SetToIdentity(I.cells);
    PrintM4x4(m.cells,1, "A");
    PrintM4x4(I.cells, 1, "I");

    printf("Rotation test\n");
    M4x4 rotM;
    M4x4_SetToIdentity(rotM.cells);
    M4x4_RotateY(rotM.cells, 45 * DEG2RAD);
    PrintM4x4(rotM.cells, 1, "RotM");
    Vec3 v = { 1, 0, 0 };
    PrintVec3(&v, "Vector");
    Vec3 v90 = Vec3_MultiplyByM4x4(&v, rotM.cells);
    PrintVec3(&v90, "Vector result");

    M4x4 rot2;
    M4x4_SetToIdentity(rot2.cells);
    M4x4_RotateY(rot2.cells, 45 * DEG2RAD);
    M4x4_Multiply(rotM.cells, rot2.cells, rot2.cells);
    PrintM4x4(rot2.cells, 1, "Rot2");

    M4x4_CREATE(scaleM)
    M4x4_SetToScaling(scaleM.cells, 4, 4, 4);
}

#endif
