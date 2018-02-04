#pragma once

#include <math.h>
#include "shared.h"

/////////////////////////////////////////////////////////////////////////////
// 4x4 MATRIX OPERATIONS
/////////////////////////////////////////////////////////////////////////////

void M4x4_SetToIdentity(f32* matrix)
{
    matrix[X0] = 1; matrix[X1] = 0; matrix[X2] = 0; matrix[X3] = 0;
    matrix[Y0] = 0; matrix[Y1] = 1; matrix[Y2] = 0; matrix[Y3] = 0;
    matrix[Z0] = 0; matrix[Z1] = 0; matrix[Z2] = 1; matrix[Z3] = 0;
    matrix[W0] = 0; matrix[W1] = 0; matrix[W2] = 0; matrix[W3] = 1;
}

void M4x4_SetAsScale(f32* matrix, f32 scaleX, f32 scaleY, f32 scaleZ)
{
    matrix[X0] = scaleX;
    matrix[Y1] = scaleY;
    matrix[Z2] = scaleZ;
}

void M4x4_SetAsMove(f32* matrix, f32 deltaX, f32 deltaY, f32 deltaZ)
{
    matrix[X3] = deltaX;
    matrix[Y3] = deltaY;
    matrix[Z3] = deltaZ;
}

void M4x4_SetRotation_Z(f32* matrix, f32 theta)
{
    matrix[X0] = (f32)cos(theta);
    matrix[X1] = (f32)-sin(theta);
    matrix[Y0] = (f32)sin(theta);
    matrix[Y1] = (f32)cos(theta);
}

void M4x4_SetProjection(f32* m, f32 prjNear, f32 prjFar, f32 prjLeft, f32 prjRight, f32 prjTop, f32 prjBottom)
{
    m[0] = (2 * prjNear) / (prjRight - prjLeft);
	m[4] = 0;
	m[8] = (prjRight + prjLeft) / (prjLeft - prjRight);
	m[12] = 0;
	
	m[1] = 0;
	m[5] = (2 * prjNear) / (prjTop - prjBottom);
	m[9] = (prjTop + prjBottom) / (prjTop - prjBottom);
	m[13] = 0;
	
	m[2] = 0;
	m[6] = 0;
	m[10] = -(prjFar + prjNear) / (prjFar - prjNear);
	m[14] = (-2 * prjFar * prjNear) / (prjFar - prjNear);
	
	m[3] = 0;
	m[7] = 0;
	m[11] = -1;
	m[15] = 0;
}

void Vec3_MultiplyByMatrix(f32* m, f32* v)
{
    v[VEC_X] =   (m[X0] * v[VEC_X]) + (m[X1] * v[VEC_Y]) + (m[X2] * v[VEC_Z]) + (m[X3] * v[VEC_W]);
    v[VEC_Y] =   (m[Y0] * v[VEC_X]) + (m[Y1] * v[VEC_Y]) + (m[Y2] * v[VEC_Z]) + (m[Y3] * v[VEC_W]);
    v[VEC_Z] =   (m[Z0] * v[VEC_X]) + (m[Z1] * v[VEC_Y]) + (m[Z2] * v[VEC_Z]) + (m[Z3] * v[VEC_W]);
    v[VEC_W] =   (m[W0] * v[VEC_X]) + (m[W1] * v[VEC_Y]) + (m[W2] * v[VEC_Z]) + (m[W3] * v[VEC_W]);
}

f32 COM_CapAngleDegrees(f32 angle)
{
    u32 loopCount = 0;
    while (angle > 360)
    {
        angle -= 360;
        loopCount++;
        Assert(loopCount < 99999);
    }
    loopCount = 0;
    while (angle < 0)
    {
        angle += 360;
        loopCount++;
        Assert(loopCount < 99999);
    }
    return angle;
}

/////////////////////////////////////////////////////////////////////////////
// Rotations
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// convert Euler angles(x,y,z) to axes(left, up, forward)
// Each column of the rotation matrix represents left, up and forward axis.
// The order of rotation is Roll->Yaw->Pitch (Rx*Ry*Rz)
// Rx: rotation about X-axis, pitch
// Ry: rotation about Y-axis, yaw(heading)
// Rz: rotation about Z-axis, roll
//    Rx           Ry          Rz
// |1  0   0| | Cy  0 Sy| |Cz -Sz 0|   | CyCz        -CySz         Sy  |
// |0 Cx -Sx|*|  0  1  0|*|Sz  Cz 0| = | SxSyCz+CxSz -SxSySz+CxCz -SxCy|
// |0 Sx  Cx| |-Sy  0 Cy| | 0   0 1|   |-CxSyCz+SxSz  CxSySz+SxCz  CxCy|
///////////////////////////////////////////////////////////////////////////////
// http://www.songho.ca/opengl/gl_anglestoaxes.html
void AngleToAxes(Vec3* angles, Vec3* left, Vec3* up, Vec3* forward)
{
	//f32 DEG2RAD = 3.141593 / 180;
    f32 sx, sy, sz, cx, cy, cz, theta;

    // rotation angle about X-axis (pitch)
    theta = angles->x * DEG2RAD;
    sx = (f32)sin(theta);
    cx = (f32)cos(theta);

    // rotation angle about Y-axis (yaw)
    theta = angles->y * DEG2RAD;
    sy = (f32)sin(theta);
    cy = (f32)cos(theta);

    // rotation angle about Z-axis (roll)
    theta = angles->z * DEG2RAD;
    sz = (f32)sin(theta);
    cz = (f32)cos(theta);

    // determine left axis
    left->x = cy*cz;
    left->y = sx*sy*cz + cx*sz;
    left->z = -cx*sy*cz + sx*sz;

    // determine up axis
    up->x = -cy*sz;
    up->y = -sx*sy*sz + cx*cz;
    up->z = cx*sy*sz + sx*cz;

    // determine forward axis
    forward->x = sy;
    forward->y = -sx*cy;
    forward->z = cx*cy;
}

///////////////////////////////////////////////////////////////////////////////
// compute transform axis from object position, target and up direction
///////////////////////////////////////////////////////////////////////////////
// http://www.songho.ca/opengl/gl_lookattoaxes.html
void LookAtToAxes(Vec3* pos, Vec3* target, Vec3* upDir, Vec3* left, Vec3* up, Vec3* forward)
{
    // compute the forward vector
    forward->x = target->x - pos->x;
    forward->y = target->y - pos->y;
    forward->z = target->z - pos->z;
	Vec3_Normalise(forward);
    //forward.normalize();

    // compute the left vector
	Vec3_CrossProduct(upDir, forward, left);
	Vec3_Normalise(left);
    //left = upDir.cross(forward);  // cross product
    //left.normalize();

    // compute the orthonormal up vector
	Vec3_CrossProduct(forward, left, up);
	Vec3_Normalise(up);
    //up = forward.cross(left);     // cross product
    //up.normalize();
}
