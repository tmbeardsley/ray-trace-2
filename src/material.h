#pragma once

#include "hittable.h"


// Abstract base class for materials
class material {

  public:

    virtual ~material() = default;

    // Base function absorbs the incoming ray by default (return false)
    virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const {
        return false;
    }
};


// A lambertian material class that is a subclass of the material class.
class lambertian : public material {

    public:

      lambertian(const color& albedo) : albedo(albedo) {}
  
      // Overriding scatter function
      bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {

        auto scatter_direction = rec.normal + random_unit_vector();     // Direction of lambertian scattered ray

        // Catch degenerate scatter direction - happens if rec.normal and random_unit_vector() are almost exactly opposite
        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;

        scattered = ray(rec.p, scatter_direction);                      // Ray object originating from object intersection point
        attenuation = albedo;                                           // Reflectance of r,g,b
        return true;
      }
  
    private:
      color albedo;     // reflectance of r,g,b
};


// A metal material class that is a subclass of the material class. Reflects incoming rays.
class metal : public material {

    public:
        metal(const color& albedo, double fuzz) : 
            albedo(albedo), 
            fuzz(fuzz < 1 ? fuzz : 1) {}

        bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {

            // Direction of reflected ray
            vec3 reflected = reflect(r_in.direction(), rec.normal);

            // Add random perturbation to direction of reflected ray (fuzzy reflection)
            reflected = unit_vector(reflected) + (fuzz * random_unit_vector());

            scattered = ray(rec.p, reflected);                          // Ray object originating from object intersection point
            attenuation = albedo;                                       // Reflectance of r,g,b

            return (dot(scattered.direction(), rec.normal) > 0);        // Ray absorbed (returns false) if fuzziness scattered ray below surface
        }

    private:
        color albedo;
        double fuzz;                // (0 < fuzz < 1) specifies fuzziness of reflected ray (random perturbation to direction of perfect reflection).
};


// Dielectric material that always refracts.
class dielectric : public material {

    public:

        dielectric(double refraction_index) : refraction_index(refraction_index) {}
    

        bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {

            attenuation = color(1.0, 1.0, 1.0);     // No attenuation of ray

            // refractive index get inverted depending on whether we're entering or leaving the medium
            double ri = rec.front_face ? (1.0/refraction_index) : refraction_index;
    
            // Get direction of refracted ray
            vec3 unit_direction = unit_vector(r_in.direction());
            vec3 refracted = refract(unit_direction, rec.normal, ri);
    
            // Create scattered ray
            scattered = ray(rec.p, refracted);

            return true;
        }
  
    private:
        // Refractive index in vacuum or air, or the ratio of the material's refractive index over
        // the refractive index of the enclosing media
        double refraction_index;
  };