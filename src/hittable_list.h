#pragma once

#include "aabb.h"
#include "hittable.h"

#include <vector>


// hittable_list is a subclass of the abstract base class, hittable
class hittable_list : public hittable {

  public:
    std::vector<shared_ptr<hittable>> objects;      // Vector of shared pointers to hittable objects

    hittable_list() {}
    hittable_list(shared_ptr<hittable> object) { add(object); }

    void clear() { objects.clear(); }

    // Add a hittable to the objects vector
    void add(shared_ptr<hittable> object) {
        objects.push_back(object);

        // Update the extents of the hittable_list's bounding box any time a hittable object is added to it
        bbox = aabb(bbox, object->bounding_box());
    }

    // This function has to be overridden as it exists in the base class, hittable.
    // Goes through all hittables in the objects vector and looks for the closest intersection of ray with object.
    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        hit_record temp_rec;
        bool hit_anything = false;
        auto closest_so_far = ray_t.max;

        // Consider each hittable object in hittable objects vector.
        for (const auto& object : objects) {

            // Call the hit() function of each hittable object to get closest intersection (if any).
            // If hit() returns true, it has to be a closer intersection than last time because: (ray_tmin < t < closest_so_far) must be true.
            if (object->hit(r, interval(ray_t.min, closest_so_far), temp_rec)) {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }

    // Accessor for the hittable_list's bounding box object.
    aabb bounding_box() const override { return bbox; }


    private:
        aabb bbox;                  // Bounding box that encompassed all hittable children of this hittable_list
};