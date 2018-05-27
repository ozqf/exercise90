#if 0
#ifndef SDL_MAIN_CPP
#define SDL_MAIN_CPP

#define GLEW_NO_GLU

// Internal
#include "../../interface/platform_interface.h"
#include "../../interface/app_interface.h"
#include "../../interface/app_stub.h"

// External
#include "../../../lib/SDL2-2.0.7/include/SDL.h"
#include "../../../lib/SDL2-2.0.7/include/SDL_thread.h"
#include <windows.h>
//#include "../../../lib/glew-2.1.0/include/gl/glew.h"
#include <stdio.h>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int metre2pix = 16;

//The window we'll be rendering to
static SDL_Window *window = NULL;
//The surface contained by the window
static SDL_Surface *screenSurface = NULL;
static bool running = true;

char* gameModulePath = "base/gamex86.dll";

AppInterface app;
void* gameModule;
PlatformInterface platInterface;

SDL_Surface* devSpriteTest = NULL;

void LinkToGame()
{
    gameModule = SDL_LoadObject(gameModulePath);
    bool success = false;
    if (gameModule != NULL)
    {
        printf("Located %s\n", gameModulePath);
        Func_LinkToApp* linkToApp = (Func_LinkToApp *)SDL_LoadFunction(gameModule, "LinkToApp");
        if (linkToApp != NULL)
        {
            app = linkToApp(platInterface);
            success = true;
        }
        else
        {
            printf("Failed to locate LinkToApp\n");
        }
    }
    else
    {
        printf("Failed to load %s\n", gameModulePath);
    }
    if (success == false)
    {
        app = GetAppInterfaceStub(platInterface);
    }
}

void UnloadGameDLL()
{
    printf("*** Reloading Game DLL ***\n");
    app.AppShutdown();
    app = GetAppInterfaceStub(platInterface);
}

void PlatformShutdown()
{
    printf("*** Shutdown ***\n");
    app.AppShutdown();
    // Free Dev sprite
    SDL_FreeSurface(devSpriteTest);

    //Destroy window
    SDL_DestroyWindow(window);

    //Quit SDL subsystems
    SDL_Quit();
}

f32 PlatformGetViewPortMinX() { return 0 / metre2pix; }
f32 PlatformGetViewPortMinY() { return 0 / metre2pix; }
f32 PlatformGetViewPortMaxX() { return SCREEN_WIDTH / metre2pix; }
f32 PlatformGetViewPortMaxY() { return SCREEN_HEIGHT / metre2pix; }

/**
Input
*/
ClientTick ClientTick = {};

void HandleInput(SDL_Event *e)
{
    if (e->type == SDL_KEYDOWN)
    {
        switch (e->key.keysym.sym)
        {
        case SDLK_w:
            clientTick.up = 1;
            break;

        case SDLK_s:
            clientTick.down = 1;
            break;

        case SDLK_a:
            clientTick.left = 1;
            break;

        case SDLK_d:
            clientTick.right = 1;
            break;
        }
    }
    else if (e->type == SDL_KEYUP)
    {
        switch (e->key.keysym.sym)
        {
        case SDLK_w:
            clientTick.up = 0;
            break;

        case SDLK_s:
            clientTick.down = 0;
            break;

        case SDLK_a:
            clientTick.left = 0;
            break;

        case SDLK_d:
            clientTick.right = 0;
            break;

        case SDLK_ESCAPE:
            running = false;
            break;

        case SDLK_F1:
            UnloadGameDLL();
            break;

        case SDLK_F2:
            LinkToGame();
            break;
        }
    }
}

/**
Render
*/
void PlatformClearScreen()
{
    SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0x00));
}

inline void RenderBlitBlock(BlitBlock *block)
{
    SDL_Rect fillRect;
    fillRect.w = (i32)((block->halfWidth * 2) * (f32)metre2pix);
    fillRect.h = (i32)((block->halfHeight * 2) * (f32)metre2pix);
    fillRect.x = (i32)(block->centre[0] * (f32)metre2pix);
    fillRect.y = (i32)(block->centre[1] * (f32)metre2pix);
    fillRect.x -= (fillRect.w/2);
    fillRect.y -= (fillRect.h/2);
    SDL_FillRect(screenSurface, &fillRect, SDL_MapRGB(screenSurface->format, block->red, block->green, block->blue));
}

