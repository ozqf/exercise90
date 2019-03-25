#pragma once

#include <stdio.h>

#include "../common/com_defines.h"
#include "../interface/app_interface.h"
#include "../interface/platform_interface.h"
#include "../app/app_textures.h"

#include "../interface/sys_events.h"

internal AppPlatform g_platform;

internal Heap g_heap;

#define APP_MAX_MALLOCS 1024
internal MallocItem g_mallocItems[APP_MAX_MALLOCS];
internal MallocList g_mallocs;

#define MAX_WORLD_SCENE_ITEMS 2048
internal RenderScene g_worldScene;
internal RenderListItem g_worldSceneItems[MAX_WORLD_SCENE_ITEMS];

internal i32 g_cubeTextureIndex = 0;

// comment out to disable logging/printing by app layer
#define APP_FULL_LOGGING

#ifdef APP_FULL_LOGGING
#define APP_LOG(messageBufSize, format, ...) \
{ \
    char appLogBuf[##messageBufSize##]; \
    sprintf_s(appLogBuf, messageBufSize##, format##, ##__VA_ARGS__##); \
    App_Log(appLogBuf); \
}

#define APP_PRINT(messageBufSize, format, ...) \
{ \
    char appLogBuf[##messageBufSize##]; \
    sprintf_s(appLogBuf, messageBufSize##, format##, ##__VA_ARGS__##); \
    App_Print(appLogBuf); \
}
#else
#define APP_LOG(messageBufSize, format, ...)
#define APP_PRINT(messageBufSize, format, ...)
#endif


/***************************************
* Public (app.h)
***************************************/
void App_Log(char* msg)
{
    g_platform.Log(msg);
}

void App_Print(char* msg)
{
    g_platform.Print(msg);
}

void App_SetMouseMode(ZMouseMode mode)
{
	g_platform.SetMouseMode(mode);
}

void App_Error(char* msg, char* heading)
{
    g_platform.Error(msg, heading);
}

internal f32 App_CalcInterpolationTime(f32 accumulator, f32 interval)
{
    return (accumulator / interval);
}

internal i32  g_isValid = 0;

void Tex_RGBA2BW(Texture2DHeader* tex, u8* target)
{
    u32* source = tex->ptrMemory;
    i32 numPixels = tex->width * tex->height;
    for (i32 i = 0; i < numPixels; i += 8)
    {
        // read eight pixel block
        u8 result = 0;
        for (i32 bit = 0; bit < 8; ++bit)
        {
            ColourU32 col = *((ColourU32*)(source));
            if (col.r || col.g || col.b)
            {
                result |= (1 << bit);
            }
            source++;
        }
        *target = result;
        target++;
    }
}

void Tex_BW2BGBA(u8* source, Texture2DHeader* tex)
{
    u16 width = *(u16*)source;
    source += sizeof(u16);
    u16 height = *(u16*)source;
    source += sizeof(u16);
    
}

internal i32 GenAndBindTestTexture()
{
    /*
    Point size = { 2, 2 };
    i32 totalBlocks = size.x * size.y;
    i32 numBytes = sizeof(BWImage) + (totalBlocks * sizeof(BW8x8Block));
    u8* mem = (u8*)malloc(numBytes);
    COM_ZeroMemory(mem, numBytes);
    BWImage* img = (BWImage*)mem;
    *img = {};
    img->size = size;
    img->blocks = (BW8x8Block*)(mem + sizeof(BWImage));

    Tex_BWSetAllPixels(&img->blocks[0]);
    Tex_BWSetAllPixels(&img->blocks[3]);
    */
    Texture2DHeader* h = Tex_AllocateTexture("test_source.bmp", 256, 256);
    
    ColourU32 col = { 0, 0, 0, 255 };
    //TexDraw_Outline(h, col);
    //TexDraw_FillRect(h, { 1, 1 }, { 6, 6}, { 0, 255, 0, 255 });
    //TexDraw_FillRect(h, { -10, -10 }, { 20, 20 }, { 0, 255, 255, 255 });
    #if 0
    TexDraw_Line(
        h, { 255, 0, 0, 255 }, 0, 0, h->width - 1, h->height - 1
    );
    TexDraw_Line(
        h, { 0, 0, 255, 255 }, h->width - 1, 0, 0, h->height - 1
    );
    #endif
    //TexDraw_Gradient(h, 1);
    i32 numBWPixels = Tex_CalcBytesForBWPixels(h->width, h->height);
    i32 sizeOfBWHeader = sizeof(u16) + sizeof(u16);
    u8* bwMem = (u8*)malloc(sizeOfBWHeader + numBWPixels);
    u8* bw = bwMem;
    *(u16*)bw = (u16)h->width;
    bw += sizeof(u16);
    *(u16*)bw = (u16)h->height;
    bw += sizeof(u16);

    Tex_RGBA2BW(h, bw);

    Texture2DHeader* result = Tex_AllocateTexture("test_result.bmp", 256, 256);

    Tex_BindTexture(result);
    return h->index;
}

internal BWImage* EncodeBW()
{
    
    Texture2DHeader* h = Tex_GetTextureByName(DEFAULT_CONSOLE_CHARSET_PATH);
    Point blockDimensions = Tex_CalcBWImageSizeFromBitmap(h);
    if (!blockDimensions.x || !blockDimensions.y)
    {
        return NULL;
    }
	
    // > Calc required memory
	// > Alloc
	// > generate
    i32 totalBlocks = blockDimensions.x * blockDimensions.y;
    i32 numBytes = sizeof(BWImage) + (totalBlocks * sizeof(BW8x8Block));
    u8* mem = (u8*)malloc(numBytes);
    COM_ZeroMemory(mem, numBytes);
    BWImage* img = (BWImage*)mem;
    *img = {};
    img->size = blockDimensions;
    img->blocks = (BW8x8Block*)(mem + sizeof(BWImage));
    Tex_GenerateBW(h, img);
    return img;
}

internal Texture2DHeader* DecodeBW(BWImage* img)
{
    Point size = Tex_CalcInternalImageSizeFromBW(img);
    i32 numPixels = size.x * size.y;
    i32 numBytes = sizeof(Texture2DHeader) + (numPixels * sizeof(u32));
    printf("Decode tex %d by %d (%d bytes)\n", size.x, size.y, numBytes);
    u8* mem = (u8*)malloc(numBytes);
    Texture2DHeader* h = (Texture2DHeader*)mem;
    h->width = size.x;
    h->height = size.y;
    h->ptrMemory = (u32*)(mem + sizeof(Texture2DHeader));
    Tex_BW2Bitmap(img, h);
    return h;
}

internal i32 App_Init()
{
    APP_LOG(128, "App initialising. Build data %s - %s\n", __DATE__, __TIME__);
    
    // Acquiring an old 'heap object here. Various platform functions
    // still use it for loading assets so can't remove. Future allocations
    // should just use the basic malloc tracker until further notice
    u32 heapMB = 64;
    u32 mainMemorySize = MegaBytes(heapMB);
    MemoryBlock mem = {};

    APP_LOG(128, "APP Requested %d MB for Heap\n", heapMB);

    if (!g_platform.Malloc(&mem, mainMemorySize))
    {
        APP_LOG(128, "APP Platform malloc failed\n");
        Assert(false);
        return 0;
    }
    else
    {
        Heap_Init(&g_heap, mem.ptrMemory, mem.size);
    }

    // Assets
    Tex_Init(&g_heap, g_platform);
    char* textures[] = 
    {
        "textures\\BitmapTest.bmp",
        DEFAULT_CONSOLE_CHARSET_PATH,
        "textures\\brbrick2.bmp",
        "textures\\W33_5.bmp",
        "textures\\COMP03_1.bmp",
        "\0"
    };
    Tex_LoadTextureList(textures);
    
    char* texName = "textures\\white_bordered.bmp";
    //char* texName = "textures\\W33_5.bmp";
    g_cubeTextureIndex = Tex_GetTextureIndexByName(texName);
    g_cubeTextureIndex = GenAndBindTestTexture();
    //BWImage* img = EncodeBW();
    //Texture2DHeader* tex = DecodeBW(img);
    //g_platform.SaveBMP(tex);

    g_platform.SetDebugInputTextureIndex(
        Tex_GetTextureIndexByName(DEFAULT_CONSOLE_CHARSET_PATH));

    // Render Scenes - orient camera
    RScene_Init(&g_worldScene, g_worldSceneItems, MAX_WORLD_SCENE_ITEMS,
		90, RENDER_PROJECTION_MODE_3D, 8);
    g_worldScene.cameraTransform.pos.z = 8;
    g_worldScene.cameraTransform.pos.y += 12;
    Transform_SetRotation(&g_worldScene.cameraTransform, -(67.5f    * DEG2RAD), 0, 0);

    //char* sizeTestName = "\\sounds\\Shield_Pickup.wav";
    char* sizeTestName = "\\gamex86.dll";
    u32 sizeTest = g_platform.MeasureFile(sizeTestName);
    APP_PRINT(256, "Size of %s: %d\n", sizeTestName, sizeTest);


    return COM_ERROR_NONE;
}

internal i32  App_Shutdown()
{
    APP_LOG(128, "App Shutdown\n");
    
    // Free memory, assuming a new APP might be loaded in it's place
    MemoryBlock mem = {};
    mem.ptrMemory = g_heap.ptrMemory;
    mem.size = g_heap.size;
    g_platform.Free(&mem);
	
    return COM_ERROR_NONE;
}

internal i32 App_EndSession()
{
    return COM_ERROR_NONE;
}

internal i32  App_RendererReloaded()
{
    Tex_RenderModuleReloaded();
    return COM_ERROR_NONE;
}

internal void App_Input(PlatformTime* time, ByteBuffer commands)
{
    
}

internal void App_Update(PlatformTime* time)
{
    
}

internal void App_Render(PlatformTime* time, ScreenInfo info)
{
    i32 texIndex = g_cubeTextureIndex;
    MeshData* cube = COM_GetCubeMesh();
    
    // Reset scene
    g_worldScene.numObjects = 0;
    
    // Render Scene - orient camera
    RScene_Init(&g_worldScene, g_worldSceneItems, MAX_WORLD_SCENE_ITEMS,
		90, RENDER_PROJECTION_MODE_3D, 8);
    #if 1
    g_worldScene.cameraTransform.pos.z = 1;
    g_worldScene.cameraTransform.pos.y = 1;
    Transform_SetRotation(&g_worldScene.cameraTransform, -(45    * DEG2RAD), 0, 0);
    #endif

    // Add render object
    RendObj obj = {};
    //RendObj_SetAsMesh(&obj, *cube, { 1, 0, 0, 1 }, texIndex);
    RendObj_SetAsMesh(&obj, *cube, { 1, 1, 1, 1 }, texIndex);
    TRANSFORM_CREATE(modelTransform);
    //t.pos.z = -2;
    #if 1
	Transform_RotateY(&modelTransform, 45 * DEG2RAD);
    #endif
	
    #if 0 // Old stype
    // Build scene
    RScene_AddRenderItem(&g_worldScene, &modelTransform, &obj);

    // Render
    g_platform.RenderScene(&g_worldScene);
    #endif
    
    #if 1 // New style
    // Prepare command buffer
    const int maxCommands = 16;
    RenderCommand cmds[maxCommands];
    COM_ZeroMemory((u8*)cmds, sizeof(RenderCommand) * maxCommands);
    i32 nextCommandIndex = 0;

    // --- Push commands ---
    RenderCommand* cmd;

    // Projection
    cmd = &cmds[nextCommandIndex++];
    cmd->type = REND_CMD_TYPE_PROJECTION;
    COM_SetupDefault3DProjection(cmd->matrix.cells, info.aspectRatio);

    // Object view model transform
    cmd = &cmds[nextCommandIndex++];
    cmd->type = REND_CMD_TYPE_MODELVIEW;
    COM_SetupViewModelMatrix(
        &cmd->matrix, &g_worldScene.cameraTransform, &modelTransform);

    // Object draw
    cmd = &cmds[nextCommandIndex++];
    cmd->type = REND_CMD_TYPE_DRAW;
    cmd->drawItem.obj = obj;

    g_platform.SubmitRenderCommands(cmds, nextCommandIndex);
    #endif
}

internal u8 App_ParseCommandString(char* str, char** tokens, i32 numTokens)
{
    return 0;
}


/***************************************
* Export Windows DLL functions
***************************************/
#include <Windows.h>

extern "C"
AppInterface __declspec(dllexport) LinkToApp(AppPlatform platInterface)
{
    printf("APP: Library Built on %s at %s\n", __DATE__, __TIME__);
    g_platform = platInterface;
    g_isValid = true;

    AppInterface app;
    app.isValid = true;
    app.AppInit = App_Init;
    app.AppShutdown = App_Shutdown;
    app.AppRendererReloaded = App_RendererReloaded;
    //app.AppFixedUpdate = App_FixedFrame;
    app.AppInput = App_Input;
    app.AppUpdate = App_Update;
    app.AppParseCommandString = App_ParseCommandString;
	app.AppRender = App_Render;
    return app;
}

extern "C"
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    // TODO: Find out why this called seamingly at random whilst running
    // ANSWER: For each thread that is started dllMain is called
    printf("APP DLL Main\n");
	return 1;
}
