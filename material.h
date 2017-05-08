#ifndef MATERIAL_H
#define MATERIAL_H
#include <cmath>
#include "Hitable.h"
#include "ray.h"
#include "vec3.h"
#include "texture.h"

extern "C" double drand48();

// helper functions
vec3 reflect(const vec3& v, const vec3 n) {
    return v - 2 * dot(v, n)*n;
}

vec3 random_in_unit_sphere() {
    vec3 p;
    do {
        p = 2.0* vec3(drand48(), drand48(), drand48()) - vec3(1, 1, 1);
    } while (p.squared_length() >= 1.0);
    return p;
}

bool refract(const vec3& v, const vec3& n, double ni_over_nt, vec3& refracted) {
    vec3 uv = unit_vector(v);
    double dt = dot(uv, n);
    double discriminant = 1.0 - ni_over_nt*ni_over_nt*(1 - dt*dt);
    if (discriminant > 0) {
        refracted = ni_over_nt*(uv - n*dt) - n * sqrt(discriminant);
        return true;
    }
    else
        return false;
}

double schilick(double cosine, double ref_idx) {
    double r0 = 1 - ref_idx / (1 + ref_idx);
    r0 = r0*r0;
    return r0 + (1 - r0)*pow((1 - cosine), 5);
}
// material classes 

class material {
public:  
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const = 0;
    virtual vec3 emmited(double u, double v, const vec3 &p) const { return vec3(0, 0, 0); }
    virtual ~material() { }
};

class lambertian : public material {
public:
    lambertian(texture *a) : albedo(a) {}
    ~lambertian() {}
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
        r_in;
        vec3 target = rec.p + rec.normal + random_in_unit_sphere();
        scattered = ray(rec.p, target - rec.p, rec.time);
        attenuation = albedo->value(rec.u,rec.v,rec.p);
        return true;
    }
private:
    texture *albedo;
};

class metal :public material {
public:
    metal(texture *a, double f) :albedo(a) { if (f < 1) fuzz = f; else fuzz = 1; }
    ~metal() {}
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        scattered = ray(rec.p, reflected + fuzz* random_in_unit_sphere(), rec.time);
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return (dot(scattered.direction(), rec.normal) > 0);
    }
    texture *albedo;
    double fuzz;
};

class dielectric : public material {
public:
    dielectric(double ri) :ref_idx(ri) {}
    ~dielectric() {}
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
        vec3 outward_normal;
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        double ni_over_nt;
        attenuation = vec3(1.0, 1.0, 1.0);
        vec3 refracted;
        double reflect_prob;
        double cosine;
        if (dot(r_in.direction(), rec.normal) > 0) {
            outward_normal = -rec.normal;
            ni_over_nt = ref_idx;
            cosine = ref_idx*dot(r_in.direction(), rec.normal) / r_in.direction().length();
        }
        else {
            outward_normal = rec.normal;
            ni_over_nt = 1/ref_idx;
            cosine = -dot(r_in.direction(), rec.normal) / r_in.direction().length();
        }
        if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted))
            reflect_prob = schilick(cosine, ref_idx);
        else
            reflect_prob = 1.0;
        if (drand48() < reflect_prob)
            scattered = ray(rec.p, reflected, rec.time);
        else
            scattered = ray(rec.p, refracted, rec.time);
        return true;
    }
    double ref_idx;
};
class isotropic : public material {
public:
    isotropic(texture *a) : albedo(a) {}
    ~isotropic() {}
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
        scattered = ray(rec.p, random_in_unit_sphere());
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }
private:
    texture *albedo;
};

#endif // MATERIAL_H
