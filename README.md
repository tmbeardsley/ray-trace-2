# 3D ray tracer

<img src="https://github.com/tmbeardsley/ray-trace/blob/chapter14/image23_png.png" width="400px">

## 1. Description
This repository follows the 3D ray tracing guide: "<a href="https://raytracing.github.io/books/RayTracingInOneWeekend.html">Ray Tracing in One Weekend</a>" by Peter Shirley, Trevor David Black and Steve Hollasch.
<br><br>
In addition to the code provided in the book, I have added extra comments and split progress up into chapters (multiple branches). 
There is also a "maths_scribbles" folder that derives some of the mathematical equations that were only stated in the book, as well as some of the more important diagrams.

## 2. Compiling
CMakeLists.txt specifies the required commands for CMake to create (and run) Makefiles, which create a 'build' directory and compile the program code into an executable calles: inOneWeekend..<br><br>
From the top directory, run: <br><br>
<b>cmake -B build -DCMAKE_BUILD_TYPE=Release</b><br>
<b>cmake --build build</b>

## 3. Running the program
After compilation the executable file, inOneWeekend, resides in the 'build' directory. 
From the top level of the directory tree, the output of the program is piped to an image file via: <br><br>
<b>./build/inOneWeekend > image.ppm</b>

## 4. Program parameters
At present, the parameters are specified in the main.cc file. Admittedly, this is not ideal (since the program must be recompiled after a parameter change) and will be addressed in a future update. 

### 4a. Camera properties
In the following table, "cam" is an instance of the camera class. It specifies how the "world" is captured by the camera.

| Parameter | Type | Description |
| :---: | :---: | --- |
| <em>cam.aspect_ratio</em> | Double | Ratio of image width to image height |
| <em>cam.image_width</em> | Integer | Width of rendered image in pixels |
| <em>cam.samples_per_pixel</em> | Integer | Number of rays fired through random positions in each pixel's area |
| <em>cam.max_depth</em> | Integer | Maximum number of ray bounces into scene (if exceeded, ray contributes no light. I.e., color(0,0,0) returned) |
| <em>cam.vfov</em> | Double | Vertical view angle (field of view). Used with cam.focus_dist to specify viewport height |
| <em>cam.lookfrom</em> | point3 (vec3) | 3D point in world-space that the camera is looking from |
| <em>cam.lookat</em> | point3 (vec3) | 3D point in world-space that the camera is looking at |
| <em>cam.vup</em> | vec3 | 3D vector specifying the camera-relative "up" direction |
| <em>cam.defocus_angle</em> | Double | Variation angle of rays through each pixel to specify depth of field |
| <em>cam.focus_dist</em> | Double | Distance from camera lookfrom point to plane of perfect focus |

### 4b. World space
The "world" is set up in main.cc. It specifies the size, location, and material applied to a series of spheres in 3D space. 
At present, this code is limited to spheres for simplicity.

### 4c. Materials
There are currently 3 materials (see material.h) that can be applied to the spheres, each of which causes the rays interacting with a sphere surface to behave differently. 
The following table provides a brief description.

| Material | Constructor | Description |
| :---: | :---: | --- |
| <em>Lambertian</em> | lambertian(const color& albedo) | Diffuse scattering material. Albedo specifies how much red/green/blue light is reflected (0 -> 1 per component) |
| <em>Metal</em> | metal(const color& albedo, double fuzz) | Metal material that reflects incoming rays. Albedo specifies how much red/green/blue light is reflected (0 -> 1 per component). Fuzz (0 -> 1) specifies the fuzziness of the reflection. |
| <em>Dielectric</em> | dielectric(double refraction_index) | Glass/water-like material. The refraction_index specifies the refractive index of the material within an enclosing material (e.g., air) |

## 5. Output files
The program will output a file named image.ppm. Ensure that you have an appropriate image viewer (such as <a href="https://www.gimp.org/">GIMP</a>) to open this kind of file. 
An example output image can be seen at the top of this readme file.




