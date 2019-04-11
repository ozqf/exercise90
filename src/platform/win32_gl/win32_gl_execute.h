#pragma once

#include "win32_gl_module.cpp"

void R_ApplySceneSettings(RenderSceneSettings* settings)
{
    R_SetupProjection(settings);
	if (settings->lightBits == 0)
	{
		glDisable(GL_LIGHTING);
	}
	else
	{
		glEnable(GL_LIGHTING);
	}
}

void R_SetViewport(RViewPort* v)
{
	//printf("R Set viewport: %d, %d - %d, %d\n",
	//	v->viewPortX, v->viewPortY, v->viewPortWidth, v->viewPortHeight);
    glViewport(v->viewPortX, v->viewPortY, v->viewPortWidth, v->viewPortHeight);
}

void Exec_DrawObject(RendObj* obj)
{
	if (obj->type != RENDOBJ_TYPE_MESH) { return; }

	#if 0
	// If you ever want to see the currently loaded matrix...
	GLfloat results[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, results);
	printf("Draw object ViewModel Matrix\n");
	printf("%f, %f, %f, %f\n", results[0], results[4], results[8], results[12]);
	printf("%f, %f, %f, %f\n", results[1], results[5], results[9], results[13]);
	printf("%f, %f, %f, %f\n", results[2], results[6], results[10], results[14]);
	printf("%f, %f, %f, %f\n", results[3], results[7], results[11], results[15]);
	printf("\n");
	#endif

	RendObj_ColouredMesh* meshRend = &obj->data.mesh;
	MeshData mesh = meshRend->mesh;
	COM_ASSERT(mesh.verts != NULL, "Mesh verts are null");
	COM_ASSERT(mesh.uvs != NULL, "Mesh UVs are null");
	COM_ASSERT(mesh.normals != NULL, "Mesh normals are null");

	GLuint* texHandles = g_textureHandles;

	glEnable(GL_TEXTURE_2D);
	glColor3f(meshRend->r, meshRend->g, meshRend->b);

	R_SetupTestTexture(meshRend->textureIndex);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, mesh.verts);
	glTexCoordPointer(2, GL_FLOAT, 0, mesh.uvs);
	glDrawArrays(GL_TRIANGLES, 0, mesh.numVerts);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void R_Execute(RenderCommand* commands, i32 numCommands, PlatformTime* time)
{
    #if 1
    //RenderSceneSettings* settings = NULL;
	for (i32 i = 0; i < numCommands; ++i)
	{
		RenderCommand* cmd = &commands[i];
		switch (cmd->type)
		{
            case REND_CMD_TYPE_DRAW:
			{
				Exec_DrawObject(&cmd->drawItem.obj);
			} break;

			case REND_CMD_TYPE_MODELVIEW:
            {
				glMatrixMode(GL_MODELVIEW);
				glLoadMatrixf(cmd->matrix.cells);
            } break;
			
			case REND_CMD_TYPE_PROJECTION:
			{
				glMatrixMode(GL_PROJECTION);
				glLoadMatrixf(cmd->matrix.cells);
			} break;
            
            /*case REND_CMD_TYPE_SETTINGS:
            {
                settings = &cmd->settings;
                R_ApplySceneSettings(settings);
            } break;

			case REND_CMD_TYPE_SET_VIEWPORT:
            {
                R_SetViewport(&cmd->viewPort);
            } break;*/
		}
	}
    #endif
}

