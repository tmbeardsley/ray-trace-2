#pragma once

// 3D axis-aligned bounding box class
class aabb {

    public:

        // Intervals in the x,y,z dimensions defining the coordinates of the bounding planes.
        interval x, y, z;
    
        aabb() {} // The default AABB is empty, since intervals are empty by default.
    
        aabb(const interval& x, const interval& y, const interval& z)
            : x(x), y(y), z(z) {}
    
        aabb(const point3& a, const point3& b) {
            // Treat the two points a and b as extrema for the bounding box, so we don't require a
            // particular minimum/maximum coordinate order. ie, a = (x0, y0, z0), b = (x1, y1, z1).
    
            x = (a[0] <= b[0]) ? interval(a[0], b[0]) : interval(b[0], a[0]);
            y = (a[1] <= b[1]) ? interval(a[1], b[1]) : interval(b[1], a[1]);
            z = (a[2] <= b[2]) ? interval(a[2], b[2]) : interval(b[2], a[2]);
        }

        // Create a bouding box that encompasses box0 and box1 (the interval constructor deals with the min/max)
        aabb(const aabb& box0, const aabb& box1) {
            x = interval(box0.x, box1.x);
            y = interval(box0.y, box1.y);
            z = interval(box0.z, box1.z);
        }
    
        // Return the bounding box interval of a particular dimension.
        const interval& axis_interval(int n) const {
            if (n == 1) return y;
            if (n == 2) return z;
            return x;
        }
    

        // ray_t is a copy of the interval over which intersections of the incoming ray are valid
        bool hit(const ray& r, interval ray_t) const {
            const point3& ray_orig = r.origin();
            const vec3&   ray_dir  = r.direction();
    
            // Test for whether a ray hits a bounding box. Look at each axis independently.
            // Reminder for ray equation: _P(t) = _Q + t*_d.
            // For bounding box intersections: t_i = (P_i - Q_i)/d_i for each i=x,y,z
            for (int axis = 0; axis < 3; axis++) {

                const interval& ax = axis_interval(axis);       
                const double adinv = 1.0 / ray_dir[axis];
    
                // t_i = (P_i - Q_i)/d_i for max and min of bounding box.
                auto t0 = (ax.min - ray_orig[axis]) * adinv;
                auto t1 = (ax.max - ray_orig[axis]) * adinv;
    
                // A hit is defined by an overlap in the tx,ty,tz over which the ray intersects the planes of the bounding box.
                // Note that the initial intersection of the ray with the bounding box in x (axis=0) is compared to the ray's interval of valid intersections.
                if (t0 < t1) {
                    if (t0 > ray_t.min) ray_t.min = t0;
                    if (t1 < ray_t.max) ray_t.max = t1;
                } else {
                    if (t1 > ray_t.min) ray_t.min = t1;
                    if (t0 < ray_t.max) ray_t.max = t0;
                }
    
                // Immediately return false if any of the axes don't satisfy the overlap condition.
                if (ray_t.max <= ray_t.min)
                    return false;
            }

            return true;
        }


        // Returns the index of the longest axis of the bounding box: 0 (x), 1 (y), 2 (z).
        int longest_axis() const {
            if (x.size() > y.size())
                return x.size() > z.size() ? 0 : 2;
            else
                return y.size() > z.size() ? 1 : 2;
        }


        // Bounding boxes that encompass nothing (empty), or everything (universe).
        static const aabb empty, universe;
};


// Define static constants for bounding boxes that contain everything or nothing.
const aabb aabb::empty    = aabb(interval::empty,    interval::empty,    interval::empty);
const aabb aabb::universe = aabb(interval::universe, interval::universe, interval::universe);