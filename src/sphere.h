#pragma once

#include "hittable.h"

// The sphere class inherits from the abstract base class, hittable
class sphere : public hittable {

  public:

    // Constructor requires the stationary sphere to have a centre, radius and material.
    sphere(const point3& static_center, double radius, shared_ptr<material> mat) :
      center(static_center, vec3(0,0,0)),                                             // ray with direction (0,0,0) means sphere centre doesn't move with time
      radius(std::fmax(0,radius)), 
      mat(mat) {}
    

    // Moving Sphere has its centre at center1 at time=0, and at center2 at time=1. The centre moves linearly between those points.
    sphere(const point3& center1, const point3& center2, double radius, shared_ptr<material> mat) :
      center(center1, center2 - center1),                                             // Ray with direction = center2 - center1, to linearly track sphere centre
      radius(std::fmax(0,radius)), 
      mat(mat) {}


    // Returns true if the specified ray intersects the sphere.
    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {

        // Use the quadratic formula to determine whether the ray, r, intersects the sphere.
        point3 current_center = center.at(r.time());              // Determine the sphere centre at the time the current ray was fired
        vec3 oc = current_center - r.origin();                    // Vector pointing to the sphere centre relative to the ray origin
        auto a = r.direction().length_squared();
        auto h = dot(r.direction(), oc);
        auto c = oc.length_squared() - radius*radius;

        // Return false if ray doesn't intersect sphere
        auto discriminant = h*h - a*c;
        if (discriminant < 0)
            return false;

        auto sqrtd = std::sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        auto root = (h - sqrtd) / a;                    // Get smallest t
        if (!ray_t.surrounds(root)) {
            root = (h + sqrtd) / a;                     // Try other possible t
            if (!ray_t.surrounds(root))
                return false;                           // Return false if t not in acceptable range.
        }

        // Save important stuff in the hit_record object.
        rec.t = root;                                             // Distance along ray to intersection point.
        rec.p = r.at(rec.t);                                      // Location of intersection point in world space.
        vec3 outward_normal = (rec.p - current_center) / radius;  // Unit outward surface normal at intersection point with sphere.
        rec.set_face_normal(r, outward_normal);                   // Set the unit face normal (enforced to oppose the ray direction).
        rec.mat = mat;                                            // Record a pointer to the material object associated with the sphere.

        return true;
    }


  private:
    ray center;                         // Sphere centre now specified by a ray as it's time dependent
    double radius;
    shared_ptr<material> mat;           // Pointer to a material object that defines scattered ray behaviour
};