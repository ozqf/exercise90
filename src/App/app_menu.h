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
	UIEntity* items;
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
global_variable AppMenuPage* g_currentMenu;

//global_variable AppMenuPage g_pageMain;

// 0 title graphic, 1 Start, 2 replays, 3 options, 4 quit
//global_variable UIEntity g_mainMenuEnts[5];

// void App_ChangeMenuPage(AppMenuPage* newPage, AppMenuPage* previousPage)
// {
// 	if (newPage == previousPage) { return; }
// 	newPage->activeItem = 0;
// }

void App_InitMenus()
{
	UIEntity* ent;
	i32 numChars;
	f32 scale = 1.0f;//0.01f;

    RScene_Init(&g_menuScene, g_menuRenderList, APP_MAX_MENU_ITEMS);

	g_mainMenu = {};
	g_mainMenu.activeItem = 0;
	g_mainMenu.numItems = 16;

	ent = &g_mainMenu.items[0];
	ent->inUse = 1;
	ent->transform.pos.y = 0.5;
	ent->transform.pos.z = -1;
	ent->transform.scale = { 0.5, 0.5, 0.5 };
	RendObj_SetAsBillboard(&ent->rendObj, 1, 1, 0, AppGetTextureIndexByName("textures\\ui_text_menu_title.bmp"));
    
	ent = &g_mainMenu.items[1];
	ent->inUse = 1;
	ent->halfWidth = 20 * scale;
	ent->halfHeight = 5 * scale;
	ent->transform.pos.x = 0;//-0.25f;
	ent->transform.pos.y = 0;//0.1f;
	ent->transform.pos.z = -1;
	char* placeholderChars2 = "ITEM 1";
	numChars = COM_StrLen(placeholderChars2);
    RendObj_SetAsAsciCharArray(
        &ent->rendObj, 
        placeholderChars2,
        numChars,
        0.1f,
		TEXT_ALIGNMENT_MIDDLE_MIDDLE,
        AppGetTextureIndexByName("textures\\charset.bmp"),
        0, 0, 1
    );

	ent = &g_mainMenu.items[2];
	ent->inUse = 1;
	ent->halfWidth = 20 * scale;
	ent->halfHeight = 5 * scale;
	ent->transform.pos.x = -0.25f;
	ent->transform.pos.y = -0.1f;
	ent->transform.pos.z = -1;
	char* placeholderChars3 = "ITEM 2";
	numChars = COM_StrLen(placeholderChars3);
    RendObj_SetAsAsciCharArray(
        &ent->rendObj, 
        placeholderChars3,
        numChars,
        0.1f,
		TEXT_ALIGNMENT_MIDDLE_MIDDLE,
        AppGetTextureIndexByName("textures\\charset.bmp"),
        0, 1, 0
    );
	
	ent = &g_mainMenu.items[3];
	ent->inUse = 1;
	ent->halfWidth = 20 * scale;
	ent->halfHeight = 5 * scale;
	ent->transform.pos.x = -0.25f;
	ent->transform.pos.y = -0.3f;
	ent->transform.pos.z = -1;
	char* placeholderChars4 = "ITEM 3";
	numChars = COM_StrLen(placeholderChars4);
    RendObj_SetAsAsciCharArray(
        &ent->rendObj, 
        placeholderChars4,
        numChars,
        0.1f,
		TEXT_ALIGNMENT_MIDDLE_MIDDLE,
        AppGetTextureIndexByName("textures\\charset.bmp"),
        0, 1, 0
    );
	
	g_mainMenu.numItems = 2;

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
	//UI_BuildUIRenderScene(&g_menuScene, g_menu_entities, APP_MAX_MENU_ITEMS);
	if (g_currentMenu != NULL)
	{
}

	UI_BuildUIRenderScene(&g_menuScene, g_mainMenu.items, g_mainMenu.numItems);
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

void App_MouseTestMenu(UIEntity* items, i32 numItems, f32 mouseX, f32 mouseY)
{
	for (i32 i = 0; i < numItems; ++i)
	{
		UIEntity* ent = &items[i];
		if (ent->inUse != 1) { continue; }
		ent->rendObj.SetColour(1, 1, 1);
		f32 x = ent->transform.pos.x;
		f32 y = ent->transform.pos.y;
		f32 hw = ent->halfWidth;
		f32 hh = ent->halfHeight;

		//printf("%.2f, %.2f, size %.2f, %.2f vs %.2f %.2f\n", x, y, hw, hh, mouseX, mouseY);
		if (mouseX > (x - hw)
			&& mouseX < (x + hw)
			&& mouseY  > (y - hh)
			&& mouseY  < (y + hh)
			)
		{
			ent->rendObj.SetColour(1, 1, 0);
			printf("Over %d\n", i);
		}
	}
}

void App_MenuInput(InputActionSet* inputs, GameTime* time)
{
	// translate mouse pos to 0, 0 in centre, 50% either side
	f32 mouseX = (f32)Input_GetActionValue(inputs, "Mouse Pos X") / 10000.0f;
	f32 mouseY = (f32)Input_GetActionValue(inputs, "Mouse Pos Y") / 10000.0f;
	mouseX -= 50.0f;
	mouseY -= 50.0f;
	mouseY = -mouseY;
	//mouseX /= 50.0f;
	//mouseY /= 50.0f;
	//printf("Mouse pos: %.4f, %.4f\n", mouseX, mouseY);
	
	if (Input_CheckActionToggledOn(inputs, "Move Backward", time->platformFrameNumber))
	{
		UIEntity* ent = &g_mainMenu.items[0];
		if (ent->rendObj.type == RENDOBJ_TYPE_BILLBOARD)
		{
			RendObj_Billboard* board = &ent->rendObj.data.billboard;
			if (ent->state == 0)
			{
				ent->state = 1;
				board->r = 0;
				board->g = 1;
				board->b = 0;
			}
			else if (ent->state == 1)
			{
				ent->state = 2;
				board->r = 0;
				board->g = 0;
				board->b = 1;
			}
			else
			{
				ent->state = 0;
				board->r = 1;
				board->g = 0;
				board->b = 0;
			}
		}
	}
	if (Input_CheckActionToggledOn(inputs, "Move Forward", time->platformFrameNumber))
	{

	}

	App_MouseTestMenu(g_mainMenu.items, g_mainMenu.numItems, mouseX, mouseY);
}
