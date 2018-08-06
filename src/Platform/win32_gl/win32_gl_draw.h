#pragma once

#include "win32_gl_module.cpp"

void R_SetupTestTexture(i32 textureIndex)
{
	// g_gl_primitive_mode
	GLuint texToBind = g_textureHandles[textureIndex];
    glBindTexture(GL_TEXTURE_2D, texToBind);

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

}

////////////////////////////////////////////////////////////////////
// Draw Quad
////////////////////////////////////////////////////////////////////
void R_DrawSpriteGeometry(f32 posX, f32 posY, f32 posZ, RendObj_Sprite* sprite)
{
	f32 hw = sprite->width * 0.5f;
	f32 hh = sprite->height * 0.5f;
	f32 minX = posX - hw;
	f32 maxX = posX + hw;
	f32 minY = posY - hh;
	f32 maxY = posY + hh;

	minX /= win32_aspectRatio;
	maxX /= win32_aspectRatio;
	
	glBegin(GL_TRIANGLES);

	glColor3f(sprite->r, sprite->g, sprite->b);
	glTexCoord2f(sprite->uvLeft, sprite->uvBottom);
	glVertex3f(minX, minY, posZ);

	glColor3f(sprite->r, sprite->g, sprite->b);
	glTexCoord2f(sprite->uvRight, sprite->uvBottom);
	glVertex3f(maxX, minY, posZ);

	glColor3f(sprite->r, sprite->g, sprite->b);
	glTexCoord2f(sprite->uvRight, sprite->uvTop);
	glVertex3f(maxX, maxY, posZ);

	// upper triangle
	glColor3f(sprite->r, sprite->g, sprite->b);
	glTexCoord2f(sprite->uvLeft, sprite->uvBottom);
	glVertex3f(minX, minY, posZ);

	glColor3f(sprite->r, sprite->g, sprite->b);
	glTexCoord2f(sprite->uvRight, sprite->uvTop);
	glVertex3f(maxX, maxY, posZ);

	glColor3f(sprite->r, sprite->g, sprite->b);
	glTexCoord2f(sprite->uvLeft, sprite->uvTop);
	glVertex3f(minX, maxY, posZ);

	glEnd();
}

////////////////////////////////////////////////////////////////////
// Geometry
////////////////////////////////////////////////////////////////////
void R_RenderTestGeometry_RainbowQuad()
{
    glBegin(GL_TRIANGLES);

	f32 size = 1;

	// lower triangle. Bottom left -> Bottom Right -> Top Right
	glColor3f(1, 0, 0);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(-size, -size);

	glColor3f(0, 1, 0);
	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(size, -size);

	glColor3f(0, 0, 1);
	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(size, size);

	// upper triangle
	glColor3f(1, 0, 0);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(-size, -size);
	
	glColor3f(0, 0, 1);
	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(size, size);

	glColor3f(1, 1, 0);
	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(-size, size);
    glEnd();
}

void R_RenderTestGeometry_ColouredQuad(f32 r, f32 g, f32 b, f32 a)
{
    glBegin(GL_TRIANGLES);

	f32 size = 1;

	// lower triangle. Bottom left -> Bottom Right -> Top Right
	glColor4f(r, g, b, a);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(-size, -size);

	glColor4f(r, g, b, a);
	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(size, -size);

	glColor4f(r, g, b, a);
	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(size, size);

	// upper triangle
	glColor4f(r, g, b, a);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(-size, -size);

	glColor4f(r, g, b, a);
	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(size, size);

	glColor4f(r, g, b, a);
	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(-size, size);

    glEnd();
}

void R_RenderAABBGeometry(f32 x, f32 y, f32 z, f32 sizeX, f32 sizeY, f32 sizeZ, f32 red, f32 green, f32 blue)
{
	f32 halfX = sizeX / 2;
	f32 halfY = sizeY / 2;
	f32 halfZ = sizeZ / 2;
	// Vec3 a = { x - halfX, y - halfY, z - halfZ };
	// Vec3 b = { x + halfX, y + halfY, z + halfZ };
	Vec3 a = { -halfX, -halfY, -halfZ };
	Vec3 b = { halfX, halfY, halfZ };

	glLineWidth(5.0);
	glBegin(GL_LINES);
	//glColor3f(red, green, blue);
	glColor3f(1, 0, 0);
	// front face
	glVertex3f(a.x, a.y, a.z);
	glVertex3f(b.x, a.y, a.z);

	glVertex3f(b.x, a.y, a.z);
	glVertex3f(b.x, b.y, a.z);

	glVertex3f(b.x, b.y, a.z);
	glVertex3f(a.x, b.y, a.z);

	glVertex3f(a.x, b.y, a.z);
	glVertex3f(a.x, a.y, a.z);

	// back face
	glVertex3f(a.x, a.y, b.z);
	glVertex3f(b.x, a.y, b.z);

	glVertex3f(b.x, a.y, b.z);
	glVertex3f(b.x, b.y, b.z);

	glVertex3f(b.x, b.y, b.z);
	glVertex3f(a.x, b.y, b.z);

	glVertex3f(a.x, b.y, b.z);
	glVertex3f(a.x, a.y, b.z);

	// connecting lines
	glVertex3f(a.x, a.y, a.z);
	glVertex3f(a.x, a.y, b.z);

	glVertex3f(b.x, a.y, a.z);
	glVertex3f(b.x, a.y, b.z);

	glVertex3f(b.x, b.y, a.z);
	glVertex3f(b.x, b.y, b.z);

	glVertex3f(a.x, b.y, a.z);
	glVertex3f(a.x, b.y, b.z);
	glEnd();
}

