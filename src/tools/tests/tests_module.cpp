#ifndef TESTS_MODULE_CPP
#define TESTS_MODULE_CPP

#include "../../common/com_module.h"
#include "test_priority.h"

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

void MatrixBasics()
{
    printf("Rotation test\n");
    M4x4 rotM;
    M4x4_SetToIdentity(rotM.cells);
    M4x4_RotateY(rotM.cells, 45 * DEG2RAD);
    PrintM4x4(rotM.cells, 1, "RotM");
    Vec3 v = { 1, 0, 0 };
    PrintVec3(&v, "Vector");
    // Rotate the vector by 45 degrees
    Vec3 v_45 = Vec3_MultiplyByM4x4(&v, rotM.cells);
    PrintVec3(&v_45, "Vector result");

    // create another 45 degree rotation matrix
    // now the vector is turned 90 degrees to align with the
    // Y axis
    M4x4 rot2;
    M4x4_SetToIdentity(rot2.cells);
    M4x4_RotateY(rot2.cells, 45 * DEG2RAD);
    M4x4_Multiply(rotM.cells, rot2.cells, rot2.cells);
    PrintM4x4(rot2.cells, 1, "Rot2");
    Vec3 v_90 = Vec3_MultiplyByM4x4(&v, rot2.cells);
    PrintVec3(&v_90, "Vector");

    // create a matrix combining the results of the two 45 degree
    // rotations with another 90 degree rotation.
    // vector is now flipped on x axis when multiplied by the matrix
    M4x4_CREATE(rot3);
    M4x4_RotateY(rot3.cells, 90 * DEG2RAD);
    M4x4_Multiply(rot2.cells, rot3.cells, rot3.cells);
    Vec3 v_180 = Vec3_MultiplyByM4x4(&v, rot3.cells);
    PrintVec3(&v_180, "Vector");

    M4x4_CREATE(scaleM)
    M4x4_SetToScaling(scaleM.cells, 4, 4, 4);
    M4x4_CREATE(rotAndScale)
    M4x4_Multiply(rot3.cells, scaleM.cells, rotAndScale.cells);
    Vec3 v_180AndScale = Vec3_MultiplyByM4x4(&v, rotAndScale.cells);
    PrintVec3(&v_180AndScale, "Vector");

    M4x4_CREATE(translate)
    M4x4_SetPosition(translate.cells, -1, 10, -64);
    Vec3 v_translated = Vec3_MultiplyByM4x4(&v, translate.cells);
    PrintM4x4(translate.cells, 0, "Translation matrix");
    PrintVec3(&v_translated, "Translated Vector");
}

void ProjectionMatrix()
{
    M4x4 m = {};
    COM_SetupDefault3DProjection(m.cells, 16.0f / 9.0f);
    M4x4 I;
    M4x4_SetToIdentity(I.cells);
    PrintM4x4(m.cells,1, "A");
    PrintM4x4(I.cells, 1, "I");
}

void Tests_Run(i32 argc, char* argv[])
{
    //MatrixBasics();
    Test_Priority();
}

#endif
