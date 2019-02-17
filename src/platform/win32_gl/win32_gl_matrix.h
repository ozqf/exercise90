#pragma once

#include "win32_gl_module.cpp"

////////////////////////////////////////////////////////////////////
// Projection
////////////////////////////////////////////////////////////////////

void R_Setup3DProjectionA(i32 fov)
{
	if (fov <= 0) { fov = 90; }
	glMatrixMode(GL_PROJECTION);

	f32 prj[16];
	M4x4_SetToIdentity(prj);
	f32 prjNear = 1;
	f32 prjFar = 1000;
	f32 prjLeft = -0.5f * win32_aspectRatio;
	f32 prjRight = 0.5f * win32_aspectRatio;
	f32 prjTop = 0.5f;
	f32 prjBottom = -0.5f;

	M4x4_SetProjection(prj, prjNear, prjFar, prjLeft, prjRight, prjTop, prjBottom);

	glLoadMatrixf(prj);
}

void R_SetupOrthoProjection(f32 halfScreenHeight)
{
	glMatrixMode(GL_PROJECTION);

	f32 halfWidth = halfScreenHeight * win32_aspectRatio;
	f32 halfHeight = halfScreenHeight;

	f32 prjNear = 0;
	f32 prjFar = 1000;

	f32 prj[16];
	// printf("Setup ortho projection %.2f by %.2f, near %.2f far %.2f\n",
	// 	halfWidth, halfHeight, prjNear, prjFar
	// );
	M4x4_SetOrthoProjection(
		prj, -halfWidth, halfWidth, -halfHeight, halfHeight, prjNear, prjFar);
	//M4x4_SetOrthoProjection(prj, -0.5, 0.5, -0.5, 0.5, 0, 10);
	//glOrtho(-0.5, 0.5, 0.5, 0.5, 0, 1000);
	glLoadMatrixf(prj);
}

void R_Setup3DProjectionB(i32 fov)
{
	if (fov <= 0) { fov = 90; }
	glMatrixMode(GL_PROJECTION);
	
	f32 prj[16];
	M4x4_SetToIdentity(prj);
	f32 prjNear = 0.1f;
	f32 prjFar = 1000;
	f32 prjLeft = -0.07f * win32_aspectRatio;
	f32 prjRight = 0.07f * win32_aspectRatio;
	f32 prjTop = 0.07f;
	f32 prjBottom = -0.07f;

	M4x4_SetProjection(prj, prjNear, prjFar, prjLeft, prjRight, prjTop, prjBottom);

	glLoadMatrixf(prj);
}

void R_SetupProjection(RenderSceneSettings* settings)
{
	switch (settings->projectionMode)
	{
		case RENDER_PROJECTION_MODE_3D:
		{
			R_Setup3DProjectionB(settings->fov);
		} break;
		case RENDER_PROJECTION_MODE_ORTHOGRAPHIC:
		{
			R_SetupOrthoProjection(settings->orthographicHalfHeight);
		} break;
		case RENDER_PROJECTION_MODE_IDENTITY:
		{
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
		} break;
		case RENDER_PROJECTION_MODE_3D_OLD:
		{
			R_Setup3DProjectionA(settings->fov);
		} break;
		default :
		{
			R_Setup3DProjectionB(settings->fov);
		} break;
	}
}

////////////////////////////////////////////////////////////////////
// ModelView
// Standard 3D transformation
////////////////////////////////////////////////////////////////////

// DO NOT CHANGE - This actually works!
void R_SetViewModelMatrixByEuler(
	RenderSceneSettings* settings, Transform* view, Transform* model)
{
	
	#if 0
	if (settings->lightBits != 0)
	{
		R_SetupLights(settings, model);
	}
	#endif

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	Vec3 camPos = view->pos;
	Vec3 camRot = Transform_GetEulerAnglesDegrees(view);

	glRotatef(-camRot.z, 0, 0, 1);
	glRotatef(-camRot.x, 1, 0, 0);
	glRotatef(-camRot.y, 0, 1, 0);
	glTranslatef(-camPos.x, -camPos.y, -camPos.z);

	#if 0
	if (settings->lightBits != 0)
	{
		R_SetupLights(settings, model);
	}
	#endif

	// *slightly* improvement on horrible jitter from physics transforms.
	// Yeah... also breaks projectile rotations so no ta.
	//wd(model->rotation.cells, 0.0001f);
	Vec3 modelRot = Transform_GetEulerAnglesDegrees(model);
	
	glTranslatef(model->pos.x, model->pos.y, model->pos.z);
	glRotatef(modelRot.y, 0, 1, 0);
	glRotatef(modelRot.x, 1, 0, 0);
	glRotatef(modelRot.z, 0, 0, 1);
	glScalef(model->scale.x, model->scale.y, model->scale.z);
	/*
	glPushMatrix();
	glLoadIdentity();
	#if 1
	glRotatef(-camRot.z, 0, 0, 1);
	glRotatef(-camRot.x, 1, 0, 0);
	glRotatef(-camRot.y, 0, 1, 0);
	glTranslatef(-camPos.x, -camPos.y, -camPos.z);
	#endif
	#if 1
	glTranslatef(model->pos.x, model->pos.y, model->pos.z);
	glRotatef(modelRot.y, 0, 1, 0);
	glRotatef(modelRot.x, 1, 0, 0);
	glRotatef(modelRot.z, 0, 0, 1);
	#endif
	#if 0
	if (settings->lightBits != 0)
	{
		R_SetupLights(settings, model);
	}
	#endif
	glPopMatrix();
	*/
}