void R_RenderPrimitive(RenderSceneSettings* settings, Transform* camera, Transform* objTransform, RendObj* obj)
{
	glDisable(GL_TEXTURE_2D);
	R_SetModelViewMatrix(settings, camera, objTransform);
	//R_SetupTestTexture();
	RendObj_Primitive* prim = &obj->data.primitive;
	switch (prim->primitiveType)
	{
		case REND_PRIMITIVE_TYPE_SINGLE_COLOUR_QUAD:
		{
			R_RenderTestGeometry_ColouredQuad(prim->red, prim->green, prim->blue, prim->alpha);
		} break;

		case REND_PRIMITIVE_TYPE_RAINBOW_QUAD:
		{
			R_RenderTestGeometry_RainbowQuad();
		} break;

		case REND_PRIMITIVE_TYPE_AABB:
		{
			Vec3 pos = objTransform->pos;
			R_RenderAABBGeometry(
				pos.x, pos.y, pos.z,
				prim->sizeX, prim->sizeY, prim->sizeZ,
				prim->red, prim->green, prim->blue);
		} break;

		default:
		{
			R_RenderTestGeometry_RainbowQuad();
		} break;
	}
}

/*
glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
to switch on,
glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
*/
void R_RenderLine(RenderSceneSettings* settings, Transform* camera, Transform* objTransform, RendObj* obj)
{
	glDisable(GL_TEXTURE_2D);
	//DebugBreak();
	R_SetModelViewMatrix(settings, camera, objTransform);
	RendObj_Line* line = &obj->data.line;
	//glDisable(GL_DEPTH_TEST);
	//glTranslatef(0.0f,0.0f,-10.0f);

	// glLineWidth(10.0);
	// glBegin(GL_LINES);
	// glColor3f(1, 1, 1);
	// glVertex3f(-1, -1, -1);
	// glVertex3f(1, 1, 1);
	// glEnd();

	glLineWidth(10.0);
	glBegin(GL_LINES);

	glColor3f(line->colourA.x, line->colourA.y, line->colourA.z);
	glVertex3f(line->a.x, line->a.y, line->a.z);
	
	glColor3f(line->colourB.x, line->colourB.y, line->colourB.z);
	glVertex3f(line->b.x, line->b.y, line->b.z);
	
	glEnd();
	//glEnable(GL_DEPTH_TEST);
}

void R_RenderAsciChar(RendObj* obj)
{
	glEnable(GL_TEXTURE_2D);

	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	RendObj_AsciChar* c = &obj->data.asciChar;
	R_SetupTestTexture(c->textureIndex);
	R_LoadAsciCharGeometry(c->asciChar, ZTXT_CONSOLE_CHAR_SHEET_WIDTH_PIXELS, 0, 0, 8, win32_aspectRatio);
}

void R_RenderAsciCharArray(RenderSceneSettings* settings, Transform* camera, Transform* objTransform, RendObj* obj)
{
	glEnable(GL_TEXTURE_2D);

	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	if (settings->projectionMode == RENDER_PROJECTION_MODE_IDENTITY)
	{
		glLoadIdentity();
	}
	else
	{
		R_SetModelViewMatrix(settings, camera, objTransform);
	}
	//glScalef(objTransform->scale.x, objTransform->scale.y, objTransform->scale.z);
	//glTranslatef(objTransform->pos.x, objTransform->pos.y, objTransform->pos.z);
	
	RendObj_AsciCharArray* c = &obj->data.charArray;
	glColor3f(c->r, c->g, c->b);
	R_SetupTestTexture(c->textureIndex);
	Vec3 pos = objTransform->pos;

	
	CharArrayGeometryData data = {};
	data.charArray = c->chars;
	data.sheetWidthPixels = ZTXT_CONSOLE_CHAR_SHEET_WIDTH_PIXELS;
	data.alignmentMode = c->alignmentMode;
	data.screenOriginX = objTransform->pos.x;
	data.screenOriginY = objTransform->pos.y;
	data.charSize = c->size;
	data.aspectRatio = win32_aspectRatio;
	data.red = c->r;
	data.green = c->g;
	data.blue = c->b;
	
	R_LoadAsciCharArrayGeometry(settings, &data);

	// R_LoadAsciCharArrayGeometry(
	// 	settings,
	// 	c->chars, ZTXT_CONSOLE_CHAR_SHEET_WIDTH_PIXELS, c->alignmentMode,
	// 	pos.x, pos.y, c->size, win32_aspectRatio);
}

