#ifndef TESTS_MODULE_CPP
#define TESTS_MODULE_CPP

#include "../../common/com_module.h"
#include "test_priority.h"
#include "test_introspection.h"

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

internal void PrintM3x3(f32* m, char* label)
{
    printf("--- %s ---\n", label);
    printf("%.3f, %.3f, %.3f\n", m[0], m[3], m[6]);
    printf("%.3f, %.3f, %.3f\n", m[1], m[4], m[7]);
    printf("%.3f, %.3f, %.3f\n", m[2], m[5], m[8]);
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

// Return 1 if yes, 0 if values differ
i32 M4x4_AreEqual(f32* a, f32* b, f32 epsilon)
{
    for (i32 i = 0; i < 16; ++i)
    {
        f32 diff = b[i] - a[i];
        if (diff > epsilon || diff < -epsilon)
        {
            printf("diff! %f\n", diff);
            return 0;
        }
    }
    return 1;
}

void SetTestViewModelMatrix_1(M4x4* m)
{
    M4x4_SetToIdentity(m->cells);
    m->w2 = -2;
}

void SetTestViewModelMatrix_2(M4x4* m)
{
    m->cells[0] = 0.707106531f;
    m->cells[1] = 0.500000298f;
    m->cells[2] = -0.499999970f;
    m->cells[3] = 0.000000000f;

    m->cells[4] = 0.000000000f;
    m->cells[5] = 0.707106531f;
    m->cells[6] = 0.707107008f;
    m->cells[7] = 0.000000000f;

    m->cells[8] = 0.707107008f;
    m->cells[9] = -0.499999970f;
    m->cells[10] = 0.499999642f;
    m->cells[11] = 0.000000000f;

	m->cells[12] = 0.000000000f;
	m->cells[13] = 7.15255737e-07f;
    m->cells[14] = -2.12132025f;
	m->cells[15] = 1.00000000f;
}

void TestRenderMatrices()
{
    printf("Test Render Matrices\n");
    M4x4_CREATE(projection);
    f32 aspectRatio = 1.83549786f;
    f32 prjNear = 0.1f;
	f32 prjFar = 1000;
	f32 prjLeft = -0.07f * aspectRatio;
	f32 prjRight = 0.07f * aspectRatio;
	f32 prjTop = 0.07f;
	f32 prjBottom = -0.07f;
    M4x4_SetProjection(projection.cells, prjNear, prjFar, prjLeft, prjRight, prjTop, prjBottom);
    //PrintM4x4(projection.cells, 0, "Target projection matrix\n");

    M4x4_CREATE(targetModelView);
    //SetTestViewModelMatrix_1(&targetModelView);
    SetTestViewModelMatrix_2(&targetModelView);
    
    PrintM4x4(targetModelView.cells, 0, "Target ViewModel matrix\n");

    // Create the camera and model transforms for the scene:
    TRANSFORM_CREATE(camera);
    TRANSFORM_CREATE(model);

    #if 1
    camera.pos.z = 1.5f;
    camera.pos.y = 1.5f;
    Transform_SetRotation(&camera, -(45    * DEG2RAD), 0, 0);
    Transform_RotateY(&model, 45 * DEG2RAD);
    #endif
    #if 0
    model.pos.z = -2;
    #endif
    #if 1
    // Now recalculate the target matrix
    M4x4_CREATE(modelView)
    COM_SetupViewModelMatrix(&modelView, &camera, &model);

    PrintM4x4(modelView.cells, 0, "Taa daaaaa");

    if (M4x4_AreEqual(targetModelView.cells, modelView.cells, 0.00001f))
    {
        printf("Yaaay\n");
    }
    else
    {
        printf("\nBoooooo :(\n");
    }
    #endif
    M3x3_CREATE(rot)
    M3x3_RotateX(camera.rotation.cells, 90  * DEG2RAD);
    M3x3_Multiply(camera.rotation.cells, model.rotation.cells, rot.cells);
    PrintM3x3(rot.cells, "Rotation (cam * model)");
    /*
    Wrong signs:
    X_Z
    Y_Y
    Z_Z
    */
    //M3x3_Multiply(model.rotation.cells, camera.rotation.cells, rot.cells);
    //PrintM3x3(rot.cells, "Rotation (model * cam)");
}

void Tests_SpreadPattern()
{
    i32 numItems = 2;
    f32 forwardRadians = 0;
    f32 arc = 1;
    printf("Firing %d projectiles, forward %.2f degrees, arc %.3f degrees\n",
        numItems, forwardRadians * RAD2DEG, arc * RAD2DEG);
    
    f32 step = arc / (numItems - 1);
    f32 radians = forwardRadians - (arc / 2.0f);
    for (i32 i = 0; i < numItems; ++i)
    {
        printf("%d - %.3f\n", i, radians);
        radians += step;
    }
}

void Tests_Run(i32 argc, char* argv[])
{
    //MatrixBasics();
    //TestRenderMatrices();
    //Test_Priority();
    //Tests_SpreadPattern();
    Test_Introspection();
}

#endif
