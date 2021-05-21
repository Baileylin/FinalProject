# CS312 Final Project
This project is an extension of the ray tracer implemented in Lab3. There are two main features implemented in this project (textures and arbitrary light sources) and some additional features (defocus blur and motion blur).

## How to build

*Windows*

Open git bash to the directory containing this repository.

```
raytracer $ mkdir build
raytracer $ cd build
raytracer/build $ cmake -G "Visual Studio 16 2019" ..
raytracer/build $ start CS312-FinalProject.sln
```
To run the program, set "materials" as the starup project in Visual Studio and run it.

To recreate different images, change the variable "cases" in materials.cpp on line 95 to numbers between 0 and 6. 
## Textures
The program allows sphere and triangles to have the following implemented textures. 
### Implemented textures
1. Constant Texture
    The constant texture is used to hold a color. In lambertian material, the orignal albedo color is replaced by a texture so that lambertian material can have different textures.
2. Checker Texture
This texture is used to create a checker texture which allows two kinds of texture (or color) to appear in alternating pattern. 
3. Image Texture 
This texture allows am image to be mapped on sphere and triangle. 
### Sample Image
<img src='./images/basic_checker_texture.png'  width='550' />



## Arbitrary Light Sources
The program allows sphere, triangles, and plane to become a light source. In the project, one addtional material called "emit_light" was added. This material will not scatter light and could serve as a light source. 

### Sample Image
<img src='./images/light_sources.png'  width='550' />

## Other features
### Defocus Blur
### Motion Blur
## Image of Solar System
