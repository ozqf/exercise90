# Exercise 90
A simple fps as a learning exercise in C, opengl and games/engines generally.

## Random todo list

### Game
* Done 2018/3/17 **Player movement** - Current player movement is messed up - rebuild.
* **Basic Arena Level** - Something to stand on and contain the player!
* **Collision Detection**
    * **Ray vs plane** - For hitscan vs world.
    * **Ray vs AABB** - For hitscan vs actor.
    * **Actor vs World** - AABB vs plane or perhaps ray vs expanded plane
    * **Actor vs Actor** - For now just AABB vs AABB?
* Done 2018/3/15 **Create Heap** - Create main memory pool.
* **Entities**
    * **Entity List** - Until a better idea comes up, alloc a big array (4096 odd) of entity structs and use that.
    Copy to keep active entities packed to start of array.
    * **Update loop**

### Rendering
* **ZBuffer** - Currently no hidden surface removal.
* **Text**
    * **Basic Text** - Just some means of rendering text for debug output and placeholder UI
* **Textures**
    * Done 2018/3/16 **Loading Textures** - Need to load in bitmaps
    * Done 2018/3/17 **Transparent textures** - Required generally, but of special importance if using sprite objects ala Doom.
    * **Tiling Textures** - Making textures repeat over large world surfaces ala old BSP engines.
* **3D Meshes**
    * **More Primitives** - Very basic shapes that are hardcoded into primitives header file
        * **Cube**
        * **Sphere**
        * **Octahedron**
    * **Improved Billboards** - Billboards (quads that always face the camera) aren't quite right orientation wise.
* Done 2018/3/17 **Scale** - X/Y/Z Scale is in the Transform data type but is not used.

### Platform
* **Mouse Input**
    * **Confine Cursor**  - Mouse cursor is not confined to the game window
    * **Hide cursor** -  cursor is currently always shown. Require a game state switch to allow the cursor to be toggled on/off
    for UI stuff.
* **Input binding is Engine-side** - Engine should pass input events to game and let game handle it to make input more modable/less platform specific.
* Done 2018/3/16 **Test File Loading**
* Done 2018/3/15 **Alloc memory for game**
