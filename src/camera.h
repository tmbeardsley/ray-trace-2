#pragma once

#include "hittable.h"
#include "material.h"

class camera {

  public:

    double aspect_ratio       = 1.0;      // Ratio of image width over height
    int    image_width        = 100;      // Rendered image width in pixel count
    int    samples_per_pixel  = 10;       // Count of random ray samples for each pixel
    int    max_depth          = 10;       // Maximum number of ray bounces into scene (if exceeded, no light contributed. I.e., color(0,0,0) returned).

    double vfov = 90;                     // Vertical view angle (field of view)
    point3 lookfrom = point3(0,0,0);      // Point camera is looking from
    point3 lookat   = point3(0,0,-1);     // Point camera is looking at
    vec3   vup      = vec3(0,1,0);        // Camera-relative "up" direction

    // defocus blur (depth of field)
    double defocus_angle      = 0;        // Variation angle of rays through each pixel
    double focus_dist         = 10;       // Distance from camera lookfrom point to plane of perfect focus



    void render(const hittable& world) {

        // Set up the camera parameters
        initialize();

        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        for (int j = 0; j < image_height; j++) {

            std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;

            for (int i = 0; i < image_width; i++) {

                // Will be used to hold the average colour of samples_per_pixel sampled rays
                color pixel_color(0,0,0);

                for (int sample = 0; sample < samples_per_pixel; sample++) {

                  // Get a ray that points at through a random point in the current pixel's space.
                  ray r = get_ray(i, j);

                  // Get the colour of the current ray and add it to the colour sum (will be averaged later)
                  pixel_color += ray_color(r, max_depth, world);
                }

                // Write the average colour of ray samples to file
                write_color(std::cout, pixel_samples_scale * pixel_color);
            }
        }

        std::clog << "\rDone.                 \n";
    }


  private:

    int    image_height;          // Rendered image height
    double pixel_samples_scale;   // Color scale factor for a sum of pixel samples (= 1.0 / samples_per_pixel)
    point3 center;                // Camera center
    point3 pixel00_loc;           // Location of pixel 0, 0
    vec3   pixel_delta_u;         // Offset to pixel to the right
    vec3   pixel_delta_v;         // Offset to pixel below
    vec3   u, v, w;               // Camera frame basis vectors

    vec3   defocus_disk_u;        // Defocus disk radius projected in u-direction
    vec3   defocus_disk_v;        // Defocus disk radius projected in v-direction


    void initialize() {
        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        pixel_samples_scale = 1.0 / samples_per_pixel;

        // Camera centre
        center = lookfrom;

        // Determine viewport dimensions.
        //auto focal_length = (lookfrom - lookat).length();                             // Focal length determined by specified points
        auto theta = degrees_to_radians(vfov);
        auto h = std::tan(theta/2);
        auto viewport_height = 2 * h * focus_dist;                                      // viewport_height defined by field-of-view angle and focal length.
        //auto viewport_height = 2 * h * focal_length;                                    // viewport_height defined by field-of-view angle and focal length.
        auto viewport_width = viewport_height * (double(image_width)/image_height);     // world-space units

        // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        vec3 viewport_u = viewport_width * u;       // Vector across viewport horizontal edge using unit vector, u.
        vec3 viewport_v = viewport_height * -v;     // Vector down viewport vertical edge using unit vector, v.

        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Calculate the location of the upper left pixel in world-space.
        auto viewport_upper_left =  center                        // camera is orthogonal to centre of viewport plane.
                                    - (focus_dist * w)          // viewport is in -w direction from camera.
                                    - viewport_u/2                // viewport_u points in +x direction.
                                    - viewport_v/2;               // viewport_v points in -y direction.

        // Calculate the location of the top-left viewport pixel.
        // Outer pixels are padded so that each pixel takes up the same world-space area.
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        // Calculate the camera defocus disk basis vectors.
        auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }


    ray get_ray(int i, int j) const {
      // Construct a camera ray originating from a random point on the defocus disk 
      // and directed at a randomly sampled point around the pixel location i, j.

      // Random (x,y,0) location within -0.5 < x,y < 0.5.
      auto offset = sample_square();

      // Random point within the bounds of the current pixel's world space.
      auto pixel_sample = pixel00_loc
                        + ((i + offset.x()) * pixel_delta_u)
                        + ((j + offset.y()) * pixel_delta_v);

      auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
      auto ray_direction = pixel_sample - ray_origin;
      auto ray_time = random_double();                                            // Time at which ray was fired, t = [0,1], for motion blur effect.

      return ray(ray_origin, ray_direction, ray_time);
    }


    vec3 sample_square() const {
      // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square.
      return vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }


    point3 defocus_disk_sample() const {
      // Returns a random point in the camera defocus disk in u,v basis vectors.
      auto p = random_in_unit_disk();
      return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }


    color ray_color(const ray& r, int depth, const hittable& world) const {
        
      // If we've exceeded the ray bounce limit, no more light is gathered.
      if (depth <= 0)
        return color(0,0,0);

      hit_record rec;

        // world is a list of hittables. world.hit() returns the closest intersection (or false if none)
        // The t_min = 0.001 is a hack to stop shadow acne effect: round off errors putting origin of next ray below surface (section 9.3).
        if (world.hit(r, interval(0.001, infinity), rec)) {

          ray scattered;
          color attenuation;

          // rec.mat->scatter() is the scattering function of the material the hittable object is made of.
          // It gives us the attenuation factor of the material and a scattered ray object in "attenuation" and "scattered".
          if (rec.mat->scatter(r, rec, attenuation, scattered))         
            return attenuation * ray_color(scattered, depth-1, world);

          return color(0,0,0);  // Case where light was not scattered (absorbed).
        }

        // Create gradient background (sky)
        vec3 unit_direction = unit_vector(r.direction());
        auto a = 0.5*(unit_direction.y() + 1.0);                        // scale a to: 0 <= a <= 1
        return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
    }
};