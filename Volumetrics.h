#ifndef VOLUMETRICS_H
#define VOLUMETRICS_H

#include "hitable.h"
#include "material.h"
#include <cmath>
extern "C" double drand48();

class constant_medium : public hitable {
public:
    constant_medium(hitable *b, double d, texture *a) : boundary(b), density(d) {
        phase_function = new isotropic(a); }
    ~constant_medium() {
        delete boundary;
        delete phase_function;
    }
    virtual bool hit(const ray& r, const double t_min, const double t_max, hit_record& rec) const;
    virtual bool bounding_box(double t0, double t1, aabb& box) const {
        return boundary->bounding_box(t0, t1, box); }

private:
    hitable *boundary;
    double density;
    material *phase_function;
};
class nonuniform_medium : public hitable {
public:
    nonuniform_medium(hitable *b, texture *d, double ov, texture *a) : boundary(b), density(d), overall_density(ov) {
        phase_function = new isotropic(a);
    }
    ~nonuniform_medium() {
        delete density;
        delete boundary;
        delete phase_function;
    }
    virtual bool hit(const ray& r, const double t_min, const double t_max, hit_record& rec) const;
    virtual bool bounding_box(double t0, double t1, aabb& box) const {
        return boundary->bounding_box(t0, t1, box);
    }

private:
    double overall_density;
    hitable *boundary;
    texture *density;
    material *phase_function;
};

bool constant_medium::hit(const ray& r, const double t_min, const double t_max, hit_record& rec) const {
    //bool db = false;
    hit_record rec1, rec2;
    if (boundary->hit(r, -DBL_MAX, DBL_MAX, rec1)) {
        if (boundary->hit(r, rec1.t + 0.0001, DBL_MAX, rec2)) {
            //if (db) std::cerr << "\nt0 t1 " << rec1.t << " " << rec2.t << "\n";
            if (rec1.t < t_min)
                rec1.t = t_min;
            if (rec2.t > t_max)
                rec2.t = t_max;
            if (rec1.t >= rec2.t)
                return false;
            if (rec1.t < 0)
                rec1.t = 0;
            double distance_inside_boundary = (rec2.t - rec1.t)*r.direction().length();
            double hit_distance = -(1 / density)*log(drand48());
            if (hit_distance < distance_inside_boundary) {
                //if (db) std::cerr << "hit_distance = " << hit_distance << "\n";
                rec.t = rec1.t + hit_distance / r.direction().length();
                //if (db) std::cerr << "rec.t = " << rec1.t << "\n";
                rec.p = r.point_at_parameter(rec.t);
                //if (db) std::cerr << "rec.p = " << rec.p << "\n";
                rec.normal = vec3(1, 0, 0);
                rec.mat_ptr = phase_function;
                return true;
            }
        }
    }
    return false;
}
bool nonuniform_medium::hit(const ray& r, const double t_min, const double t_max, hit_record& rec) const {
    hit_record rec1, rec2;
    if (boundary->hit(r, -DBL_MAX, DBL_MAX, rec1)) {
        if (boundary->hit(r, rec1.t + 0.0001, DBL_MAX, rec2)) {
            if (rec1.t < t_min)
                rec1.t = t_min;
            if (rec2.t > t_max)
                rec2.t = t_max;
            if (rec1.t >= rec2.t)
                return false;
            if (rec1.t < 0)
                rec1.t = 0;
            double distance_inside_boundary = (rec2.t - rec1.t)*r.direction().length();
            double hit_distance = 0;
            while (1) {
                double x = drand48();
                hit_distance += -log(1 - x) / overall_density;
                vec3 new_pos = r.origin() + hit_distance*r.direction();
                if ((density->value(0, 0, new_pos)*0.99 > drand48()))
                    break;
            }
            if (hit_distance < distance_inside_boundary) {
                rec.t = rec1.t + hit_distance / r.direction().length();
                rec.p = r.point_at_parameter(rec.t);
                rec.normal = vec3(1, 0, 0);
                rec.mat_ptr = phase_function;
                return true;
            }
        }
    }
    return false;
}


#endif // !VOLUMETRICS_H