void R_RenderBillboard(RenderSceneSettings* settings, Transform* camera, Transform* objTransform, RendObj* obj)
{
	glEnable(GL_TEXTURE_2D);
	glColor3f(1, 1, 1);
	R_SetModelViewMatrix_Billboard(settings, camera, objTransform);
	//R_SetModelViewMatrix(settings, camera, objTransform);
	RendObj_Billboard* b = &obj->data.billboard;
	R_SetupTestTexture(b->textureIndex);
	R_RenderTestGeometry_ColouredQuad(b->r, b->g, b->b, b->a);
}

void R_RenderSprite(RenderSceneSettings* settings, Transform* camera, Transform* objTransform, RendObj* obj)
{
	//DebugBreak();
	glEnable(GL_TEXTURE_2D);
	RendObj_Sprite* sprite = &obj->data.sprite;
	switch (sprite->mode)
	{
		case SPRITE_MODE_MESH:
		{

		} break;

		case SPRITE_MODE_UI:
		{
			Vec3 pos = objTransform->pos;
			//R_SetupOrthoProjection(8);
			//glMatrixMode(GL_PROJECTION);
			//glLoadIdentity();
			
            // char buf[64];
            // sprintf_s(buf, 64, "SPRITE POS: %3.7f, %3.7f, %3.7f\n", objTransform->pos.x, objTransform->pos.y, objTransform->pos.z);
            // OutputDebugString(buf);


			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glScalef(objTransform->scale.x, objTransform->scale.y, objTransform->scale.z);
			
			R_SetupTestTexture(sprite->textureIndex);
			//R_RenderTestGeometry_RainbowQuad();
			R_DrawSpriteGeometry(pos.x, pos.y, pos.z, sprite);

		} break;

		case SPRITE_MODE_BILLBOARD:
		{

		} break;
	}
}

void R_RenderMesh(RenderSceneSettings* settings, Transform* camera, Transform* objTransform, RendObj* obj)
{
	RendObj_ColouredMesh* meshRend = &obj->data.mesh;
	MeshData mesh = meshRend->mesh;
	AssertAlways(mesh.verts != NULL);
	AssertAlways(mesh.uvs != NULL);
	AssertAlways(mesh.normals != NULL);

	GLuint* texHandles = g_textureHandles;

	glEnable(GL_TEXTURE_2D);
	glColor3f(meshRend->r, meshRend->g, meshRend->b);
	R_SetModelViewMatrix(settings, camera, objTransform);
	R_SetupTestTexture(meshRend->textureIndex);
	
	//f32* meshVerts = mesh->verts;
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, mesh.verts);
	glTexCoordPointer(2, GL_FLOAT, 0, mesh.uvs);
	glDrawArrays(GL_TRIANGLES, 0, mesh.numVerts);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void R_RenderEntity(RenderSceneSettings* settings, Transform* camera, RenderListItem* item)
{
	switch(item->obj.type)
	{
		case RENDOBJ_TYPE_MESH:
		{
			R_RenderMesh(settings, camera, &item->transform,  &item->obj);
		} break;

		case RENDOBJ_TYPE_SPRITE:
		{
			R_RenderSprite(settings, camera, &item->transform,  &item->obj);
		} break;

		case RENDOBJ_TYPE_LINE:
		{
			R_RenderLine(settings, camera, &item->transform,  &item->obj);
		} break;
		
		case RENDOBJ_TYPE_PRIMITIVE:
		{
			R_RenderPrimitive(settings, camera, &item->transform,  &item->obj);
		} break;

		case RENDOBJ_TYPE_ASCI_CHAR_ARRAY:
		{
			R_RenderAsciCharArray(settings, camera, &item->transform,  &item->obj);
		} break;
		
		case RENDOBJ_TYPE_BILLBOARD:
		{
			R_RenderBillboard(settings, camera, &item->transform,  &item->obj);
		} break;

		case RENDOBJ_TYPE_ASCI_CHAR:
		{
			R_RenderAsciChar(&item->obj);
		} break;
	}
}

void R_RenderScene(RenderScene* scene)
{
	R_SetupProjection(scene);
	//R_SetupOrthoProjection(8);
    for (u32 i = 0; i < scene->numObjects; ++i)
    {
		if (scene->sceneItems[i].obj.flags & RENDOBJ_FLAG_DEBUG)
		{
			scene->sceneItems[i].obj.flags |= RENDOBJ_FLAG_DEBUG;
			//DebugBreak();
		}
        R_RenderEntity(&scene->settings, &scene->cameraTransform, &scene->sceneItems[i]);
    }

	// for (u32 i = 0; i < scene->numUIObjects; ++i)
	// {
	// 	R_RenderEntity(&scene->cameraTransform, &scene->uiItems[i]);
	// }
}
