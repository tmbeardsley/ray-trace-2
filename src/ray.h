#pragma once

#include "vec3.h"

// _ray(t) = _origin + t*_direction
class ray {
  public:
    ray() {}

    ray(const point3& origin, const vec3& direction, double time) :           // Sets up a ray and keeps track of time that it was fired
      orig(origin), 
      dir(direction),
      tm(time) {}
    
    ray(const point3& origin, const vec3& direction) :                        // Constructor without time specified assumes time = 0
      ray(origin, direction, 0.0) {}

    const point3& origin() const  { return orig; }
    const vec3& direction() const { return dir; }

    double time() const { return tm; }                                        // Return time that the ray was fired

    // Return a point along the ray
    point3 at(double t) const {
        return orig + t*dir;
    }

  private:
    point3 orig;
    vec3 dir;
    double tm;      // Time at which the ray was fired in [0, 1]
};