#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class ray {
public:
	ray() {}
	ray(const vec3& a, const vec3& b,  const double ti=0.0) : orig(a), dir(b), _time(ti) {
    }
	vec3 origin() const { return orig; }
	vec3 direction() const { return dir; }
    double time() const { return _time; }
	vec3 point_at_parameter(double t) const { return orig + dir * t; }

private:
	vec3 orig;
	vec3 dir;
    double _time;
};
#endif // RAY_H
