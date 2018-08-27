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

## Random todo list

### ECS complexity
Insanely fiddly steps to adding a new component:

Game Types
1. define component flag
2. Add the component struct
3. Add the component state struct
4. Add state var to master EntityState struct
5. Define component list
6. Add list var to GameState
7. Define Component base functions

Game State
8. declare array of components
9. assign array to GameState
10. Zero component memory on gamestate reset
11. Clear component on entity removal

ec_factory:
12. Add component state read call
13. add component state write call
14. Add apply state function 

### Command Structure
Done Move the engine and game to using commands.
* Done Commands are stored in double buffers, swapped at the start of a frame.
* Defunct - Commands will flow Platform -> App -> Game, with each level choosing what goes down to the level below.
* Done Similarly outputs are commands written to an output buffer.
* Done The game is 'playing back' commands it is given, either from single player, a server or a recorded demo file.

### Command Refactor - 2018/7
* **Refactor Entity Components** - Split entity components into their transferable server state and their local data/pointers etc.
* **Rebuild Entity Factory** - Rewrite entity factory to work entirely from a 'recipe'. Prefab entities are prebuilt/stored recipes. State 
* **Entity Updates in Stream** - variable sized command that can include only parts of an entity's state, for smaller updates.

### Bugs or fundamental issues
* **Calculate View-Model matrix in renderer manually** - View model matrix is currently constructed by extracting euler angles from transform rotations then applying those angles to opengl's internal matrix. This mostly works but causes awful jittering when transforms come from the physics engine.
* Done with move to 3x3 rotation matrices in transforms *Calculation of Foward/Up Vectors is wrong (added 2018/4/2)* - This was hacked in someway to make movement work. Now needs to be unhacked, as it is preventing the casting of rays out of the camera.
* **Game requires asset files to launch (added 2018/4/2)** - Some very basic assets should be baked into the code for such early development work. Only thing that cannot be procedurally generated is the console char sheet. So that first.

### Game
* **Gameplay**
    * **Enemy Attack Cycle** - pause, shoot, pause, continue
* Done 2018/3/16 *Player movement* - Current player movement is messed up - rebuild.
    * Done 2018/6/10 *Improved movement* - Movement code uses adapted version of Quake's movement code.
* Done 2018/5 *Basic Arena Level* - Something to stand on and contain the player!
* **Entities**
    **Reduce Boilerplate** - lot of fiddly boilerplate related to adding components. Reduce this.
    * Done 2018/3/30 *How will entities be stored? - Exact structure of entity system. Big structs for now with separate list of render objects built each frame?*
    * Done 2018/3/30 *Entity List - Until a better idea comes up, alloc a big array (4096 odd) of entity structs and use that.*
    *Copy to keep active entities packed to start of array?* - Why?
    * **Update loop**
* Done 2018/3/14 **Create Heap** - Create main memory pool.
    * **Split Heaps** - Two or more heaps for application and session memory
        Short lived: Specific session related: Entities/game state
        
* ~Collision Detection~ - Deferred to physics engine
    * **Ray vs plane** - For hitscan vs world.
    * Done 2018/4/2 ...in 2D yes, in 3D, probably *Ray vs AABB* - For hitscan vs actor.
    * Defunct *Actor vs World* - AABB vs plane or perhaps ray vs expanded plane
    * Defunct *Actor vs Actor* - For now just AABB vs AABB?
    * Done 2018/3/30 *Binary AABB vs AABB collision detection*

### Physics Wrapper
* **Convert Physics inputs to command buffer** - Buffer actions in the physics engine to be processed before stepping.
    * **Use command events to store factory settings** - 
* **Querying (Rays and spheres/cube volumes)** - for object searchs, explosions and projectiles
* **Overlap detection (Rigidbodies, static and triggers)** - Kinda need this to make a game out of
    * Done 2018/5/5 *Basic detection of overlap pairs* - Inefficient and messy
    * **Overlap detection (triggers)**
* **Teleportation** - directly setting object positions
* **One way collisions** - Creating planes/very thin boxes that can only be moved through in one direction (forcefields)
* **'Static' moving shapes** - can shapes with zero mass be repositioned? If so can they be used as blocking
    volumes ala old Box2D flash stuff?
* **Kinematic** - examine kinematic shapes and how they might be used for player movement?
* Done 2018/5/5 *Lock Rotation* - Create shapes that only move and cannot rotate.
* Done 2018/4/5 *Bullet Physics Library* - Attach the bullet physics library via a wrapper

### Engine
* Done 2018/5/4 *Command Buffer* - Change nature of inputs to processing of frame specific command buffers
    * Done *Record Command input* - Allow for playback of game commands.
* *Debug Console* - Console for viewing debug output and entering commands.
    * Done 2018/6 *Windows console* - Use printf via a windows command prompt for debugging output. 
    * Defunct *Basic Text log* - A scrolling block of text.
    * Defunct *Frame-updated status text* - Displays live debugging information (eg framerate).
    * Done 2018/6 *Input box* - Type in and execute commands.
* Done 2018/4/9 *Split Renderer to DLL* - Allow live reloading of a renderer DLL for easier renderer development
    * Done 2018/4/8 *Texture load calls are now initiated by the game layer not the platform layer.*
    * Done 2018/6/11*Texture Database* - App must keep track of textures it has loaded so they can be found by entities or rebound to the GPU.
    * Done 2018/4/8 *Rebindable Textures* - Allow platform to rebind loaded textures at any time, allowing a reloaded renderer

### Rendering
* **Convert Renderer to Command Queue**
    * **Renderer Commands**
        * **Draw Object** - Implements current drawing system
        * **Settings Data** - Set the current camera and lighting settings
* Done 2018/3/18 *ZBuffer - Currently no hidden surface removal.*
* Done *Basic Text* - Just some means of rendering text for debug output and placeholder UI.
    * Done *Render from Char sheet* - Can double for sprite sheet rendering too.
        * **Basic UI System** - System in App to lay out basic UI elements on screen and modify them.
        * **String Struct** - Internal structure for storing fixed sized char arrays in a Heap.
        * Done 2018/3/18 *Render an asci char*
        * Done 2018/3/18 *Render a char array*
* **Textures**
    * **Tiling Textures** - Making textures repeat over large world surfaces ala old BSP engines.
    * Done 2018/3/15 *Loading Textures - Need to load in bitmaps*
    * Done 2018/3/16 *Transparent textures - Required generally, but of special importance if using sprite objects ala Doom.*
* **3D Meshes**
    * **More Primitives** - Very basic shapes that are hardcoded into primitives header file
        * **Sphere**
        * Done 2018/3/24 *Octahedron*
        * Done 2018/3/24 *Cube*
        * Done 2018/3/24 *Inverse Cube*
    * **Improved Billboards** - Billboards (quads that always face the camera) aren't quite right orientation wise.
* Done 2018/3/16 *Scale - X/Y/Z Scale is in the Transform data type but is not used.*
* Done 2018/3/24 *Projection - Adjust Projection matrix to reduce 'near' value*

### Platform
* Done 2018/3/24 *Mouse Input*
    * Done 2018/3/24 *Confine Cursor  - Mouse cursor is not confined to the game window*
    * Done 2018/3/24 *Hide cursor -  cursor is currently always shown. Require a game state switch to allow the cursor to be toggled on/off*
    for UI stuff.
* Done 2018/5 *Input binding is Engine-side* - Engine should pass input events to game and let game handle it to make input more modable/less platform specific.
    See command buffer stuff.
* Done 2018/3/15 *Test File Loading*
* Done 2018/3/14 *Alloc memory for game*
