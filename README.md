# EDAN35 - Computer Graphics Project

The EDAN35 Project is an open-ended computer graphics projects where we got to implement any CG algorithm of our choosing.
We implemented a landscape generator using perlin noise and marching cubes.

This project is based on the EDAN35 Lab, for more information, see [their github](https://github.com/LUGGPublic/CG_Labs).

This app was made as the final project for the EDAN35 High-Performance Computer Graphics course at [LTH, Lund University.](https://www.lth.se/english/)

---

![Example of landscape generated with project](/readme/generated-landscape-example.png)

## Landscape Generator
The landscape is based on a 3D grid of booleans, indicating if any place should be part of the terrain.
Using this boolean the Marching Cubes algorithm is used to generate a mesh.
This mesh can be sculpted in real time by the user. There are also some other options to modify the terrain, which can be found [here](#tools).

> ⚠️ This project uses boolean marching cubes, not the traditional interpolated version. A future version could improve on this by changing the grid to store floats instead of booleans. Currently some of the interpolation is implemented, but it is unfinished, mainly because the sculpting and grid do not support it yet.



## Tools
The project has several tools, both for debugging and interacting with the terrain. These tools can all be accessed in the "Scene Controls" ImGUI menu.
Changing these options will be reflected in the app immediately. 
![The Scene Controls menu displaying all tool options](/readme/scene-controls.png)

### Debugging
In order to debug the 3d grid, a point debugger is added.
This debugger renders the underlying 3d boolean grid as points in 3d space.
White indicates the voxel should be part of the terrain, and black indicates air.

Since it is hard to visualise an entire 3d grid using just dots, it is also possible to show only one X/Y/Z slice at a time, for which any direction and slice index can be chosen.

![An example of displaying a single slice in the points debugger](/readme/point-debugger-single-slice.png)

### Sculpting
Because the landscape is based on a grid of booleans, it is relatively simple to implement a sculpting feature.
In the app when pressing `Z/X` terrain will be added or removed where the camera is pointing.

This is done by creating a ray from the camera. The ray is moved the size of one voxel at a time, until the closest voxel to the ray is TRUE, which is where the ray adds or removes terrain.
This approach is efficient because the ray only has to compare to one value per move. The rays can also be visualised using the "Show Sculpting Rays" option in the debug menu.