// BROKEN
void R_SetViewModelMatrixDirect(Transform *view, Transform *model)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	 // Calculate model/view manually

	// pull out complete camera transform to M4x4
	// offset by model position.
	// multiply by model rotation
	M4x4 view4x4;
	M4x4 mv4x4;
	M4x4_SetToIdentity(mv4x4.cells);
	Transform_ToM4x4(view, &view4x4);
	M4x4_Copy(view4x4.cells, mv4x4.cells);
	mv4x4.posX += model->pos.x;
	mv4x4.posY += model->pos.y;
	mv4x4.posZ += model->pos.z;
	
	M4x4 model4x4;
	Transform_ToM4x4(model, &model4x4);

	// Clear position, position has already been applied above
	M4x4_SetPosition(model4x4.cells, 0, 0, 0);

	M4x4_Multiply(mv4x4.cells, model4x4.cells, mv4x4.cells);

	glLoadMatrixf((GLfloat*)mv4x4.cells);
}

// BROKEN
void R_SetViewModelMatrixNaive(Transform *view, Transform *model)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// VIEW
	M4x4 viewM;
	Transform_ToM4x4(view, &viewM);
	viewM.posX = -viewM.posX;
	viewM.posY = -viewM.posY;
	viewM.posZ = -viewM.posZ;

	// MODEL
	M4x4 modelM;
	Transform_ToM4x4(model, &modelM);
	M4x4 result;
	M4x4_Multiply(viewM.cells, modelM.cells, result.cells);

	// COMBINE
	glLoadMatrixf((GLfloat*)result.cells);
}

// BROKEN
void R_SetViewModelMatrixNaive2(Transform *view, Transform *model)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// --- VIEW ---

	// Split matrices
	M4x4 viewRot;
	Transform_ToM4x4(view, &viewRot);
	M4x4_ClearPosition(viewRot.cells);

	M4x4 viewPos;
	Transform_ToM4x4(view, &viewPos);
	M4x4_ClearRotation(viewPos.cells);

	// viewPos.posX = -viewPos.posX;
	// viewPos.posY = -viewPos.posY;
	// viewPos.posZ = -viewPos.posZ;
	M4x4_Invert(viewPos.cells);
	// M4x4_Invert(viewRot.cells);

	// MODEL
	M4x4 modelRot;
	M4x4 modelPos;
	Transform_ToM4x4(model, &modelRot);
	M4x4_ClearPosition(modelRot.cells);

	Transform_ToM4x4(model, &modelPos);
	M4x4_ClearRotation(modelPos.cells);
	
	// COMBINE
	M4x4 result;
	M4x4_SetToIdentity(result.cells);
	
	M4x4_Multiply(result.cells, viewRot.cells, result.cells);
	M4x4_Multiply(result.cells, viewPos.cells, result.cells);
	M4x4_Multiply(result.cells, modelPos.cells, result.cells);
	M4x4_Multiply(result.cells, modelRot.cells, result.cells);
	
	
	
	glLoadMatrixf((GLfloat*)result.cells);
}