inline void RenderBlitImage(BlitImage* image, SDL_Surface* bitmap)
{
    SDL_Rect imageRect;
    imageRect.w = bitmap->w;
    imageRect.h = bitmap->h;
    imageRect.x = (i32)((image->centre[0] * (f32)metre2pix) - (imageRect.w / 2));
    imageRect.y = (i32)((image->centre[1] * (f32)metre2pix) - (imageRect.h / 2));
    SDL_BlitSurface(devSpriteTest, NULL, screenSurface, &imageRect);
}

void PlatformRenderBlitItems(BlitItem* items, i32 numItems)
{
    for (int i = 0; i < numItems; ++i)
    {
        switch (items->type)
        {
            case BLIT_ITEM_BLOCK:
            {
                BlitBlock block = items->content.block;
                RenderBlitBlock(&block);
            }
            break;

            case BLIT_ITEM_IMAGE:
            {
                BlitImage image = items->content.image;
                RenderBlitImage(&image, devSpriteTest);
            }
            break;
        }
        items++;
    }
}

void PlatformFillRect(i32 x, i32 y, i32 width, i32 height, uChar red, uChar green, uChar blue)
{
    SDL_Rect fillRect;
    fillRect.x = x;
    fillRect.y = y;
    fillRect.w = width;
    fillRect.h = height;
    SDL_FillRect(screenSurface, &fillRect, SDL_MapRGB(screenSurface->format, red, green, blue));
}

/**
Multithreading test
*/

int threadValue = 0;

int ThreadFunction(void *data);

int ThreadFunction(void *data)
{
    while (running)
    {
        // print incoming data
        threadValue += (int)data;
        printf("Running thread add %d value = %d\n", (int)data, threadValue);
    }

    return 0;
}

bool runningDumbThreadTest = false;
void StartDumbThreadTest()
{
    runningDumbThreadTest = true;
    // int data = 1;
    // SDL_Thread* threadID = SDL_CreateThread(ThreadFunction, "Lazy Thread", (void*)data);

    // int data2 = -1;
    // SDL_Thread* threadID2 = SDL_CreateThread(ThreadFunction, "Lazy Thread 2", (void*)data2);
}

void AwaitDumbThreadTest()
{
    if (runningDumbThreadTest == false) { return; }
    runningDumbThreadTest = false;
    
    // SDL_WaitThread(threadID, NULL);
    // SDL_WaitThread(threadID2, NULL);
}

// Prepare function table for game DLL
void InitPlatformInterface()
{
    platInterface.PlatformGetViewPortMinX = PlatformGetViewPortMinX;
    platInterface.PlatformGetViewPortMinY = PlatformGetViewPortMinY;
    platInterface.PlatformGetViewPortMaxX = PlatformGetViewPortMaxX;
    platInterface.PlatformGetViewPortMaxY = PlatformGetViewPortMaxY;
    platInterface.PlatformClearScreen = PlatformClearScreen;
    platInterface.PlatformRenderBlitItems = PlatformRenderBlitItems;
}

