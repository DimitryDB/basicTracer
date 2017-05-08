#ifndef PERLIN_H
#define PERLIN_H


#include "vec3.h"
#include <cmath>

extern "C" double drand48();
extern "C" void srand48(long);

class perlin {
public:
    perlin() {
        indexes = new int[arraySize];
        randomArray = new vec3[arraySize];
        for (int i = 0; i < arraySize; ++i) 
            randomArray[i] = unit_vector(vec3(-1 + 2*drand48(), -1 + 2 * drand48(), 
                -1 + 2 * drand48()));
        for (int i = 0; i < arraySize; ++i)
            indexes[i] = i;
        permutate();
    }
    ~perlin() {
        delete[] randomArray;
        delete[] indexes;
    }
    double getNoise(const vec3& p) {
        double u = p.x() - floor(p.x());
        double v = p.y() - floor(p.y());
        double w = p.z() - floor(p.z());
        int i = static_cast<int>(floor(p.x()));
        int j = static_cast<int>(floor(p.y()));
        int k = static_cast<int>(floor(p.z()));
        vec3 domain[2][2][2];
        for (int di = 0; di < 2; ++di)
            for (int dj = 0; dj < 2; ++dj)
                for (int dk = 0; dk < 2; ++dk)
                    domain[di][dj][dk] = randomArray[indexes[(i+di) & (arraySize - 1)] 
                    ^ indexes[(j+dj) & (arraySize - 1)] ^ indexes[(k+dk) & (arraySize - 1)]];
        return trilinearInterp(domain,u,v,w);
    }
    double getTurb(const vec3& p, int depth=7) {
        double accum = 0;
        vec3 tmp_p = p;
        double weight = 1;
        for (int i = 0; i < depth; ++i) {
            accum += weight*getNoise(tmp_p);
            weight *= 0.5;
            tmp_p *= 2;
        }
        return fabs(accum);
    }
private:
    static constexpr int arraySize = 256;
    vec3* randomArray;
    int* indexes;
    void permutate(long seed = 0) {
        for (int i = arraySize - 1; i > 0; --i) {
            if (seed !=0)
                srand48(seed);
            int target = static_cast<int>(drand48()*(i + 1));
            int tmp = indexes[i];
            indexes[i] = indexes[target];
            indexes[target] = tmp;
        }
    }
    double trilinearInterp(vec3 domain[2][2][2], double u, double v, double w) {
        double uu = u*u*(3 - 2 * u);
        double vv = v*v*(3 - 2 * v);
        double ww = w*w*(3 - 2 * w);
        double accum = 0.;
        for (int i = 0; i < 2; ++i)
            for (int j = 0; j < 2; ++j)
                for (int k = 0; k < 2; ++k) {
                    vec3 weight_v(u - i, v - j, w - k);
                    accum += (i*uu + (1 - i)*(1 - uu)) *
                    (j*vv + (1 - j)*(1 - vv)) *
                    (k*ww + (1 - k)*(1 - ww)) * dot(domain[i][j][k], weight_v);
                    }
        return accum;
    }
};

#endif // !PERLIN_H

