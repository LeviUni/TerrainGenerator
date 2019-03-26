# Terrain Generator
Terrain Generator build in Qt for the Computer Graphics 2019 course @RuG

---

### Overview
This program shows a procedurally generated island in the middle of a sea. It has a few special features:

 * Procedural terrain generator using different levels of noise
 * Regeneration of the terrain using the `regenerate` button.
 * Multiple textures on the terrain, mixed based on the height and slope of the terrain
 * Water that is reflective and waves
 * A very simple day/night cycle, including orange-y light at sunset and sunrise
 * Camera controls with Qt widgets to rotate and scale the camera around the scene

---

### Notes
Because of the complexity of this scene and of the rendering process, this program might not run at 60 fps on all computers. However, even with the most basic graphics configuration (e.g. Intel HD graphics), it should still work and give around 20-30 fps. With dedicated (modern) graphics cards, a nice 60 fps can quickly be reached. 

---
 
### Screenshots
To show off the features, here are some screenshots. Before each screenshot was taken, a new terrain was generated.
 
![A view from the side](/Screenshots/sideview.png?raw=true)
In this screenshot, it is clearly visible how the terrain reflects in the water, while this reflection is also affected by the waves. The lighting effects are also affected by the same wavey pattern. Because the angle with the water is very shallow, not much of the scene that is below the water is visible. This is due to the [Fresnel effect](https://www.scratchapixel.com/lessons/3d-basic-rendering/introduction-to-shading/reflection-refraction-fresnel).

![A view from the top](/Screenshots/topview.png?raw=true)
When looking from the top, much more of the world under water is visible, because the normals of the water are almost directly aligned with the view vectors. 

![Nighttime](/Screenshots/nighttime.png?raw=true)
During the night (so, when the light position is below 0), the sky becomes dark. Not much is visible now.

![Sunrise](/Screenshots/sunrise.png?raw=true)
When the 'sun' is setting or rising, it colors an orange color, which can be seen in the lighting in this screenshot. 

---

### Implementation

The scene is rendered three times. Two of these passes are rendered to [Framebuffers](https://www.khronos.org/opengl/wiki/Framebuffer_Object). The textures that these passes give are used in the water shader. It uses a dUdV map to distort the textures, and renders them to a flat quad. This means the water doesn't actually wave, it is just an illusion. Calculating real-time reflections of complex objects is only feasible if this is the reflection is on a plane.

The multiple textures are done by passing an array of materials to the shader. Based on the dot product of the triangle normal and the up vector, a slope is calculated. This is used in mixing between the grass/sand textures and the rock texture. Then, the sand and water are mixed based on a height. 

All materials in the terrain have a diffuse map, a normal map, and a specular map. This ensures nice lighting effects.

The day/night cycle is done pretty simply by changing the `glClearColor` and light color paramters based on the position of the light. 
