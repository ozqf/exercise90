#ifndef TESTS_MODULE_CPP
#define TESTS_MODULE_CPP

#include "../../common/com_module.h"

#define TEST_ASSERT(expression, msg) \
if (!(expression)) { printf("Failed: %s\n", msg##); } \
else { printf("Passed: %s\n", msg##); }

internal void PrintM4x4(f32* m, char* label)
{
    printf("--- %s ---\n", label);
    printf("%.3f, %.3f, %.3f, %.3f\n", m[0], m[4], m[8], m[12]);
    printf("%.3f, %.3f, %.3f, %.3f\n", m[1], m[5], m[9], m[13]);
    printf("%.3f, %.3f, %.3f, %.3f\n", m[2], m[6], m[10], m[14]);
    printf("%.3f, %.3f, %.3f, %.3f\n", m[3], m[7], m[11], m[15]);
}

void Tests_Run(i32 argc, char* argv[])
{
    M4x4 m = {};
    COM_SetupDefault3DProjection(m.cells, 16.0f / 9.0f);
    M4x4 I;
    M4x4_SetToIdentity(I.cells);
    PrintM4x4(m.cells,"A");
    PrintM4x4(I.cells, "I");
}

#endif
