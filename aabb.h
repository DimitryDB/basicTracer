#ifndef AABB_H
#define AABB_H
#include <cmath>
#include <utility>
#include "ray.h"
#include "vec3.h"
class aabb {
public:
    static aabb surrounding_box(aabb box0, aabb box1) {
        vec3 small(fmin(box0.min().x(), box1.min().x()),
            fmin(box0.min().y(), box1.min().y()),
            fmin(box0.min().z(), box1.min().z()));
        vec3 big(fmax(box0.max().x(), box1.max().x()),
            fmax(box0.max().y(), box1.max().y()),
            fmax(box0.max().z(), box1.max().z()));
        return aabb(small, big);
    }
    aabb() {}
    aabb(const vec3& a, const vec3& b) {
        bounds[0] = a;
        bounds[1] = b;
    }
    vec3 min() const { return bounds[0]; } 
    vec3 max() const { return bounds[1]; }
    vec3 getSize() const { return bounds[1] - bounds[0]; };
    bool hit(const ray& r, double tmin, double tmax) const {
        for (int a  = 0 ; a<3; ++a) {
            double invD = 1.0 / r.direction()[a];
            double t0 = (min()[a] - r.origin()[a])*invD;
            double t1 = (max()[a] - r.origin()[a])*invD;
            if (invD < 0.0)
                std::swap(t0, t1);
            tmin = t0 > tmin ? t0 : tmin;
            tmax = t1 < tmax ? t1 : tmax;
            if (tmax <= tmin)
                return false;
        }   
    return true;
    }
    double area() const { 
        vec3 size = getSize();
        return 2.0*(size.x()*size.y() + size.x()*size.z() + size.y()*size.z());
    }
    int longestAxis() {
        vec3 size = getSize();
        if (size.x() > size.y()) {
            if (size.x() > size.z()) 
                return 0;
            else
                return 2;
            }
        else {
            if (size.y() > size.z())
                return 1;
            else
                return 2;
            }
        }
              
private:
    vec3 bounds[2];

};

inline std::ostream& operator << (std::ostream &os, aabb &t) {
    os <<"Min: "<< t.min() << " Max: " << t.max() << " ";
    return os;
}
#endif // AABB_H
