# Exercise 90
A simple fps as a learning exercise in C, opengl and games/engines generally.

## Random todo list

### Game
* Done 2018/3/16 **Player movement** - Current player movement is messed up - rebuild.
* **Basic Arena Level** - Something to stand on and contain the player!
* **Collision Detection**
    * **Ray vs plane** - For hitscan vs world.
    * **Ray vs AABB** - For hitscan vs actor.
    * **Actor vs World** - AABB vs plane or perhaps ray vs expanded plane
    * **Actor vs Actor** - For now just AABB vs AABB?
* **Entities**
    * **How will entities be stored?** - Exact structure of entity system. Big structs for now with separate list
    of render objects built each frame?
    * **Entity List** - Until a better idea comes up, alloc a big array (4096 odd) of entity structs and use that.
    Copy to keep active entities packed to start of array.
    * **Update loop**
* Done 2018/3/14 **Create Heap** - Create main memory pool.

### Engine
* **Debug Console** - Console for viewing debug output and entering commands.
    * **Basic Text log** - A scrolling block of text.
    * **Frame-updated status text** - Displays live debugging information (eg framerate).
    * **Input box** - Type in and execute commands.

### Rendering
* **ZBuffer** - Currently no hidden surface removal.
* **Basic Text** - Just some means of rendering text for debug output and placeholder UI.
    * **Render from Char sheet** - Can double for sprite sheet rendering too.
        * **Basic UI System** - System in App to lay out basic UI elements on screen and modify them.
        * **String Struct** - Internal structure for storing fixed sized char arrays in a Heap.
        * Done 2018/3/18 *Render an asci char*
        * Done 2018/3/18 *Render a char array*
* **Textures**
    * **Tiling Textures** - Making textures repeat over large world surfaces ala old BSP engines.
    * Done 2018/3/15 *Loading Textures* - Need to load in bitmaps
    * Done 2018/3/16 *Transparent textures* - Required generally, but of special importance if using sprite objects ala Doom.
* **3D Meshes**
    * **More Primitives** - Very basic shapes that are hardcoded into primitives header file
        * **Sphere**
        * Done 2018/3/24 *Octahedron*
        * Done 2018/3/24 *Cube*
        * Done 2018/3/24 *Inverse Cube*
    * **Improved Billboards** - Billboards (quads that always face the camera) aren't quite right orientation wise.
* Done 2018/3/16 *Scale* - X/Y/Z Scale is in the Transform data type but is not used.
* Done 2018/3/24 *Projection* - Adjust Projection matrix to reduce 'near' value

### Platform
* **Mouse Input**
    * **Confine Cursor**  - Mouse cursor is not confined to the game window
    * **Hide cursor** -  cursor is currently always shown. Require a game state switch to allow the cursor to be toggled on/off
    for UI stuff.
* **Input binding is Engine-side** - Engine should pass input events to game and let game handle it to make input more modable/less platform specific.
* Done 2018/3/15 *Test File Loading*
* Done 2018/3/14 *Alloc memory for game*
