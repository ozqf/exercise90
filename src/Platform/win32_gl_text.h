#pragma once

#include "../Shared/shared.h"

#include "gl/gl.h"

#define ZTXT_CONSOLE_CHAR_SHEET_WIDTH_PIXELS 256
#define ZTXT_CONSOLE_CHAR_SHEET_WIDTH_CHARS 16
#define ZTXT_CONSOLE_CHAR_STRIDE 1 / 16

void R_LoadAsciCharGeometry(
	u8 asciChar,
	i32 sheetWidthPixels,
	f32 screenX,
	f32 screenY,
	f32 charsPerScreenHalf,
	f32 aspectRatio
	)
{
    //DebugBreak();
    #if 1
    i32 resultX = asciChar % ZTXT_CONSOLE_CHAR_SHEET_WIDTH_CHARS;
    i32 resultY = asciChar / ZTXT_CONSOLE_CHAR_SHEET_WIDTH_CHARS;
    //resultY = resultY / ZTXT_CONSOLE_CHAR_STRIDE;
    // Sheet is top -> down but opengl is bottom -> up so flip the Y coord
    resultY = (ZTXT_CONSOLE_CHAR_SHEET_WIDTH_CHARS - 1) - resultY;
	
	//f32 letterWidth = 1 / (charsPerScreenHalf / aspectRatio);
	f32 letterWidth = (aspectRatio - 1) / charsPerScreenHalf;
	f32 letterHeight = 1 / charsPerScreenHalf;

	f32 stride = 1.0 / 16.0;

    glBegin(GL_TRIANGLES);

	f32 sizeX = letterWidth;
	f32 sizeY = letterHeight;
    f32 zPos = -1;

    f32 left = (f32)stride * (f32)resultX;
    f32 right = left + stride;

    f32 bottom = (f32)stride* (f32)resultY;
    f32 top = bottom + stride;

	// lower triangle. Bottom left -> Bottom Right -> Top Right
	//glColor4f(1, 1, 1, 1);
	glTexCoord2f(left, bottom);
	glVertex3f(-sizeX, -sizeY, zPos);

	//glColor4f(1, 1, 1, 1);
	glTexCoord2f(right, bottom);
	glVertex3f(sizeX, -sizeY, zPos);

	//glColor4f(1, 1, 1, 1);
	glTexCoord2f(right, top);
	glVertex3f(sizeX, sizeY, zPos);

	// upper triangle
	//glColor4f(1, 1, 1, 1);
	glTexCoord2f(left, bottom);
	glVertex3f(-sizeX, -sizeY, zPos);

	//glColor4f(1, 1, 1, 1);
	glTexCoord2f(right, top);
	glVertex3f(sizeX, sizeY, zPos);

	//glColor4f(1, 1, 1, 1);
	glTexCoord2f(left, top);
	glVertex3f(-sizeX, sizeY, zPos);

    glEnd();
    #endif

    #if 0
    glBegin(GL_TRIANGLES);

	f32 size = 1;
    f32 zPos = -1;

	// lower triangle. Bottom left -> Bottom Right -> Top Right
	glColor3f(1, 0, 0);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-size, -size, zPos);

	glColor3f(0, 1, 0);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(size, -size, zPos);

	glColor3f(0, 0, 1);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(size, size, zPos);

	// upper triangle
	glColor3f(1, 0, 0);
	glVertex3f(-size, -size, zPos);

	glColor3f(0, 0, 1);
	glVertex3f(size, size, zPos);

	glColor3f(1, 1, 0);
	glVertex3f(-size, size, zPos);

    glEnd();
    #endif
}