int main2(int argc, char *args[])
{
    printf("Platform layer built on %s at %s\n", __DATE__, __TIME__);
    app.isvalid = false;
    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }
    else
    {
        printf("SDL initialised\n");
        //Create window
        window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (window == NULL)
        {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
            return 1;
        }
        else
        {
            devSpriteTest = SDL_LoadBMP("DEV_SpriteTest.bmp");
            if (devSpriteTest == NULL)
            {
                printf("Failed to load DEV_SpriteTest.bmp");
                running = false;
                return 1;
            }

            // Init external interface
            InitPlatformInterface();
            // Connect to App
            LinkToGame();

            //Get window surface
            screenSurface = SDL_GetWindowSurface(window);

            SDL_Event e;

            // init timing
            GameTime time = {};
            time.fixedDeltaTime = 0.02f;
            u64 now = SDL_GetPerformanceFrequency();
            u64 last = 0;
            f64 fixedFrameAccumulator = 0;

            app.AppInit();

            while (running)
            {
                u32 mButtonFlags = SDL_GetMouseState(&clientTick.mouse[0], &clientTick.mouse[1]);
                time.ticks = SDL_GetTicks();

                last = now;
                now = SDL_GetPerformanceCounter();
                u64 diff = now - last;
                time.deltaTime = (f32)((now - last) * 1000.0f / SDL_GetPerformanceFrequency());
                time.deltaTime *= 0.001f;
                fixedFrameAccumulator += time.deltaTime;

                if (fixedFrameAccumulator >= time.fixedDeltaTime)
                {
                    fixedFrameAccumulator = 0;
                    // Fixed game tick
                    app.AppFixedUpdate(&time, &ClientTick);
                    time.fixedFrameNumber++;
                }

                // Input
                while (SDL_PollEvent(&e) != 0)
                {
                    if (e.type == SDL_QUIT)
                    {
                        running = false;
                    }
                    else if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)
                    {
                        HandleInput(&e);
                    }
                }
                app.AppUpdate(&time, &ClientTick);
                PlatformFillRect(clientTick.mouse[0] - 4, clientTick.mouse[1] - 4, 8, 8, 0xFF, 0x00, 0x00);
                SDL_UpdateWindowSurface(window);

                time.frameNumber++;

                // End of loop
            }

            AwaitDumbThreadTest();

            PlatformShutdown();
        }
        return 0;
    }
}

/* A simple function that prints a message, the error code returned by SDL,
 * and quits the application */
void sdldie(const char *msg)
{
    printf("%s: %s\n", msg, SDL_GetError());
    SDL_Quit();
    exit(1);
}


void checkSDLError(int line = -1)
{
#ifndef NDEBUG
	const char *error = SDL_GetError();
	if (*error != '\0')
	{
		printf("SDL Error: %s\n", error);
		if (line != -1)
			printf(" + line: %i\n", line);
		SDL_ClearError();
	}
#endif
}

#define PROGRAM_NAME "Tutorial1"

int main(int argc, char *args[])
{
    printf("Start");

    SDL_Window *mainwindow; /* Our window handle */
    SDL_GLContext maincontext; /* Our opengl context handle */

    if (SDL_Init(SDL_INIT_VIDEO) < 0) /* Initialize SDL's Video subsystem */
        sdldie("Unable to initialize SDL"); /* Or die on error */

    /* Request opengl 3.2 context.
     * SDL doesn't have the ability to choose which profile at this time of writing,
     * but it should default to the core profile */
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    /* Turn on double buffering with a 24bit Z buffer.
     * You may need to change this to 16 or 32 for your system */
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    /* Create our window centered at 512x512 resolution */
    mainwindow = SDL_CreateWindow(PROGRAM_NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        512, 512, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (!mainwindow) /* Die if creation failed */
        sdldie("Unable to create window");

    checkSDLError(__LINE__);

    /* Create our opengl context and attach it to our window */
    maincontext = SDL_GL_CreateContext(mainwindow);
    checkSDLError(__LINE__);


    /* This makes our buffer swap syncronized with the monitor's vertical refresh */
    SDL_GL_SetSwapInterval(1);

    /* Clear our buffer with a red background */
    glClearColor ( 1.0, 0.0, 0.0, 1.0 );
    glClear ( GL_COLOR_BUFFER_BIT );
    /* Swap our back buffer to the front */
    SDL_GL_SwapWindow(mainwindow);
    /* Wait 2 seconds */
    SDL_Delay(2000);

    /* Same as above, but green */
    glClearColor ( 0.0, 1.0, 0.0, 1.0 );
    glClear ( GL_COLOR_BUFFER_BIT );
    SDL_GL_SwapWindow(mainwindow);
    SDL_Delay(2000);

    /* Same as above, but blue */
    glClearColor ( 0.0, 0.0, 1.0, 1.0 );
    glClear ( GL_COLOR_BUFFER_BIT );
    SDL_GL_SwapWindow(mainwindow);
    SDL_Delay(2000);

    /* Delete our opengl context, destroy our window, and shutdown SDL */
    SDL_GL_DeleteContext(maincontext);
    SDL_DestroyWindow(mainwindow);
    SDL_Quit();

    return 0;
}

#endif
#endif