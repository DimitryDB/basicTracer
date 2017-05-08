#ifndef HITABLE_H
#define HITABLE_H

#include "aabb.h"
#include "ray.h"


class material;

struct hit_record {
    double time;
    double t;
    double u, v;
    vec3 p;
    vec3 normal;
    material * mat_ptr;
};

class hitable {
public:
    virtual bool hit(const ray& r, const double t_min, const double t_max, hit_record& rec) const = 0;
    virtual bool bounding_box(double t0, double t1, aabb& box) const = 0;
    virtual ~hitable() {};
};


#endif // HITABLE_H

