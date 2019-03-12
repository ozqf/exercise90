#pragma once

#include "../com_module.h"


////////////////////////////////////////////////////////////////////
// Projection
////////////////////////////////////////////////////////////////////

extern "C"
com_internal void COM_Setup3DProjection(
	f32* m4x4,
	i32 fov,
	f32 prjScaleFactor,
	f32 prjNear,
	f32 prjFar,
	f32 aspectRatio)
{
	if (fov <= 0) { fov = 90; }
	M4x4_SetToIdentity(m4x4);
	
	f32 prjLeft = -prjScaleFactor * aspectRatio;
	f32 prjRight = prjScaleFactor * aspectRatio;
	f32 prjTop = prjScaleFactor;
	f32 prjBottom = -prjScaleFactor;

	M4x4_SetProjection(
		m4x4, prjNear, prjFar, prjLeft, prjRight, prjTop, prjBottom);
}

extern "C"
com_internal void COM_SetupDefault3DProjection(
	f32* m4x4, f32 aspectRatio)
{
	//COM_Setup3DProjection(m4x4, 90, 0.5f, 1.0f, 1000.0f, aspectRatio);
	COM_Setup3DProjection(m4x4, 90, 0.07f, 0.1f, 1000.0f, aspectRatio);
}

void M4x4_FlipRotation(f32* m)
{
	m[M4x4_X0] = -m[M4x4_X0];
	m[M4x4_X1] = -m[M4x4_X1];
	m[M4x4_X2] = -m[M4x4_X2];

	m[M4x4_Y0] = -m[M4x4_Y0];
	m[M4x4_Y1] = -m[M4x4_Y1];
	m[M4x4_Y2] = -m[M4x4_Y2];

	m[M4x4_Z0] = -m[M4x4_Z0];
	m[M4x4_Z1] = -m[M4x4_Z1];
	m[M4x4_Z2] = -m[M4x4_Z2];
}

extern "C"
com_internal void COM_SetupViewModelMatrix(
	M4x4* result, Transform* cameraTransform, Transform* modelTransform)
{
	/*
	Current opengl 1.1 implementation:
	> Switch to view model matrix and set to identity
	> Get camera euler angles
	> rotate by inverse of camera Z angle
	> rotate by inverse of camera X angle
	> rotate by inverse of camera Y angle
	> translate by inverse of camera position
	> Translate by model position
	> rotate by model Z angle
	> rotate by model X angle
	> rotate by model Y angle
	*/
	M4x4 camTrans;
	M4x4 cam;
	M4x4 modelTrans;
	M4x4 model;
	
	// Camera rotation and translation
	Transform_ToM4x4(cameraTransform, &cam);
	M4x4_RotateX(cam.cells, 90 * DEG2RAD);
	M4x4_SetToTranslation(
		camTrans.cells, -cam.wAxis.x, -cam.wAxis.y, -cam.wAxis.z);
	cam.wAxis.x = 0;
	cam.wAxis.y = 0;
	cam.wAxis.z = 0;

	// Model rotation and translation
	Transform_ToM4x4(modelTransform, &model);
	M4x4_SetToTranslation(
		modelTrans.cells, model.wAxis.x, model.wAxis.y, model.wAxis.z);
	model.wAxis.x = 0;
	model.wAxis.y = 0;
	model.wAxis.z = 0;

	// camera rot * inverse camera pos * model pos * model rot
	M4x4_Multiply(result->cells, cam.cells, result->cells);
	M4x4_Multiply(result->cells, camTrans.cells, result->cells);
	M4x4_Multiply(result->cells, modelTrans.cells, result->cells);
	M4x4_Multiply(result->cells, model.cells, result->cells);
}
