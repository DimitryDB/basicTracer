#ifndef SPHERE_H
#define SPHERE_H

#define _USE_MATH_DEFINES
// VS 2015 update 3  math constants don't work with c++ header
#include <math.h>
#include "aabb.h"
#include "hitable.h"
#include "material.h"


class sphere : public hitable {
public:
    sphere() {}
    sphere(vec3 cen, double r, material* ptr) :center(cen), radius(r), mat_ptr(ptr) {};
    virtual bool hit(const ray& r, const double t_min, const double t_max, hit_record& rec) const;
    virtual bool bounding_box(double t0, double t1, aabb& box) const;
private:
    ~sphere() {}
    void getSphereUV(vec3 &p, double &u, double &v) const {
        double phi = atan2(p.z(), p.x());
        double theta = asin(p.y());
        u = 1 - (phi + M_PI) / (2 * M_PI);
        v = (theta + M_PI /2 ) / M_PI;
    }
    
    vec3 center;
    double radius;
    material *mat_ptr;
};

class moving_sphere : public hitable {
public:
    moving_sphere() {}
    moving_sphere(vec3 cen0,vec3 cen1,double t0, double t1, double r, material* ptr) :center0(cen0),center1(cen1), time0(t0),time1(t1), radius(r), mat_ptr(ptr) {};
    virtual bool hit(const ray& r, const double t_min, const double t_max, hit_record& rec) const;
    virtual bool bounding_box(double t0, double t1, aabb& box) const;
    vec3 center(double time) const;
private:
    ~moving_sphere() {}
    // calculate uv on unit sphere at the origin
    // consider this then call
    void getSphereUV(vec3 &p, double &u, double &v) const {
        double phi = atan2(p.z(), p.x());
        double theta = asin(p.y());
        u = 1 - (phi + M_PI) / (2 * M_PI);
        v = (theta + M_PI / 2) / M_PI;
    }
    vec3 center0, center1;
    double time0, time1;
    double radius;
    material *mat_ptr;
};

 bool sphere::hit(const ray& r, const double t_min, const double t_max, hit_record& rec) const {
     
     vec3 oc = r.origin() - center;
     double a = dot(r.direction(), r.direction());
     double b = dot(oc, r.direction());
     double c = dot(oc, oc) - radius*radius;
     double discriminant = b*b - a*c;
     if (discriminant > 0) {
         double temp = (-b - sqrt(b*b - a*c)) / a;
         if (temp < t_max && temp > t_min) {
             rec.t = temp;
             rec.p = r.point_at_parameter(temp);
             rec.normal = (rec.p - center) /radius;
             rec.mat_ptr = mat_ptr;
             rec.time = r.time();
             getSphereUV((rec.p-center)/radius, rec.u, rec.v);
             return true;
         }
         temp = (-b + sqrt(b*b - a*c)) / a;
         if (temp < t_max && temp > t_min) {
             rec.t = temp;
             rec.p = r.point_at_parameter(temp);
             rec.normal = (rec.p - center) /radius;
             rec.mat_ptr = mat_ptr;
             rec.time = r.time();
             getSphereUV((rec.p - center) / radius, rec.u, rec.v);
             return true;
         }    
     }
     return false;
}

 bool sphere::bounding_box(double t0, double t1, aabb& box) const {
     box = aabb(center - vec3(radius, radius, radius), center + vec3(radius, radius, radius));
     return true;
 }

 vec3 moving_sphere::center(double time) const {
     return center0 + (time - time0) / (time1 - time0)*(center1 - center0);
}
 bool moving_sphere::hit(const ray& r, const double t_min, const double t_max, hit_record& rec) const {
     vec3 oc = r.origin() - center(r.time());
     double a = dot(r.direction(), r.direction());
     double b = dot(oc, r.direction());
     double c = dot(oc, oc) - radius*radius;
     double discriminant = b*b - a*c;
     if (discriminant > 0) {
         double temp = (-b - sqrt(b*b - a*c)) / a;
         if (temp < t_max && temp > t_min) {
             rec.t = temp;
             rec.p = r.point_at_parameter(temp);
             rec.normal = (rec.p - center(r.time())) / radius;
             rec.mat_ptr = mat_ptr;
             getSphereUV((rec.p - center(r.time())) / radius, rec.u, rec.v);
             rec.time = r.time();
             return true;
         }
         temp = (-b + sqrt(b*b - a*c)) / a;
         if (temp < t_max && temp > t_min) {
             rec.t = temp;
             rec.p = r.point_at_parameter(temp);
             rec.normal = (rec.p - center(r.time())) / radius;
             rec.mat_ptr = mat_ptr;
             rec.time = r.time();
             getSphereUV((rec.p - center(r.time())) / radius, rec.u, rec.v);
             return true;
         }
     }
     return false;
 }
 bool moving_sphere::bounding_box(double t0, double t1, aabb& box) const {
     aabb box1 = aabb(center(t0) - vec3(radius, radius, radius), center(t0) + vec3(radius, radius, radius));
     aabb box2 = aabb(center(t1) - vec3(radius, radius, radius), center(t1) + vec3(radius, radius, radius));
     box = aabb::surrounding_box(box1, box2);
     return true;
}

#endif // SPHERE_H

