# GLSScene
3D graphical engine using openGL and openCL interoperability written in C++

<img src="https://upload.wikimedia.org/wikipedia/commons/thumb/1/18/ISO_C%2B%2B_Logo.svg/612px-ISO_C%2B%2B_Logo.svg.png?20170928190710" alt="openCL-logo" width="90"/>
<img src="https://www.khronos.org/assets/images/api_logos/opengl.svg" alt="openGL-logo" width="200"/>
<img src="https://www.khronos.org/assets/images/api_logos/opencl.svg" alt="openCL-logo" width="200"/>

### external libraries

**stb_image** (https://github.com/nothings/stb.git): loading images assets

**assimp** (https://github.com/assimp/assimp): loading scenes assets like 3d models and animations

**glfw** (https://github.com/glfw/glfw): window & input framework

**nanogui** (https://github.com/wjakob/nanogui.git): a GUI library to use with openGL that creates interfaces (such as text, button, textfield, etc...) on the screen

**nlohmannjson** (https://github.com/nlohmann/json): parsing of json files

> Note: glfw, nanogui and nlohmannjson are only used to show scene examples. The core graphic engine is usable with any other window framework like SDL or SFML

## Presentation

![shadow-scene](preview/shadow-scene.gif)

This project is used as a playground to try out things that I learn with openGL. I'm trying to make everything as inter compatible components.

The structure of a scene is a bit inspired from the gltf file format organization.
* Everything in the scene is a node that has a location, scale and rotation.
* Nodes are organized as tree graph. They have a parent node and a list of children nodes.
* The transformation (translation, scale and rotation) of a parent node will recursively apply to all childrens.
* A node can have a list of renderable components like a mesh, particle system or skinned mesh.

The engine is independent from the window management. It is only used to render a frame. The render loop is a behavior that should be implemented from the window management system.

A scene is composed of node objects from a root class `GLS::Node` that have a transformation matrix and an array of child nodes of the same type.
It's a recusive tree organisation that can group nodes together.
When modifying a transformation matrix, it will have an effect on the rendering, but also on the child node that will move along the parent.
All of those nodes also contains properties like a set of renderables objects `GLS::IRenderable`. It's an interface class that is subclassed by anything that can be drawn on screen. Like 3d meshes, particle system, skybox or post-processing effects.

### Casting shadows

![shadow-scene-plus](preview/shadow-scene-plus.gif)

In this scene there are multiple elements that are drawn on the same image to test the compatibility of drawing between different objects:
* Some static meshes like the white plane and a textured sphere
* One textured cube that rotates with time using a rotation matrix
* Ten thousands of small cubes with different translations, scales and rotations revolving around a pivot point. The rendering is optimized using instanciated mesh
* A particle system rendering thousands of particles floating around
* A spotlight moving and casting shadow for all elements
* An outliner effect for the spotlight using custom stencil and post rendering

To cast a shadow, elements have two ways of rendering. One that behaves using multiple elements in a scene like lights, texture or camera orientation. And another to render in a depth buffer to have a much quicker render so a light caster can render a scene quickly.

### Loading external assets

![skybox-scene](preview/skybox-scene.gif)

We can import external 3d scene from popular formats like fbx, dae or gltf. It's mostly done using Assimp to parse a scene from a file, then the result is maped to the coresponding engine class.
Images are also imported but using the famous `stb_image` repo.

This previous scene contains a skybox and a static mesh loaded from external file.

The skybox is rendered using a sampler cube on a texture cube map. The render is a basic cube but unlike every other elements of a scene, it ignores the camera translation and scale but only uses the rotation to give the background effect and is overwritten by any other rendered object.

### Particles system

![particle-scene](preview/particle-scene.gif)

<img src="https://www.khronos.org/assets/images/api_logos/opencl.svg" alt="openCL-logo" width="100"/> <img src="preview/fusion.png" alt="fusion" width="120"/> <img src="https://www.khronos.org/assets/images/api_logos/opengl.svg" alt="openGL-logo" width="100"/>

Another project from school was to lean OpenCL to create particle systems.
OpenCL is used to do parallel computation for custom shaders (unrelated to rendering). It comes really handy when creating things like particle systems. We could have one million particles and make OpenCL update all of their positions *blazingly* fast. Then with a small trick, we can make an interoperability with buffers between openCL and openGL.

I therefore created a small module in the engine to manage openCL functionalities. One of those functionalities is to create a buffer shared by OpenCL and OpenGL.

So I merged this project with my engine, turned the particle system into a `GLS::IRenderable` and apply my node transformation to all particles when rendering, then the integration was complete.

Those are rendered using a cloud of dots and updated with an openCL kernel applied to each of the particles.

The power of the GPU parralelization allows it to render and update millions of particles in real time with a correct 60 fps.

A particle kernel has two functions. One to initialize the particles so we can begin with a desired shape or color. And the other to update particles data with a given delta time.

We can also apply a texture to the particles. This will make the rendering pipeline use a geometry shader to turn a dot into a square.

![particle-scene](preview/particle-scene-plus.gif)

### Skinned mesh

The engine can render skinned meshes with skeletal animation and blend multiple animations inside single skeletal mesh.

The skeleton has different articulations which can be moved and rotated using manual or pre defined animation with custom speed. We can cover the skeleton with the skin which follows the skeleton bones.

![skinned-scene](preview/skinned-scene.gif)

### Voxel engine

One of the goal of this project was to play with procedural generation in a voxel world like minecraft.

The code of the procedural generation is contained in an openCL kernel so we can generate chunks of the world in parralel. As a small chunk of 128 blocks width has a size of two millions blocks (128^3 = 2 097 152), a `for` loop on the CPU would be pretty heavy.

Here are some presentations of the generation algorithm from `assets/voxelProceduralGeneratorSources/default.cl`

![voxel-scene](preview/voxel-scene.gif)
![voxel-caves](preview/VoxelWorld-caves.png)
![voxel-forest](preview/VoxelWorld-forest.png)
![voxel-ice](preview/VoxelWorld-ice.png)
![voxel-mountain](preview/VoxelWorld-mountain.png)

Another project from school was to make a voxel engine that render in real-time. I experimented with geometry shader that could generate just the right meshes during the rendering so we could simply send a 3d matrix of blocks to the GPU and the voxel will draw at each frames.
That was a *wrong good idea*. The rendering took just too long, and for a single chunck of 128x128x128 blocks, the framerate dropped to 20 fps.

A better solution was to bake the chuck into a mesh and render it. The downside of it is that we have to rebuild the mesh when one block changes. It's still better than rebuilding the mesh in the geometry shader at each frames. But to make it less impactful, the chucks were split into smaller 16x16x16 blocks.

### Postprocess effects

After the image is rendered to the framebuffer, we can apply a postprocess effect.

Some examples are in assets/postProcessEffects

```
./game -scene shadow -diffuse assets/textures/brickwall.jpg -normal assets/textures/brickwall_normal.jpg -effect assets/postProcessEffects/*effect*.glsl
```

> Averaging the colors to a greyscale to make tthe image black and white
![postprocess-grayscale](preview/postprocess-grayscale.png)

> Procedurally generated fog above the image
![postprocess-nightyfog](preview/postprocess-nightyfog.png)

> Using truncated colors and color edge detections to make a drawing effect
![postprocess-drawing](preview/postprocess-drawing.png)

> As the position of the camera is sent to the post process shader, we can imagine many thing even unrelated to our scene. Like drawing a 3D fractal calculated with a ray marching algorithm above our scene.
![postprocess-raymarch](preview/postprocess-raymarch.png)

The possibilities could be greatly improved if I implemented a multi buffered rendering so the shader could also compute effect using depth, normal or world position along with the color.

## Conclusion

It was really fun to work on this project and discover the possibilities and features of the GPU. I started this project before learning about Rust and Vulkan that were just emerging. Also I did not have any global knowledge about game development so I figured out a bit late that I could take a totally different approach. A better separation between the framework and the engine, an entity-component sytem for nodes and renderable things and many other things that can be better engineered.

I won't fix those, I think I'm done with this project and with openGL as the future is written with Vulkan and I'm also starting to love using fully featured game engine like UE5.
