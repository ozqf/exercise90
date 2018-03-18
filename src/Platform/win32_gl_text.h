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
}
void R_LoadAsciCharArrayGeometry(
	char* charArray,
	i32 sheetWidthPixels,
	f32 screenOriginX,
	f32 screenOriginY,
	f32 charsPerScreenHalf,
	f32 aspectRatio
	)
{
	glBegin(GL_TRIANGLES);

	u8* current = (u8*)charArray;
	f32 posX = screenOriginX;
	f32 posY = screenOriginY;
	f32 posZ = 0;

	// asci char sheet will be 256 characters, 16x16
    f32 strideX = 1.0f / 16;
    f32 strideY = 1.0f / 16;

	f32 letterWidth = (aspectRatio - 1) / charsPerScreenHalf;
	f32 letterHeight = 1 / charsPerScreenHalf;
    
	while (*current)
	{
		if (*current == '\n')
		{
			posX = screenOriginX;
			posY -= letterHeight;
			++current;
			continue;
		}

    	i32 sheetX = *current % 16;
    	i32 sheetY = *current / 16;
    	// Sheet is top -> down but opengl is bottom -> up so flip the Y coord
    	sheetY = (16 - 1) - sheetY;

    	f32 uvLeft = (f32)strideX * (f32)sheetX;
    	f32 uvRight = (f32)uvLeft + (f32)strideX;

    	f32 uvBottom = (f32)strideY * (f32)sheetY;
    	f32 uvTop = (f32)uvBottom + (f32)strideY;

		f32 hw = letterWidth * 0.5f;
		f32 hh = letterHeight * 0.5f;
		f32 minX = posX - hw;
		f32 maxX = posX + hw;
		f32 minY = posY - hh;
		f32 maxY = posY + hh;

		// lower triangle. Bottom left -> Bottom Right -> Top Right
		//glColor3f(sprite->r, sprite->g, sprite->b);
		glTexCoord2f(uvLeft, uvBottom);
		glVertex3f(minX, minY, posZ);

		//glColor3f(sprite->r, sprite->g, sprite->b);
		glTexCoord2f(uvRight, uvBottom);
		glVertex3f(maxX, minY, posZ);

		//glColor3f(sprite->r, sprite->g, sprite->b);
		glTexCoord2f(uvRight, uvTop);
		glVertex3f(maxX, maxY, posZ);

		// upper triangle
		//glColor3f(sprite->r, sprite->g, sprite->b);
		glTexCoord2f(uvLeft, uvBottom);
		glVertex3f(minX, minY, posZ);

		//glColor3f(sprite->r, sprite->g, sprite->b);
		glTexCoord2f(uvRight, uvTop);
		glVertex3f(maxX, maxY, posZ);

		//glColor3f(sprite->r, sprite->g, sprite->b);
		glTexCoord2f(uvLeft, uvTop);
		glVertex3f(minX, maxY, posZ);

		// Step
		++current;
		posX += letterWidth;
	}

	glEnd();
}