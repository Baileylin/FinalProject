# raytracer

Implements a raytracer based on the book [_Ray Tracing in One Weekend_](https://raytracing.github.io/books/RayTracingInOneWeekend.html). 

## Basic   
<img src='./images/basic.png'  width='550' />

## Materials
<img src='./images/materials.png'  width='550' />

## Unique Feature 
Implements changing camera position with lookat and lookup vectors. (Change cam1 in line 192 in basic.cpp to cam2 to recreate this image.)     

<img src='./images/basic_with_changed_camera_position.png'  width='550' />

## Unique Image  
1. Background color changed
2. Used sphere, triangle, and plane
3. Created a tetrahedron
4. Created a snowflake-pattern of spheres   

<img src='./images/unique_image.png'  width='550' />
(this unique image is implemented in basic.cpp)



## How to build

*Windows*

Open git bash to the directory containing this repository.

```
raytracer $ mkdir build
raytracer $ cd build
raytracer/build $ cmake -G "Visual Studio 16 2019" ..
raytracer/build $ start CS312-Raytracer.sln
```

Your solution file should contain multiple projects, such as `gradient` and `normals`.
To run from the git bash command shell, 

```
raytracer/build $ ../bin/Debug/gradient
raytracer/build $ ../bin/Debug/normals
```

*macOS*

Open terminal to the directory containing this repository.

```
raytracer $ mkdir build
raytracer $ cd build
raytracer/build $ cmake ..
raytracer/build $ make
```

To run each program from build, you would type

```
raytracer/build $ ../bin/gradient
raytracer/build $ ../bin/normals
```

