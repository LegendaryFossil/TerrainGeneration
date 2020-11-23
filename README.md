# Procedural Terrain Generation

Inspired by [Sebastian Lague's video series on how to develop a procedural landmass generator](https://youtu.be/wbpMiKiSKm8) in Unity I developed a terrain generator in C++. Frameworks/APIs include OpenGL (rendering), GLSL (shader programming) 
GLFW (window handling) and Dear ImGui (GUI-interaction). In addition to the procedural generation dynamic level-of-detail (LOD), terrain texturing, water simulation, water reflection 
and a lightning model supporting multiple lights have been implemented. Most of the features have one or several modifiable properties in the GUI to get different results (see **Usage** section). 
CMake is used as the build system, making it simple to modfiy and get a project up and running.

## How the generation works
A noise map is generated to be used as a height map in the generation of the terrain. Rather than generating all vertex data on CPU the program use **patches** as its primitive type. The map 
is divided in uniform patches based on a patch size, and every patch is represented by one vertex in its lower-left corner. The vertex data is sent to the vertex stream where it goes through several stages in the OpenGL pipeline:

**Vertex Shader**: The vertex shader makes no changes to the vertex data. Its only responsibility is to pass the vertex data gathered together to form a patch that is passed to the Tessellation Control Shader (**TCS**)

↓

**Tessellation Control Shader**: The **TCS** will operate on the vertices in the received patch, referred to as control points in this context. Two things are done at this stage:

- The **TCS** outputs a new patch of control points to the Tessellation Evaluation Shader (**TES**)

- The inner and outer tessellation levels are determined before being sent to the fixed-function Tessellation Primitive Generator (**TPG**). The **TPG** will operate on an abstract patch whose type is specified in the 
**TES**, and tessellate the patch based on the tessellation levels passed by the **TCS**. This generates a new set of vertices which are passed to the **TES** in the form of coordinates 
describing where the vertices lie in the patch. The coordinate have different meaning depending on the type of abstract patch used. This program use a **quad** as its type meaning the coordinates passed 
from **TPG** to **TES** are a pair of normalized coordinates. 

↓

**Tessellation Evaluation Shader**: At this stage the **TES** receives the patch from the **TCS** and the normalized coordinate for every generated vertex by the **TPG** (meaning the number of **TES** invocations are guaranteed to be a minimum of the number of generated vertices). The input patch, the normalized coordinates and the height map are then used together to calculate the vertex position. At this point all model and view transformations are applied, and the vertex is transfomed to clip space to be prepared for perspective division and clipping against the view frustum.

↓

**Fragment Shader**: The fragment shader run as the final step to color the terrain.

As most of the vertex data is generated in the OpenGL pipeline a huge amount of memory is offloaded from CPU which would otherwise have to generate and store all vertex data for the terrain and transfer it to the GPU.
Moreover, since the generation is done in the tessellation stages it makes it easy to implement a dynamic LOD by just altering the inner and outer tessellation levels in the **TCS** based on a chosen algorithm.

## Usage

### Navigation
The program use a first person camera to navigate in the scene. Keys are following:

**W**= Move forward

**S** = Move backward

**A** = Move left

**D** = Move right

**Q** = Move up

**E** = Move down

**U** = Show GUI

**Mouse movement** = Aim

Note that when the GUI is shown the camera navigation is turned off.

### GUI settings

**Terrain Settings -> Noise Map Settings**

The noise map settings allows modifying properties such as the number of octaves, persistance, lacunarity, noise map offset and more. All of these affect the generated noise map which in turns affect the final look of the terrain.

**Terrain Settings -> Terrain type settings**

The terrain type settings can be used to modify properties of a terrain type such as the color, the height at which the type starts, the blending between the type and the previous type and more. The colors and heights are used to generate a color map from the noise map. This color map is then used to sample the color in the fragment shader.

**Terrain Settings -> Use falloff map**

With the falloff map activated you get a terrain that is surrounded by water. This is done by generating a falloff map which is a map starting from white in the edges and slowly transitioning to black as it reaches the center. The noise map is then subtracted by this map to get a noise map that is black around the edges.

**Terrain Settings -> Pixels per triangle**

This setting controls the dynamic LOD. The LOD is determined by how many pixels each patch edge occupies. The setting controls how many pixels each triangle occupies in the tessellated edge. In other words, a small value means high tessellation levels thus a high-detail mesh and a large value means low tessellation levels thus a low-detailed mesh. This can be seen clearly in the Wireframe Mesh render mode

**Light Settings**
The lightning in the scene is based on the Blinn-Phong reflection model. Properties that can be modified here are the light and specular light color, the intensity of the specular light and the specular power (large values => small highlights, small values > large highlights)

Note that the position of the light sources can be moved with the same keyboard keys as for the camera. To do so select the  light source you want to move in the scene under **Scene Control**. The lights are rendered as meshes to make it easier to see where they are located.

**Water settings**
The water settings allow two properties to be changed: the water distortion speed and the distortion map used. Every distortion map has an associated normal map that is used for bump mapping the water as well as adding specular highlights to it.

A video below demonstrates some of these features

[![](http://img.youtube.com/vi/8Yf5n6LFYsU/0.jpg)](http://www.youtube.com/watch?v=8Yf5n6LFYsU "Procedural Terrain Generation")

### Future improvements

- Adding a day/night cycle.
- Soft edges between water and wet sand, at the moment the edge of the sand is very clear.
- Terrain collision detection when moving around with the camera.
- Fog system
- Details to the terrain textures. Although the persistance and lacunarity can be used to give more details it can still look quite flat from certain viewpoints. Using techniques like bump mapping (similar to the water) or parallax mapping can give more depth and details to the terrain.

### References

The noise, color and falloff map generators are based on Sebastian Lague's video series on procedural landmass generation in 
Unity. In addition the blending between terrain types is also based on his video series:

https://youtu.be/wbpMiKiSKm8

https://github.com/SebLague

FastNoise is used for the perlin noise generator when generating the noise map:

https://github.com/Auburn/FastNoise/tree/FastNoise-Legacy

The dynamic LOD and terrain generation through GPU tessellation is based on a chapter by Ant́onio Ramires Fernandes and Bruno Oliveira in OpenGL Insights (Chapter 10: GPU Tessellation: We Sstill Have a LOD of Terrain to Cover):

https://github.com/OpenGLInsights/OpenGLInsightsCode

The water reflection and simulation is based on ThinMatrix video series on water tutorial implemented with OpenGL and GLSL in Java:

https://www.youtube.com/watch?v=HusvGeEDU_U

The UI is handled by Dear ImGui developed by Omar Cornut and other contributors:

https://github.com/ocornut/imgui

Window handling is done through GLFW:

https://github.com/glfw/glfw

Credit also to Joey de Vries Learn OpenGL website which is a great resource for brushing up rusty skills in both basic and advanced topics in computer graphics:

https://learnopengl.com/
