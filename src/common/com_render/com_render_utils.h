#pragma once

#include "../com_module.h"


////////////////////////////////////////////////////////////////////
// Projection
////////////////////////////////////////////////////////////////////

extern "C"
com_internal void COM_Setup3DProjection(
	f32* m4x4, i32 fov, f32 prjScaleFactor, f32 prjNear, f32 prjFar, f32 aspectRatio);

extern "C"
com_internal void COM_SetupDefault3DProjection(
	f32* m4x4, f32 aspectRatio);