#pragma once

#include "../com_module.h"


////////////////////////////////////////////////////////////////////
// Projection
////////////////////////////////////////////////////////////////////

extern "C"
com_internal void COM_Setup3DProjection(
	f32* m4x4, i32 fov, f32 prjScaleFactor, f32 prjNear, f32 prjFar, f32 aspectRatio)
{
	if (fov <= 0) { fov = 90; }
	M4x4_SetToIdentity(m4x4);
	
	f32 prjLeft = -prjScaleFactor * aspectRatio;
	f32 prjRight = prjScaleFactor * aspectRatio;
	f32 prjTop = prjScaleFactor;
	f32 prjBottom = -prjScaleFactor;

	M4x4_SetProjection(m4x4, prjNear, prjFar, prjLeft, prjRight, prjTop, prjBottom);
}

extern "C"
com_internal void COM_SetupDefault3DProjection(
	f32* m4x4, f32 aspectRatio)
{
	COM_Setup3DProjection(m4x4, 90, 0.5f, 1.0f, 1000.0f, aspectRatio);
}
