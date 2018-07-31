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
	UIEntity items[4];
	i32 numItems;
	i32 activeItem;
};

global_variable AppMainMenuPage g_mainMenu;

//global_variable AppMenuPage* g_pageCurrent;
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

    RScene_Init(&g_menuScene, g_menuRenderList, APP_MAX_MENU_ITEMS);

	g_mainMenu = {};
	g_mainMenu.activeItem = 0;
	
	ent = &g_mainMenu.items[0];
	ent->inUse = 1;
	ent->transform.pos.y = 0.5;
	ent->transform.pos.z = -1;
	ent->transform.scale = { 0.5, 0.5, 0.5 };
	RendObj_SetAsBillboard(&ent->rendObj, 1, 1, 0, AppGetTextureIndexByName("textures\\ui_text_menu_title.bmp"));
    
	ent = &g_mainMenu.items[1];
	ent->inUse = 1;
	ent->transform.pos.y = 0;
	ent->transform.pos.z = -1;
	char* placeholderChars2 = "START";
	i32 numChars = COM_StrLen(placeholderChars2);
    RendObj_SetAsAsciCharArray(
        &ent->rendObj, 
        placeholderChars2,
        numChars,
        0.2f,
        AppGetTextureIndexByName("textures\\charset.bmp"),
        1, 0, 0
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
	UI_BuildUIRenderScene(&g_menuScene, g_mainMenu.items, 2);
}

void App_MenuInput(InputActionSet* inputs, GameTime* time)
{
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
}
