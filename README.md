# Exercise 90
A simple fps as a learning exercise in C, opengl and games/engines generally.

## Build instructions

Builds as several different projects via Unity builds:
* **Platform (EXE):** win32_main.cpp
    * Links to user32.lib opengl32.lib Gdi32.lib
* **Renderer (DLL):** win32_gl_module.cpp
    * Links to user32.lib opengl32.lib Gdi32.lib
* **Sound (DLL):** win32_snd_module.cpp
    * Links to fmod_vc.lib fmodstudio_vc.lib
* **Applcation (DLL):** app_module.cpp and ZPhysics.cpp
    * Links to ZBulletPhysicsWrapper.lib

Requires Visual C++ command line Build Tools for Visual Studio (2015 or 2017 used).
* Open cmd.exe in exercise90\build
* Initialise Visual Studio tools with either vsvars2015.bat or vsvars2017.bat
* Run buildall.bat to build everything or buildwin32, buildrenderer, buildsound and buildgame to build individually.
* Download the asset files from here: https://www.dropbox.com/sh/65xmjtpvtnng3hm/AAD5mSgHuvS0DlsAj46U9XuGa?dl=0
* Extract the assets into the directory with the executable.

Final structure:
* exercise90.exe
* fmod.dll
* fmodstudio.dll
* win32gl.dll
* win32sound.dll
* base\gamex86.dll
* base\data01.dat

## Libraries

### Bullet Physics

[https://pybullet.org]
Link to the game DLL via the 'physics wrapper' which abstracts access to the physics engine

### FMOD

[https://www.fmod.com/]
Sound library. Linked to win32sound.dll.

### zlib
[https://zlib.net/]
Linked as a .lib to the platform layer for compressing/decompressing files in .dat files

## Reading/Resources
* [docs.gl: GL API 'Can use'](http://docs.gl/)
    * Shader related functions: glBindProgram, glCompileShader, glLinkProgram

## Intended Features:

### High Concept
* Basic Doom/Quake level 3D engine capable of prototyping 3D games.
* All external libraries are abstracted to keep the core game code clean of anything third party.

### Basic Architecture
* Platform/Engine layer is the exe that handles basic input, update loop, rendering, file/network IO etc and runs DLLs.
* Game is separated into a DLL which is sealed from the outside world. All platform specific functionality goes through the platform layer.
* Renderer and Sound are also DLLs but also contain platform specific code - should do something about this eventually.
* All DLLs are automatically 'hot reloaded' if the platform detects the file has been modified.
* Assets will be stored together in ZIP files.

#### Renderer
* Very basic fixed function. Naively draws a 'scene' object assembled by the game (or the app for menus and platform for debug input). Need to convert to a command buffer like everything else.
* Currently has no lighting, blending, vertex buffers... or *any* occulusion whatsoever.

### Future thoughts:
* Rewrite platform/engine layer for linux once engine is developed enough.

#### Engine
* Done - Split the renderer off from the platform layer as another DLL, and use the platform layer simply as a kernal which intermediates between the game and specific services such as rendering or networking?
    * Done - Like this idea, hot reloading the renderer will make it easier to iterate and debug!
* Upgrade to later version of opengl.
    * Shaders!
* Proper tree based scene graph with parent/child object transforms.

