#include "rtweekend.h"

#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"


// Placeholder for colour of ray
color ray_color(const ray& r, const hittable& world) {

    hit_record rec;
    if (world.hit(r, interval(0, infinity), rec)) {
        return 0.5 * (rec.normal + color(1,1,1));
    }

    // Create gradient background
    vec3 unit_direction = unit_vector(r.direction());
    auto a = 0.5*(unit_direction.y() + 1.0);                            // scale a to: -1 <= a <= +1
    return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
}



int main() {

    // Image

    auto aspect_ratio = 16.0 / 9.0;
    int image_width = 400;  // pixels

    // Calculate the image height, and ensure that it's at least 1.
    int image_height = int(image_width / aspect_ratio);     // pixels
    image_height = (image_height < 1) ? 1 : image_height;


    // World

    hittable_list world;

    world.add(make_shared<sphere>(point3(0,0,-1), 0.5));
    world.add(make_shared<sphere>(point3(0,-100.5,-1), 100));


    // Camera

    auto focal_length = 1.0;
    auto viewport_height = 2.0;                                                         // world-space units
    auto viewport_width = viewport_height * (double(image_width)/image_height);         // world-space units
    auto camera_center = point3(0, 0, 0);

    // Calculate the vectors across the horizontal and down the vertical viewport edges.
    auto viewport_u = vec3(viewport_width, 0, 0);
    auto viewport_v = vec3(0, -viewport_height, 0);

    // Calculate the horizontal and vertical delta vectors from pixel to pixel.
    auto pixel_delta_u = viewport_u / image_width;
    auto pixel_delta_v = viewport_v / image_height;

    // Calculate the location of the upper left pixel in world-space.
    auto viewport_upper_left =  camera_center                   // camera is orthogonal to centre of viewport plane.
                                - vec3(0, 0, focal_length)      // viewport is in -z direction from camera.
                                - viewport_u/2                  // viewport_u points in +x direction.
                                - viewport_v/2;                 // viewport_v points in -y direction.
    
    // Calculate the location of the top-left viewport pixel.
    // Outer pixels are padded so that each pixel takes up the same world-space area.
    auto pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);


    // Render

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = 0; j < image_height; j++) {

        std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;

        for (int i = 0; i < image_width; i++) {

            auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);    // Current pixel location in world-space
            auto ray_direction = pixel_center - camera_center;                              // Direction of ray from camera to pixel (not unit-vector)
            ray r(camera_center, ray_direction);                                            // Create a ray object that is fired from camera toward current pixel

            color pixel_color = ray_color(r, world);                                        // Get the colour of the ray depending on its hit
            write_color(std::cout, pixel_color);

        }
    }

    std::clog << "\rDone.                 \n";
}