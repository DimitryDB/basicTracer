#ifndef SAMPLER_H
#define SAMPLER_H
#include "camera.h"
#include <vector>
#include "ray.h"
#include "Hitable.h"
#include "Camera.h"

extern "C" double drand48();
// sampler function
vec3 sample(const ray& r, const hitable *world, int depth) {
    hit_record rec;
    if (world->hit(r, 0.0001, DBL_MAX, rec)) {
        ray scattered;
        vec3 attenuation;
        vec3 emmited = rec.mat_ptr->emmited(rec.u, rec.v, rec.p);
        if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
            return emmited +attenuation*sample(scattered, world, depth + 1);
        }
        else {
            return emmited;
        }
    }
    else {
        return vec3(0, 0, 0);
    }
}

#endif // !SAMPLER_H
