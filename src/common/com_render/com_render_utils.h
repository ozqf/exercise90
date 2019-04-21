#pragma once

#include "../common.h"


////////////////////////////////////////////////////////////////////
// Projection
////////////////////////////////////////////////////////////////////

extern "C"
com_internal void COM_Setup3DProjection(
	f32* m4x4, i32 fov, f32 prjScaleFactor, f32 prjNear, f32 prjFar, f32 aspectRatio);

extern "C"
com_internal void COM_SetupDefault3DProjection(
	f32* m4x4, f32 aspectRatio);

extern "C"
com_internal void COM_SetupViewModelMatrix(
	M4x4* result, Transform* cameraTransform, Transform* modelTransform);
