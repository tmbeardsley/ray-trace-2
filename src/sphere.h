#pragma once

#include "hittable.h"

// The sphere class inherits from the abstract base class, hittable
class sphere : public hittable {

  public:
    // Constructor requires the sphere to have a centre and a radius.
    // ray_tmin < t < ray_tmax, allow us to specify a range of valid intersection points (e.g., discounting -ve t).
    sphere(const point3& center, double radius) : center(center), radius(std::fmax(0,radius)) {}

    // Returns true if the specified ray intersects the sphere.
    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {

        // Use the quadratic formula to determine whether the ray, r, intersects the sphere.
        vec3 oc = center - r.origin();
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
        rec.t = root;                                     // Distance along ray to intersection point.
        rec.p = r.at(rec.t);                              // Location of intersection point in world space.
        vec3 outward_normal = (rec.p - center) / radius;  // Unit outward surface normal at intersection point with sphere.
        rec.set_face_normal(r, outward_normal);           // Set the unit face normal (enforced to oppose the ray direction)

        return true;
    }

  private:
    point3 center;
    double radius;
};