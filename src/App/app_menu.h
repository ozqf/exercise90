#pragma once

#include "app_module.cpp"

/*
Prototype menus:
Main
	Start
	Scores
		
	Replays
		-Replay List-
		Replay 1 Score, Length, Date, file size
		Replay 2
		...
		<pages>
		Back
	Options
		Controls
	Quit
*/

struct AppMenuPage
{
	char label[16];
	UIEntity *items;
	i32 numItems;
	i32 activeItem;
};

struct AppMainMenuPage
{
	UIEntity items[16];
	i32 numItems;
	i32 activeItem;
};

global_variable AppMainMenuPage g_mainMenu;
global_variable AppMenuPage *g_currentMenu;

//global_variable AppMenuPage g_pageMain;

// 0 title graphic, 1 Start, 2 replays, 3 options, 4 quit
//global_variable UIEntity g_mainMenuEnts[5];

// void App_ChangeMenuPage(AppMenuPage* newPage, AppMenuPage* previousPage)
// {
// 	if (newPage == previousPage) { return; }
// 	newPage->activeItem = 0;
// }

void UI_InitEntAsButton(UIEntity *ent, char *text, char *name, f32 posX, f32 posY)
{
	Transform_SetToIdentity(&ent->transform);
	ent->inUse = 1;
	f32 squareSize = 4; //0.7f;
	ent->halfWidth = 8;
	ent->halfHeight = 0.75f;
	ent->transform.pos = {posX, posY, 1};
	ent->isInteractive = 1;

	i32 nameLength = COM_StrLen(name);
	AssertAlways(nameLength < 16);
	COM_CopyStringLimited(name, ent->name, 16);

	i32 numChars = COM_StrLen(text);
	RendObj_SetAsAsciCharArray(
		&ent->rendObj,
		text,
		numChars,
		1,
		TEXT_ALIGNMENT_MIDDLE_MIDDLE,
		//TEXT_ALIGNMENT_TOP_LEFT,
		AppGetTextureIndexByName("textures\\charset.bmp"),
		1, 1, 1);
	//RendObj_SetAsRainbowQuad(&ent->debugRend);
	RendObj_SetAsColouredQuad(&ent->debugRend, 0.3f, 0.3f, 0.3f);
}

void App_InitMenus()
{
	UIEntity *ent;
	//i32 numChars;
	f32 scale = 1.0f; //0.01f;

	RScene_Init(&g_menuScene, g_menuRenderList, APP_MAX_MENU_ITEMS);
	g_menuScene.settings.projectionMode = RENDER_PROJECTION_MODE_ORTHOGRAPHIC;
	g_menuScene.settings.orthographicHalfHeight = 12;
	Transform_SetRotationDegrees(&g_menuScene.cameraTransform, 0, 180, 180);

	g_mainMenu = {};
	g_mainMenu.activeItem = 0;
	g_mainMenu.numItems = 16;

#if 1
	ent = &g_mainMenu.items[0];
	Transform_SetToIdentity(&ent->transform);
	Transform_SetRotationDegrees(&ent->transform, 0, 0, 0);
	ent->inUse = 1;
	ent->transform.pos.x = 8;
	ent->transform.pos.y = 6;
	ent->transform.pos.z = 4;
	ent->transform.scale = {4, 4, 4};
	ent->halfWidth = 8;
	ent->halfHeight = 4;
	ent->transform.scale.x = 4;
	ent->transform.scale.y = 4;
	ent->transform.scale.z = 4;

#if 1
	RendObj_SetAsMesh(
		&ent->rendObj,
		Assets_GetMeshDataByName("Cube"),
		1, 1, 1,
		AppGetTextureIndexByName("textures\\ui_text_menu_title.bmp"));
#endif
#if 0
	RendObj_SetAsBillboard(
		&ent->rendObj, 1, 1, 0,
		AppGetTextureIndexByName("textures\\ui_text_menu_title.bmp")
	);
	//RendObj_SetAsRainbowQuad(&ent->debugRend);
#endif
	RendObj_SetAsRainbowQuad(&ent->debugRend);
#endif

	ent = &g_mainMenu.items[1];
	UI_InitEntAsButton(ent, "START", "START", 0, 1);

	ent = &g_mainMenu.items[2];
	UI_InitEntAsButton(ent, "OPTIONS", "OPTIONS", 0, -1);

	ent = &g_mainMenu.items[3];
	UI_InitEntAsButton(ent, "QUIT", "QUIT", 0, -3);

#if 0
	// buttons
	ent = &g_mainMenu.items[1];
	Transform_SetToIdentity(&ent->transform);
	ent->inUse = 1;
	f32 squareSize = 4;//0.7f;
	ent->halfWidth = 8;
	ent->halfHeight = 1.0f;
	ent->transform.pos = { 0, 0, 1 };
	char* placeholderChars2 = "START";
	numChars = COM_StrLen(placeholderChars2);
    RendObj_SetAsAsciCharArray(
        &ent->rendObj, 
        placeholderChars2,
        numChars,
        1,
		TEXT_ALIGNMENT_MIDDLE_MIDDLE,
		//TEXT_ALIGNMENT_TOP_LEFT,
        AppGetTextureIndexByName("textures\\charset.bmp"),
        1, 1, 1
    );
	//RendObj_SetAsRainbowQuad(&ent->debugRend);
	RendObj_SetAsColouredQuad(&ent->debugRend, 0.3f, 0.3f, 0.3f);

#endif

	g_mainMenu.numItems = 4;
//g_currentMenu = &g_mainMenu;

//g_pageMain = {};
//COM_CopyStringLimited("main", g_pageMain.label, 15);
//g_pageCurrent = &g_pageMain;
#if 0
    COM_ZeroMemory((u8*)g_menu_entities, sizeof(UIEntity) * APP_MAX_MENU_ITEMS);
	ent = UI_GetFreeEntity(g_menu_entities, APP_MAX_MENU_ITEMS);
    if (ent == NULL)
    {
        printf("Failed to get free UI entity\n");
        ILLEGAL_CODE_PATH
    }
    ent->transform.pos.y = 0.5;
    ent->transform.pos.z = -1;
    ent->transform.scale = { 0.5, 0.5, 0.5 };
    RendObj_SetAsBillboard(&ent->rendObj, 1, 1, 0, AppGetTextureIndexByName("textures\\ui_text_menu_title.bmp"));
#endif
}

