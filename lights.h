#ifndef LIGHTS_H
#define LIGHTS_H

#include "material.h"

class diffuse_light : public material {
public:
    diffuse_light(texture* a) : emit(a) {}
    ~diffuse_light() {}
    virtual bool scatter(const ray& r_in,const hit_record& rec, vec3& attenuation, ray& scattered) const {
        return false; }
    virtual vec3 emmited(double u, double v, const vec3 &p) const { return emit->value(u, v, p); }
private:
    texture *emit;
};

#endif // !LIGHTS_H

