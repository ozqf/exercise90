# Exercise 90
A simple fps as a learning exercise in C, opengl and games/engines generally.

## Intended Features:

### High Concept
* Basic Doom/Quake level 3D engine capable of prototyping 3D games.
* Written from scratch and avoiding the use of external libraries/plugins where possible for maximum education and fun...yay.

### Basic Architecture
* Platform/Engine layer is the exe that handles basic input, update loop, rendering, file/network IO etc.
* Game is separated into a DLL which is sealed from the outside world. All platform specific functionality goes through the platform layer.
* Renderer is simple (hey I'm learning) and uses opengl.

### Future thoughts:
* Rewrite platform/engine layer for linux once engine is developed enough.

#### Engine
* Split the renderer off from the platform layer as another DLL, and use the platform layer simply as a kernal which intermediates between the game and specific services such as rendering or networking?
    * Like this idea, hot reloading the renderer will make it easier to iterate and debug!
* Upgrade to later version of opengl.
* Proper tree based scene graph with parent/child object transforms.

#### Game

## Random todo list

### Command Structure
Move the engine and game to using commands.
* Commands are stored in double buffers, swapped at the start of a frame.
* Commands will flow Platform -> App -> Game, with each level choosing what goes down to the level below.
* Similarly outputs are commands written to an output buffer.
* The game is 'playing back' commands it is given, either from single player, a server or a recorded demo file.

### Bugs or fundamental issues
* **Calculate View-Model matrix in renderer manually** - View model matrix is currently constructed by extracting euler angles from transform rotations then applying those angles to opengl's internal matrix. This mostly works but causes awful jittering when transforms come from the physics engine.
* Done with move to 3x3 rotation matrices in transforms *Calculation of Foward/Up Vectors is wrong (added 2018/4/2)* - This was hacked in someway to make movement work. Now needs to be unhacked, as it is preventing the casting of rays out of the camera.
* **Game requires asset files to launch (added 2018/4/2)** - Some very basic assets should be baked into the code for such early development work. Only thing that cannot be procedurally generated is the console char sheet. So that first.

### Game
* Done 2018/3/16 **Player movement** - Current player movement is messed up - rebuild.
* **Basic Arena Level** - Something to stand on and contain the player!
* **Entities**
    * Done 2018/3/30 *How will entities be stored? - Exact structure of entity system. Big structs for now with separate list of render objects built each frame?*
    * Done 2018/3/30 *Entity List - Until a better idea comes up, alloc a big array (4096 odd) of entity structs and use that.*
    *Copy to keep active entities packed to start of array?*
    * **Update loop**
* Done 2018/3/14 **Create Heap** - Create main memory pool.
* ~Collision Detection~ - Deferred to physics engine
    * **Ray vs plane** - For hitscan vs world.
    * Done 2018/4/2 ...in 2D yes, in 3D, probably *Ray vs AABB* - For hitscan vs actor.
    * **Actor vs World** - AABB vs plane or perhaps ray vs expanded plane
    * **Actor vs Actor** - For now just AABB vs AABB?
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
* **Command Buffer** - Change nature of inputs to processing of frame specific command buffers
    * **Record Command input** - Allow for playback of game commands.
* **Debug Console** - Console for viewing debug output and entering commands.
    * **Basic Text log** - A scrolling block of text.
    * **Frame-updated status text** - Displays live debugging information (eg framerate).
    * **Input box** - Type in and execute commands.
* **Split Renderer to DLL** - Allow live reloading of a renderer DLL for easier renderer development
    * Done 2018/4/8 *Texture load calls are now initiated by the game layer not the platform layer.*
    * **Texture Database** - App must keep track of textures it has loaded so they can be found by entities or rebound to the GPU.
    * **Rebindable Textures** - Allow platform to rebind loaded textures at any time, allowing a reloaded renderer

### Rendering
* Done 2018/3/18 *ZBuffer - Currently no hidden surface removal.*
* **Basic Text** - Just some means of rendering text for debug output and placeholder UI.
    * **Render from Char sheet** - Can double for sprite sheet rendering too.
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
* **Input binding is Engine-side** - Engine should pass input events to game and let game handle it to make input more modable/less platform specific.
    See command buffer stuff.
* Done 2018/3/15 *Test File Loading*
* Done 2018/3/14 *Alloc memory for game*
