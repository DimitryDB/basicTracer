#ifndef TRANSFORMS_H
#define TRANSFORMS_H

#include "hitable.h"
#define _USE_MATH_DEFINES
// VS 2015 update 3  math constants don't work with c++ header
#include <math.h>

class flip_normals : public hitable {
public:
    flip_normals(hitable *p) : ptr(p) {}
    virtual bool hit(const ray& r, const double t_min, const double t_max, hit_record& rec) const {
        if (ptr->hit(r, t_min, t_max, rec)) {
            rec.normal = -rec.normal;
            return true;
        }
        else
            return false;
    }
    ~flip_normals() { delete ptr; }
    virtual bool bounding_box(double t0, double t1, aabb& box) const {
        return ptr->bounding_box(t0, t1, box);
    }
private:
    hitable *ptr;
};

class translate : public hitable {
public:
    translate(hitable *p, const vec3& displacement) : ptr(p), disp(displacement){}
    ~translate() { delete ptr; }
    virtual bool hit(const ray& r, const double t_min, const double t_max, hit_record& rec) const;
    virtual bool bounding_box(double t0, double t1, aabb& box) const;
private:
    hitable *ptr;
    vec3 disp;
};
class rotate_y :public hitable {
public:
    rotate_y(hitable *p, double angle);
    ~rotate_y() { delete ptr; }
    virtual bool hit(const ray& r, const double t_min, const double t_max, hit_record& rec) const;
    virtual bool bounding_box(double t0, double t1, aabb& box) const {
        box = bbox; 
        return true; 
    }
private:
    double sin_theta;
    double cos_theta;
    hitable *ptr;
    //bool hasbox;
    aabb bbox;
};
class rotate_x :public hitable {
public:
    rotate_x(hitable *p, double angle);
    ~rotate_x() { delete ptr; }
    virtual bool hit(const ray& r, const double t_min, const double t_max, hit_record& rec) const;
    virtual bool bounding_box(double t0, double t1, aabb& box) const {
        box = bbox;
        return true;
    }
private:
    double sin_theta;
    double cos_theta;
    hitable *ptr;
    //bool hasbox;
    aabb bbox;
};
class rotate_z :public hitable {
public:
    rotate_z(hitable *p, double angle);
    ~rotate_z() { delete ptr; }
    virtual bool hit(const ray& r, const double t_min, const double t_max, hit_record& rec) const;
    virtual bool bounding_box(double t0, double t1, aabb& box) const {
        box = bbox;
        return true;
    }
private:
    double sin_theta;
    double cos_theta;
    hitable *ptr;
    //bool hasbox;
    aabb bbox;
};
bool translate::hit(const ray& r, const double t_min, const double t_max, hit_record& rec) const {
    ray moved_r(r.origin() - disp, r.direction(), r.time());
    if (ptr->hit(moved_r, t_min, t_max, rec)) {
        rec.p += disp;
        return true;
    }
    else 
        return false;
}
bool translate::bounding_box(double t0, double t1, aabb& box) const {
    if (ptr->bounding_box(t0, t1, box)) {
        box = aabb(box.min() + disp, box.max() + disp);
        return true; 
    }
    else
        return false;
}
rotate_y::rotate_y(hitable *p, double angle) : ptr(p) {
    double radians = (M_PI / 180.0)* angle;
    sin_theta = sin(radians);
    cos_theta = cos(radians);
    bool hasbox = ptr->bounding_box(0, 1, bbox);
    vec3 min(DBL_MAX, DBL_MAX, DBL_MAX);
    vec3 max(-DBL_MAX, -DBL_MAX, -DBL_MAX);
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            for (int k = 0; k < 2; ++k) {
                double x = i *bbox.max().x() + (1 - i)*bbox.min().x();
                double y = j *bbox.max().x() + (1 - j)*bbox.min().y();
                double z = k *bbox.max().z() + (1 - k)*bbox.min().z();
                double newx = cos_theta*x + sin_theta*z;
                double newz = -sin_theta*x + cos_theta*z;
                vec3 test(newx, y, newz);
                for (int c = 0; c < 3; ++c) {
                    if (test[c] > max[c])
                        max[c] = test[c];
                    if (test[c] < min[c])
                        min[c] = test[c];
                }
            }
        }
    }
    bbox = aabb(min, max);
}
bool rotate_y::hit(const ray& r, const double t_min, const double t_max, hit_record& rec) const {
    vec3 origin = r.origin();
    vec3 direction = r.direction();
    origin[0] = cos_theta*r.origin()[0] - sin_theta*r.origin()[2];
    origin[2] = sin_theta*r.origin()[0] + cos_theta*r.origin()[2];
    direction[0] = cos_theta*r.direction()[0] - sin_theta*r.direction()[2];
    direction[2] = sin_theta*r.direction()[0] + cos_theta*r.direction()[2];
    ray rotated_r(origin, direction, r.time());
    if (ptr->hit(rotated_r, t_min, t_max, rec)) {
        vec3 p = rec.p;
        vec3 normal = rec.normal;
        p[0] = cos_theta*rec.p[0] + sin_theta*rec.p[2];
        p[2] = -sin_theta*rec.p[0] + cos_theta*rec.p[2];
        normal[0] = cos_theta*rec.normal[0] + sin_theta*rec.normal[2];
        normal[2] = -sin_theta*rec.normal[0] + cos_theta*rec.normal[2];
        rec.p = p;
        rec.normal = normal;
        return true;
    }
    else
        return false;
}
rotate_x::rotate_x(hitable *p, double angle) : ptr(p) {
    double radians = (M_PI / 180.0)* angle;
    sin_theta = sin(radians);
    cos_theta = cos(radians);
    bool hasbox = ptr->bounding_box(0, 1, bbox);
    vec3 min(DBL_MAX, DBL_MAX, DBL_MAX);
    vec3 max(-DBL_MAX, -DBL_MAX, -DBL_MAX);
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            for (int k = 0; k < 2; ++k) {
                double x = i *bbox.max().x() + (1 - i)*bbox.min().x();
                double y = j *bbox.max().x() + (1 - j)*bbox.min().y();
                double z = k *bbox.max().z() + (1 - k)*bbox.min().z();
                double newy = cos_theta*y - sin_theta*z;
                double newz = sin_theta*y + cos_theta*z;
                vec3 test(x, newy, newz);
                for (int c = 0; c < 3; ++c) {
                    if (test[c] > max[c])
                        max[c] = test[c];
                    if (test[c] < min[c])
                        min[c] = test[c];
                }
            }
        }
    }
    bbox = aabb(min, max);
}
bool rotate_x::hit(const ray& r, const double t_min, const double t_max, hit_record& rec) const {
    vec3 origin = r.origin();
    vec3 direction = r.direction();
    origin[1] = cos_theta*r.origin()[1] + sin_theta*r.origin()[2];
    origin[2] = -sin_theta*r.origin()[1] + cos_theta*r.origin()[2];
    direction[1] = cos_theta*r.direction()[1] + sin_theta*r.direction()[2];
    direction[2] = -sin_theta*r.direction()[1] + cos_theta*r.direction()[2];
    ray rotated_r(origin, direction, r.time());
    if (ptr->hit(rotated_r, t_min, t_max, rec)) {
        vec3 p = rec.p;
        vec3 normal = rec.normal;
        p[1] = cos_theta*rec.p[1] - sin_theta*rec.p[2];
        p[2] = sin_theta*rec.p[1] + cos_theta*rec.p[2];
        normal[1] = cos_theta*rec.normal[1] - sin_theta*rec.normal[2];
        normal[2] = sin_theta*rec.normal[1] + cos_theta*rec.normal[2];
        rec.p = p;
        rec.normal = normal;
        return true;
    }
    else
        return false;
}
rotate_z::rotate_z(hitable *p, double angle) : ptr(p) {
    double radians = (M_PI / 180.0)* angle;
    sin_theta = sin(radians);
    cos_theta = cos(radians);
    bool hasbox = ptr->bounding_box(0, 1, bbox);
    vec3 min(DBL_MAX, DBL_MAX, DBL_MAX);
    vec3 max(-DBL_MAX, -DBL_MAX, -DBL_MAX);
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            for (int k = 0; k < 2; ++k) {
                double x = i *bbox.max().x() + (1 - i)*bbox.min().x();
                double y = j *bbox.max().x() + (1 - j)*bbox.min().y();
                double z = k *bbox.max().z() + (1 - k)*bbox.min().z();
                double newx = cos_theta*x - sin_theta*y;
                double newy = sin_theta*x + cos_theta*y;
                vec3 test(newx, newy, z);
                for (int c = 0; c < 3; ++c) {
                    if (test[c] > max[c])
                        max[c] = test[c];
                    if (test[c] < min[c])
                        min[c] = test[c];
                }
            }
        }
    }
    bbox = aabb(min, max);
}
bool rotate_z::hit(const ray& r, const double t_min, const double t_max, hit_record& rec) const {
    vec3 origin = r.origin();
    vec3 direction = r.direction();
    origin[0] = cos_theta*r.origin()[0] + sin_theta*r.origin()[1];
    origin[1] = -sin_theta*r.origin()[0] + cos_theta*r.origin()[1];
    direction[0] = cos_theta*r.direction()[0] + sin_theta*r.direction()[1];
    direction[1] = -sin_theta*r.direction()[0] + cos_theta*r.direction()[1];
    ray rotated_r(origin, direction, r.time());
    if (ptr->hit(rotated_r, t_min, t_max, rec)) {
        vec3 p = rec.p;
        vec3 normal = rec.normal;
        p[0] = cos_theta*rec.p[0] - sin_theta*rec.p[1];
        p[1] = sin_theta*rec.p[0] + cos_theta*rec.p[1];
        normal[0] = cos_theta*rec.normal[0] - sin_theta*rec.normal[1];
        normal[1] = sin_theta*rec.normal[0] + cos_theta*rec.normal[1];
        rec.p = p;
        rec.normal = normal;
        return true;
    }
    else
        return false;
}
#endif // !TRANSFORMS_H