void App_BuildMenuRenderScene()
{
	UI_BuildUIRenderScene(&g_menuScene, g_mainMenu.items, g_mainMenu.numItems);
	// if (g_currentMenu != NULL)
	// {
	// 	UI_BuildUIRenderScene(&g_menuScene, g_mainMenu.items, g_mainMenu.numItems);
	// }
}

void UI_MenuUp()
{
}

void UI_MenuDown()
{
}

void UI_MenuLeft()
{
}

void UI_MenuRight()
{
}

i32 App_MouseTestMenu(UIEntity *items, i32 numItems, f32 mouseX, f32 mouseY)
{
	f32 min[2];
	f32 max[2];
	u8 overlapped = 0;
	i32 result = -1;
	for (i32 i = 0; i < numItems; ++i)
	{
		UIEntity *ent = &items[i];
		if (ent->inUse != 1)
		{
			continue;
		}
		if (ent->isInteractive == 0)
		{
			continue;
		}
		ent->rendObj.SetColour(1, 1, 1);
		ent->debugRend.SetColour(0.2f, 0.2f, 0.2f);
		f32 x = ent->transform.pos.x;
		f32 y = ent->transform.pos.y;
		f32 hw = ent->halfWidth;
		f32 hh = ent->halfHeight;

		min[0] = x - hw;
		min[1] = y - hh;
		max[0] = x + hw;
		max[1] = y + hh;
#if 0
		printf("%.2f, %.2f, vs box %.2f, %.2f to %.2f %.2f\n",
			mouseX, mouseY,
			min[0], min[1],
			max[0], max[1]
		);
#endif
		if (mouseX > min[0] && mouseX < max[0] && mouseY > min[1] && mouseY < max[1])
		{
			overlapped = 1;
			result = i;
			ent->rendObj.SetColour(0, 0, 0);
			ent->debugRend.SetColour(1, 1, 1);
		}
	}
	return result;
}

void App_MenuInput(InputActionSet *inputs, GameTime *time, ScreenInfo *info)
{
	// translate mouse pos to 0, 0 in centre, 50% either side
	i32 mouseX = Input_GetActionValue(inputs, "Mouse Pos X");
	i32 mouseY = Input_GetActionValue(inputs, "Mouse Pos Y");

	// printf("Mouse pos and screen size: %d, %d in %d, %d\n",
	// 	mouseX, mouseY, g_screenInfo.width, g_screenInfo.height
	// );

	f32 halfWidth = (f32)info->width * 0.5f;
	f32 halfHeight = (f32)info->height * 0.5f;

	f32 mX = (f32)mouseX - halfWidth;
	f32 mY = (f32)mouseY - halfHeight;
	mY = -mY;
	mX /= halfWidth / (12 * info->aspectRatio);
	mY /= halfHeight / 12;

	i32 itemIndex = App_MouseTestMenu(
		g_mainMenu.items,
		g_mainMenu.numItems,
		(f32)mX,
		(f32)mY
	);
	if (itemIndex == -1) { return; }
	
	if (Input_CheckActionToggledOff(inputs, "Attack 1", time->platformFrameNumber))
	{
		UIEntity *ent = &g_mainMenu.items[itemIndex];
		if (!COM_CompareStrings(ent->name, "START"))
		{
			platform.Platform_WriteTextCommand("MENU START");
		}
		if (!COM_CompareStrings(ent->name, "OPTIONS"))
		{
			platform.Platform_WriteTextCommand("MENU OPTIONS");
		}
		if (!COM_CompareStrings(ent->name, "QUIT"))
		{
			platform.Platform_WriteTextCommand("QUIT");
		}
	}
}

u8 Menu_ParseCommandString(char* str, char** tokens, i32 numTokens)
{
	if (numTokens == 2)
	{
		if (!COM_CompareStrings(tokens[1], "START"))
		{
			printf("  Start game\n");
		}
		if (!COM_CompareStrings(tokens[1], "OPTIONS"))
		{
			printf("  Goto options\n");
		}
	}
	return 1;
}
