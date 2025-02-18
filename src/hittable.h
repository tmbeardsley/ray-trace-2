#pragma once

// A class in which to store some important stuff when a ray intersects a hittable.
class hit_record {
  public:
    point3 p;         // Point in space at which the hit occured
    vec3 normal;      // The surface normal at the hit point (convention: points against the ray direction)
    double t;         // The distance, t, along the ray at which the hit occured
    bool front_face;  // Whether the ray intersects the sphere from inside or outside the sphere.

    void set_face_normal(const ray& r, const vec3& outward_normal) {
      // Sets the hit record normal vector.
      // NOTE: the parameter `outward_normal` is assumed to have unit length.
      front_face = dot(r.direction(), outward_normal) < 0;      // Ray hit front face if ray direction and outward normal direction are opposite.
      normal = front_face ? outward_normal : -outward_normal;   // Enforce convention that surface normals oppose the ray direction.
  }
};


// Abstract base class for a hittable. Derived classed must implement hit().
class hittable {
  public:
    virtual ~hittable() = default;

    virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;
};