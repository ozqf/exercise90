#pragma once

#include "../../common/com_module.h"

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

void R_MeasureStringSize(char* str, i32* charsWide, i32* charsHigh)
{
	i32 accumulator = 0;
	i32 longestLine = 0;
	i32 numLines = 1;
	while(*str)
	{
		if (*str == '\n')
		{
			if (accumulator > longestLine)
			{
				longestLine = accumulator;
			}
			accumulator = 0;
			numLines++;
		}
		else
		{
			accumulator++;
		}
		str++;
	}
	if (longestLine == 0 || accumulator > longestLine)
	{
		*charsWide = accumulator;
	}
	else
	{
		*charsWide = longestLine;
	}
	
	*charsHigh = numLines;
}

void R_SetTextAlignmentOffsets(
	i32 alignmentMode,
	f32 charSize,
	f32* resultX,
	f32* resultY,
	i32 charsWide,
	i32 charsHigh
)
{
	f32 halfWidth = (f32)charsWide / 2.0f;
	f32 halfHeight = (f32)charsHigh / 2.0f;
	*resultX = 0;
	*resultY = 0;
	switch(alignmentMode)
	{
		case TEXT_ALIGNMENT_MIDDLE_MIDDLE:
		{
			*resultX = -(halfWidth * charSize);
			*resultX /= 2.0f;
			*resultX -= (charSize * 0.5f);
			*resultY = halfHeight * charSize;
			*resultY -= (charSize * 0.5f);
		} break;

		case TEXT_ALIGNMENT_BOTTOM_LEFT:
		{
			*resultY += charsHigh * charSize;
			int b = 0;
		} break;

		case TEXT_ALIGNMENT_TOP_LEFT:
		{
			*resultX = charSize * 0.5f;
			*resultY = -(charSize * 0.5f);
		} break;
	}
	printf("Char size: %.2f, Chars wide/high: %d, %d. Half width/height: %.2f, %.2f\nOffset %.2f, %.2f\n",
		charSize,
		charsWide,
		charsHigh,
		halfWidth,
		halfHeight,
		*resultX,
		*resultY
	);
	//*resultY = -*resultY;
}

void R_LoadAsciCharArrayGeometry(
	char* charArray,
	i32 sheetWidthPixels,
	i32 alignmentMode,
	f32 screenOriginX,
	f32 screenOriginY,
	f32 charSize,
	f32 aspectRatio
	)
{
	if (alignmentMode != 0)
	{
		int c = 0;
	}
	glBegin(GL_TRIANGLES);

	u8* current = (u8*)charArray;

	// Setup values to find uv position on char sheet
	// asci char sheet will be 256 characters, 16x16
    f32 strideX = 1.0f / 16;
    f32 strideY = 1.0f / 16;

	f32 letterWidth = charSize / aspectRatio;
	f32 letterHeight = charSize;

	if (!COM_CompareStrings(charArray, "ITEM 1"))
	{
		int a = 3;
	}

	// calculate alignment
	i32 paragraphWidth;
	i32 paragraphHeight;
	R_MeasureStringSize(charArray, &paragraphWidth, &paragraphHeight);

	f32 alignmentOffsetX = 0;
	f32 alignmentOffsetY = 0;

	R_SetTextAlignmentOffsets(
		alignmentMode,
		charSize,
		&alignmentOffsetX,
		&alignmentOffsetY,
		paragraphWidth,
		paragraphHeight
	);

	// Setup drawing position
	// 0, 0 of chars is their centre so offset by a half
	//screenOriginX = (letterWidth * 0.5f);
	//screenOriginY = -(letterHeight * 0.5f);
	screenOriginX = 0;
	screenOriginY = 0;
	// Apply alignment offset to this
	screenOriginX += alignmentOffsetX;
	screenOriginY += alignmentOffsetY;
	f32 posX = screenOriginX;
	f32 posY = screenOriginY;
	f32 posZ = -1;

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