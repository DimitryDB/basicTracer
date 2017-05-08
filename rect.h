#ifndef RECT_H
#define RECT_H

#include "hitable.h"
#include "material.h"
#include "transforms.h"

class xy_rect : public hitable {
public:
    xy_rect() {}
    xy_rect(double _x0, double _x1, double _y0, double _y1, double _k, material *_mat) :
        x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_k), mat(_mat) {}
    ~xy_rect() {
        delete mat;
        mat = 0;
    }
    virtual bool hit(const ray& r, const double t_min, const double t_max, hit_record& rec) const;
    virtual bool bounding_box(double t0, double t1, aabb& box) const {
        box = aabb(vec3(x0, y0, k - 0.000001), vec3(x1, y1, k + 0.000001));
        return true;
    }
private:
    material *mat;
    double x0, x1, y0, y1, k;
};

class xz_rect : public hitable {
public:
    xz_rect() {
        delete mat;
        mat = 0;
    }
    xz_rect(double _x0, double _x1, double _z0, double _z1, double _k, material *_mat) :
        x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_k), mat(_mat) {}
    ~xz_rect() {}
    virtual bool hit(const ray& r, const double t_min, const double t_max, hit_record& rec) const;
    virtual bool bounding_box(double t0, double t1, aabb& box) const {
        box = aabb(vec3(x0, k - 0.000001, z0), vec3(x1, k + 0.000001, z1));
        return true;
    }
private:
    material *mat;
    double x0, x1, z0, z1, k;
};

class yz_rect : public hitable {
public:
    yz_rect() {}
    yz_rect(double _y0, double _y1, double _z0, double _z1, double _k, material *_mat) :
        y0(_y0), y1(_y1), z0(_z0), z1(_z1), k(_k), mat(_mat) {}
    ~yz_rect() {}
    virtual bool hit(const ray& r, const double t_min, const double t_max, hit_record& rec) const;
    virtual bool bounding_box(double t0, double t1, aabb& box) const {
        box = aabb(vec3(k - 0.000001, y0, z0), vec3(k + 0.000001, y1, z1));
        return true;
    }
private:
    material *mat;
    double y0, y1, z0, z1, k;
};

class box : public hitable {
public:
    box() {}
    box(const vec3& p0, const vec3& p1, material *ptr);
    ~box() { delete list_ptr; }
    virtual bool hit(const ray& r, const double t_min, const double t_max, hit_record& rec) const;
    virtual bool bounding_box(double t0, double t1, aabb& box) const {
        box = aabb(pmin, pmax);
        return true;
    }
private:
    hitable *list_ptr;
    vec3 pmin, pmax;
};

box::box(const vec3& p0, const vec3& p1, material *ptr) {
    pmin = p0;
    pmax = p1;
    hitable **list = new hitable*[6];
    list[0] = new xy_rect(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), ptr);
    list[1] = new flip_normals(new xy_rect(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), ptr));
    list[2] = new xz_rect(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), ptr);
    list[3] = new flip_normals(new xz_rect(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), ptr));
    list[4] = new yz_rect(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), ptr);
    list[5] = new flip_normals(new yz_rect(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), ptr));
    list_ptr = new hitable_list(list, 6);
}
bool box::hit(const ray& r, const double t_min, const double t_max, hit_record& rec) const {
    return list_ptr->hit(r, t_min, t_max, rec);
}

bool xy_rect::hit(const ray& r, const double t_min, const double t_max, hit_record& rec) const {
    double t = (k - r.origin().z()) / r.direction().z();
    if (t<t_min || t>t_max)
        return false;
    double x = r.origin().x() + t*r.direction().x();
    double y = r.origin().y() + t*r.direction().y();
    if (x < x0 || x > x1 || y < y0 || y > y1)
        return false;
    rec.u = (x - x0) / (x1 - x0);
    rec.v = (y - y0) / (y1 - y0);
    rec.t = t;
    rec.mat_ptr = mat;
    rec.p = r.point_at_parameter(t);
    rec.normal = vec3(0, 0, 1);
    return true;
}

bool xz_rect::hit(const ray& r, const double t_min, const double t_max, hit_record& rec) const {
    double t = (k - r.origin().y()) / r.direction().y();
    if (t<t_min || t>t_max)
        return false;
    double x = r.origin().x() + t*r.direction().x();
    double z = r.origin().z() + t*r.direction().z();
    if (x < x0 || x > x1 || z < z0 || z > z1)
        return false;
    rec.u = (x - x0) / (x1 - x0);
    rec.v = (z - z0) / (z1 - z0);
    rec.t = t;
    rec.mat_ptr = mat;
    rec.p = r.point_at_parameter(t);
    rec.normal = vec3(0, 1, 0);
    return true;
}

bool yz_rect::hit(const ray& r, const double t_min, const double t_max, hit_record& rec) const {
    double t = (k - r.origin().x()) / r.direction().x();
    if (t<t_min || t>t_max)
        return false;
    double y = r.origin().y() + t*r.direction().y();
    double z = r.origin().z() + t*r.direction().z();
    if (y < y0 || y > y1 || z < z0 || z > z1)
        return false;
    rec.u = (y - y0) / (y1 - y0);
    rec.v = (z - z0) / (z1 - z0);
    rec.t = t;
    rec.mat_ptr = mat;
    rec.p = r.point_at_parameter(t);
    rec.normal = vec3(1, 0, 0);
    return true;
}
#endif // !RECT_H
