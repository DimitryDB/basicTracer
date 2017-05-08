#ifndef CAMERA_H
#define CAMERA_H
#include <cmath>
#include "ray.h"

extern "C" double drand48();

vec3 random_in_unit_disk() {
    vec3 p;
    do {
        p = 2.0* vec3(drand48(), drand48(), 0) - vec3(1, 1, 0);
    } while (p.squared_length() >= 1.0);
    return p;
}

class camera {
public:
    camera(vec3 lookfrom, vec3 lookat, vec3 vup, double vfov, double aspect, double apperture, double focus_dist, double t0, double t1) {
        lens_radius = apperture / 2;
        time0 = t0;
        time1 = t1;
        double v_fov_radian = vfov *Pi / 180;
        double half_heght = tan(v_fov_radian / 2);
        double half_width = aspect*half_heght;
        origin = lookfrom;
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w,u);
        upper_left_corner = origin -half_width*u*focus_dist +half_heght*v*focus_dist -w*focus_dist;
        horizontal = 2 * half_width*focus_dist*u;
        vertical = -2*half_heght*focus_dist*v;
    }
    ray get_ray (double s, double t) const {
        vec3 rd = lens_radius*random_in_unit_disk();
        vec3 offset = u*rd.x() + v*rd.y();
        double tmp_time = time0 + drand48()*(time1 - time0);
        return ray(origin+offset, upper_left_corner + s*horizontal + t*vertical - origin-offset, tmp_time);}
   
    const double Pi = 3.14;
    vec3 origin;
    vec3 vertical;
    vec3 horizontal;
    vec3 upper_left_corner;
    vec3 u, v, w;
    double lens_radius;
    double time0, time1;
};

#endif // CAMERA_H
