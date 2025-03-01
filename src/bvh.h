#pragma once

#include "aabb.h"
#include "hittable.h"
#include "hittable_list.h"

#include <algorithm>            // Access to sort() function.

// Note that bvh_node is itself a subclass of hittable.
class bvh_node : public hittable {

  public:

    bvh_node(hittable_list list) : bvh_node(list.objects, 0, list.objects.size()) {                 // Calls the other constructor, supplying it the objects in the list
        // There's a C++ subtlety here. This constructor (without span indices) creates an
        // implicit copy of the hittable list, which we will modify. The lifetime of the copied
        // list only extends until this constructor exits. That's OK, because we only need to
        // persist the resulting bounding volume hierarchy.
    }


    // The objects vector is the full list of hittables.
    // Start and end define the index range to split up in the current bvh_node.
    bvh_node(std::vector<shared_ptr<hittable>>& objects, size_t start, size_t end) {
        
        // Build the bounding box of the span of source objects, allows us to choose the longest axis to split on.
        // Maximises subdivision of objects and better than a random axis choice.
        bbox = aabb::empty;
        for (size_t object_index=start; object_index < end; object_index++)
            bbox = aabb(bbox, objects[object_index]->bounding_box());

        int axis = bbox.longest_axis();

        // Get a reference to the appropriate box comparison function (randomly selected)
        auto comparator = (axis == 0) ? box_x_compare
                        : (axis == 1) ? box_y_compare
                                      : box_z_compare;
        
        // Number of hittables in objects vector
        size_t object_span = end - start;

        if (object_span == 1) {

            // If there is only one object in the list, add it to both children.
            left = right = objects[start];

        } else if (object_span == 2) {

            // If there are two objects, place one in each of the child nodes.
            left = objects[start];
            right = objects[start+1];

        } else {

            // Three or more objects in the list.
            // Sort items in the current bvh_node's sublist in x, y or z according to the comparitor.
            std::sort(std::begin(objects) + start, std::begin(objects) + end, comparator);

            // Divide the list in two by getting the index of the middle object.
            // Note the the new node gets a copy of the objects list in its constructor.
            auto mid = start + object_span/2;
            left = make_shared<bvh_node>(objects, start, mid);
            right = make_shared<bvh_node>(objects, mid, end);
        }

    }


    // Traverse the bvh tree looking for closest hit (stored in rec).
    // Must look at both left and right: Even if left hits, right might hit again but closer.
    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {

        // Return false if ray does not intersect this node's bounding box
        if (!bbox.hit(r, ray_t))
            return false;

        // Check to see if left child (hittable) node is hit by the incoming ray.
        bool hit_left = left->hit(r, ray_t, rec);

        // Check for intersection with the right child (hittable) node.
        // Note: If left child was hit, the upper bound of the valid intersection interval for the right child becomes that intersection point: rec.t.
        bool hit_right = right->hit(r, interval(ray_t.min, hit_left ? rec.t : ray_t.max), rec);

        return hit_left || hit_right;
    }


    // Accessor for this bvh_node's bounding box.
    aabb bounding_box() const override { return bbox; }




  private:
    shared_ptr<hittable> left;      // Left child (hittable) node
    shared_ptr<hittable> right;     // Right child (hittable) node
    aabb bbox;                      // Bounding box of current bvh_node


    // Return true if the minimum of a's bounding box interval is less than that of b's.
    // axis_index = 0,1,2 -> x,y,z axis.
    static bool box_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b, int axis_index) {
        auto a_axis_interval = a->bounding_box().axis_interval(axis_index);
        auto b_axis_interval = b->bounding_box().axis_interval(axis_index);
        return a_axis_interval.min < b_axis_interval.min;
    }

    static bool box_x_compare (const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
        return box_compare(a, b, 0);
    }

    static bool box_y_compare (const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
        return box_compare(a, b, 1);
    }

    static bool box_z_compare (const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
        return box_compare(a, b, 2);
    }
};