void R_SetModelViewMatrix(RenderSceneSettings* settings, Transform *view, Transform *model)
{
	if (settings->viewModelMode > 3) { settings->viewModelMode = 0; }

	if (settings->viewModelMode == 1)
	{
		R_SetViewModelMatrixDirect(view, model);
		//R_SetViewModelMatrixByEuler(view, model);
	}
	else if (settings->viewModelMode == 2)
	{
		R_SetViewModelMatrixNaive(view, model);
		//R_SetViewModelMatrixByEuler(view, model);
	}
	else if (settings->viewModelMode == 3)
	{
		R_SetViewModelMatrixNaive2(view, model);
		//R_SetViewModelMatrixByEuler(view, model);
	}
	else
	{
		//R_SetViewModelMatrixDirect(view, model);
		R_SetViewModelMatrixByEuler(settings, view, model);
	}
	
	// TODO: Figure out how to construct ViewModel matrix entirely from
	// matrices with no euler angles as they are causing awful jittering
	// with transforms from physics engine
#if 0
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	 // Calculate model/view manually

	// pull out complete camera transform to M4x4
	// offset by model position.
	// multiply by model rotation
	M4x4 view4x4;
	M4x4 mv4x4;
	M4x4_SetToIdentity(mv4x4.cells);
	Transform_ToM4x4(view, &view4x4);
	M4x4_Copy(view4x4.cells, mv4x4.cells);
	mv4x4.posX += model->pos.x;
	mv4x4.posY += model->pos.y;
	mv4x4.posZ += model->pos.z;
	

	M4x4 model4x4;
	Transform_ToM4x4(model, &model4x4);

	// Clear position, position has already been applied above
	M4x4_SetPosition(model4x4.cells, 0, 0, 0);

	M4x4_Multiply(mv4x4.cells, model4x4.cells, mv4x4.cells);

	glLoadMatrixf((GLfloat*)mv4x4.cells);
	

#endif
	// VIEW
#if 0 // Trying to do view without euler angles
	// // ModelView_M = View_M * Model_M
	//f32 viewM[16];
	M4x4 viewM;
	Transform_ToM4x4(view, &viewM);
	//Vec3 angles;
	viewM.posX = -viewM.posX;
	viewM.posY = -viewM.posY;
	viewM.posZ = -viewM.posZ;
	//M4x4_Invert(viewM.cells);
#endif
#if 0 // View via euler angles.
	M4x4 viewM;
	M4x4_SetToIdentity(viewM.cells);
	Vec3 camRot = Transform_GetEulerAngles(view);
	Vec3 camPos = view->pos;
	M4x4_RotateZ(viewM.cells, -camRot.z);
	M4x4_RotateX(viewM.cells, -camRot.x);
	M4x4_RotateY(viewM.cells, -camRot.y);
	M4x4_SetPosition(viewM.cells, -camPos.x, -camPos.y, -camPos.z);
	// glRotatef(-camRot.z, 0, 0, 1);
	// glRotatef(-camRot.x, 1, 0, 0);
	// glRotatef(-camRot.y, 0, 1, 0);
	// glTranslatef(-camPos.x, -camPos.y, -camPos.z);
#endif

#if 0
	// MODEL
	//f32 modelM[16];
	M4x4 modelM;
	Transform_ToM4x4(model, &modelM);
	//f32 result[16];
	M4x4 result;
	M4x4_Multiply(viewM.cells, modelM.cells, result.cells);
	//M4x4_Multiply(modelM.cells, viewM.cells, result.cells);

	// COMBINE
	glLoadMatrixf((GLfloat*)result.cells);
#endif


#if 1 // Calculate model/view using opengl fixed function operations
	// http://www.songho.ca/opengl/gl_transform.html
	// First, transform the camera (viewing matrix) from world space to eye space
	// Notice all values are negated, because we move the whole scene with the
	// inverse of camera transform
	// x = pitch, y = yaw, z = roll
	// Working order
	// view: Z/X/Y or roll/pitch/yaw
	// model is reversed: Y/X/Z
#if 0 // matrices directly... requires matrix multiplication to work
	f32* cells = view->matrix.cells;
	glLoadMatrixf((GLfloat*)cells);

	cells = model->matrix.cells;
	glLoadMatrixf((GLfloat*)cells);
#endif

#if 0 // Extra pos/rot/scale from matrix - broken... ?

	Vec3 camPos = view->pos;
	Vec3 camRot = Transform_GetEulerAnglesDegrees(view);
	// f32 fAngZ = atan2f(openglM[1], openglM[5]);
    // f32 fAngY = atan2f(openglM[8], openglM[10]);
    // f32 fAngX = -asinf(openglM[9]);
	//f32* m = view->

	glRotatef(-camRot.z, 0, 0, 1);
	glRotatef(-camRot.x, 1, 0, 0);
	glRotatef(-camRot.y, 0, 1, 0);
	glTranslatef(-camPos.x, -camPos.y, -camPos.z);

	M3x3_ClearTinyValues(model->rotation.cells, 0.0001f);
	Vec3 modelRot = Transform_GetEulerAnglesDegrees(model);
	
	glTranslatef(model->pos.x, model->pos.y, model->pos.z);
#if 0
	if (modelRot.y != ZNaN())
	{
		//glRotatef(modelRot.y, 0, 1, 0);
	}
	
	if (modelRot.x != ZNaN())
	{
		//glRotatef(modelRot.x, 1, 0, 0);
	}
	
	if (modelRot.z != ZNaN())
	{
		//glRotatef(modelRot.z, 0, 0, 1);
	}
#endif
#if 0
	glRotatef(modelRot.y, 0, 1, 0);
	glRotatef(modelRot.x, 1, 0, 0);
	glRotatef(modelRot.z, 0, 0, 1);
	glScalef(model->scale.x, model->scale.y, model->scale.z);
#endif
// 	char buf[512];
// 	sprintf_s(buf, 512,
// "\nView: Rot: %3.3f, %3.3f, %3.3f\nModel: Rot: %3.3f, %3.3f, %3.3f",
// camRot.x, camRot.y, camRot.z, rotX, rotY, rotZ
// );
// 	OutputDebugStringA(buf);

#endif

#if 0 // Previous system, using Transform with pos/rot/scale vectors

	glRotatef(-view->rot.z, 0, 0, 1);
	glRotatef(-view->rot.x, 1, 0, 0);
	glRotatef(-view->rot.y, 0, 1, 0);
	glTranslatef(-view->pos.x, -view->pos.y, -view->pos.z);

	glTranslatef(model->pos.x, model->pos.y, model->pos.z);
	glRotatef(model->rot.y, 0, 1, 0);
	glRotatef(model->rot.x, 1, 0, 0);
	glRotatef(model->rot.z, 0, 0, 1);
	
	glScalef(model->scale.x, model->scale.y, model->scale.z);
#endif
	// transform the object (model matrix)
	// The result of GL_MODELVIEW matrix will be:
	// ModelView_M = View_M * Model_M
#endif
}

