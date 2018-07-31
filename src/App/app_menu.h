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

global_variable AppMenuPage* g_pageCurrent;
global_variable AppMenuPage g_pageMain;

// 0 title graphic, 1 Start, 2 replays, 3 options, 4 quit
global_variable UIEntity g_mainMenuEnts[5];

void App_ChangeMenuPage(AppMenuPage* newPage, AppMenuPage* previousPage)
{
	if (newPage == previousPage) { return; }
	newPage->activeItem = 0;
}

void App_InitMenus()
{
    RScene_Init(&g_menuScene, g_menuRenderList, APP_MAX_MENU_ITEMS);
    
	g_pageMain = {};
	COM_CopyStringLimited("main", g_pageMain.label, 15);
	
	g_pageCurrent = &g_pageMain;

    COM_ZeroMemory((u8*)g_menu_entities, sizeof(UIEntity) * APP_MAX_MENU_ITEMS);
    UIEntity* ent = UI_GetFreeEntity(g_menu_entities, APP_MAX_MENU_ITEMS);
    if (ent == NULL)
    {
        printf("Failed to get free UI entity\n");
        ILLEGAL_CODE_PATH
    }
    ent->transform.pos.y = 0.5;
    ent->transform.pos.z = -1;
    ent->transform.scale = { 0.5, 0.5, 0.5 };
    RendObj_SetAsBillboard(&ent->rendObj, 1, 1, 0, AppGetTextureIndexByName("textures\\ui_text_menu_title.bmp"));
}

void App_BuildMenuRenderScene()
{
	UI_BuildUIRenderScene(&g_menuScene, g_menu_entities, APP_MAX_MENU_ITEMS);
}
