#ifndef TEXTURE_H
#define TEXTURE_H

#include "vec3.h"
#include <cmath>
#include "perlin.h"

class texture {
public:
    virtual vec3 value(double u, double v, const vec3& p) const = 0;
};

class constant_texture : public texture {
public:
    constant_texture() {}
    constant_texture(vec3 c) : color(c) {}
    virtual vec3 value(double u, double v, const vec3& p) const {
        return color;
    }
private:
    vec3 color;
};

class checker_texture : public texture {
public:
    checker_texture() {}
    checker_texture(texture *t0, texture *t1) :even(t0), odd(t1) {}
    ~checker_texture() {
        delete odd;
        delete even;
    }
    virtual vec3 value(double u, double v, const vec3& p) const {
        double sines = sin(10 * p.x()) * sin(10 * p.y()) * sin(10 * p.z());
        if (sines < 0)
            return odd->value(u, v, p);
        else
            return even->value(u, v, p);
    }
private:
    texture *odd;
    texture *even;
};

class noise_texture : public texture {
public:
    noise_texture(double sc = 1, vec3 col = vec3(1, 1, 1)) : scale(sc) ,color(col) {
        noiseFunc = new perlin;
    }
    ~noise_texture() {
        delete noiseFunc;
    }
    virtual vec3 value(double u, double v, const vec3& p) const {
        return color *0.5* (1+ sin(scale *p.z()+5*noiseFunc->getTurb(scale*p)));
    }
private:
    vec3 color;
    double scale;
    perlin* noiseFunc;
};

class image_texture : public texture {
public:
    image_texture() {}
    image_texture(unsigned char *pixels, int x, int y) : data(pixels), nx(x), ny(y) {}
    virtual vec3 value(double u, double v, const vec3& p) const;
private:
    unsigned char* data;
    int nx, ny;
};

vec3 image_texture::value(double u, double v, const vec3& p) const {
    int i = static_cast<int>(u*nx);
    int j = static_cast<int>((1 - v)*ny - 0.0001);
    if (i < 0) i = 0;
    if (j < 0) j = 0;
    if (j > nx - 1) j = nx - 1;
    if (j > ny - 1) j = ny - 1;
    double r = static_cast<int>(data[3 * i + 3 * nx*j]) / 255.0;
    double g = static_cast<int>(data[3 * i + 3 * nx*j+1]) / 255.0;
    double b = static_cast<int>(data[3 * i + 3 * nx*j+2]) / 255.0;
    return vec3(r, g, b);
}

#endif // !TEXTURE_H