////////////////////////////////////////////////////////////////////
// ModelView - Billboard
// Faces the camera (more or less)
////////////////////////////////////////////////////////////////////
void R_SetModelViewMatrix_Billboard(
	RenderSceneSettings* settings, Transform *view, Transform *model)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	Vec3 camPos = view->pos;
	Vec3 camRot = Transform_GetEulerAnglesDegrees(view);

	glRotatef(-camRot.z, 0, 0, 1);
	glRotatef(-camRot.x, 1, 0, 0);
	glRotatef(-camRot.y, 0, 1, 0);
	glTranslatef(-camPos.x, -camPos.y, -camPos.z);

	// *slightly* improvement on horrible jitter from physics transforms.
	// Yeah... also breaks projectile rotations so no ta.
	//wd(model->rotation.cells, 0.0001f);
	Vec3 modelRot = Transform_GetEulerAnglesDegrees(model);
	
	glTranslatef(model->pos.x, model->pos.y, model->pos.z);
	glRotatef(camRot.y, 0, 1, 0);
	// glRotatef(modelRot.y, 0, 1, 0);
	// glRotatef(modelRot.x, 1, 0, 0);
	// glRotatef(modelRot.z, 0, 0, 1);
	glScalef(model->scale.x, model->scale.y, model->scale.z);
#if 0
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// http://www.songho.ca/opengl/gl_transform.html
	// First, transform the camera (viewing matrix) from world space to eye space
	// Notice all values are negated, because we move the whole scene with the
	// inverse of camera transform
	// x = pitch, y = yaw, z = roll
	
	Vec3 camRot = Transform_GetEulerAnglesDegrees(view);
	
	glRotatef(-view->pos.z, 0, 0, 1);
	glRotatef(-view->pos.x, 1, 0, 0);
	glRotatef(-view->pos.y, 0, 1, 0);
	glTranslatef(-camRot.x, -camRot.y, -camRot.z);

	//Vec4 modelPos = M4x4_GetPosition(model->matrix.cells);
	Vec3 modelRot = Transform_GetEulerAnglesDegrees(model);

	glTranslatef(model->pos.x, model->pos.y, model->pos.z);
	glRotatef(modelRot.y, 0, 1, 0);

	//glTranslatef(modelPos.x, modelPos.y, modelPos.z);
	//glRotatef(view->rot.z, 0, 0, 1);
	//glRotatef(view->rot.x, 1, 0, 0);
	//glRotatef(view->rot.y, 0, 1, 0);
#endif
}
