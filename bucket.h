#ifndef BUCKET_H
#define BUCKET_H
#include <stack>
#include <vector>
#include "camera.h"
#include "Hitable.h"
#include "sample.h"
#include <cmath>
#include <iostream>
#include "threadsafeStack.h"

extern "C" double drand48();

class bucket {
public:
    // factory function
    static threadsafeStack<bucket*>  *createBuckets(int imageSizeX, int imageSizeY, int numderOfBucketsX, int numberOfBucketsY);
    ~bucket() {
        result.clear();
    }
    //double getProgress() const { return progress; }
    bool sampler(const camera &cam, const hitable* world, const int &ns);
    bool outputToArray( int *out);
    
private:
    bucket(int sx, int ex, int sy, int ey, int isx, int isy) : x_start(sx), x_end(ex), y_start(sy), y_end(ey), imageSizeX(isx), imageSizeY(isy) {
        int size = (ex - sx) * (ey - sy);
        result.reserve(size * 3);
        //progress = 0;
    }
    int imageSizeX, imageSizeY;
    int x_start, x_end;
    int y_start, y_end;
    //double progress;
    std::vector<int> result;
   
};
bool bucket::sampler(const camera &cam,  const hitable* world, const int &ns) {
    for (int j = y_start; j <y_end; ++j) {
        for (int i = x_start; i <  x_end; ++i) {
            vec3 col(0, 0, 0);
            for (int s = 0; s < ns; ++s) {
                double u = double(i + drand48()) / double(imageSizeX);
                double v = double(j + drand48()) / double(imageSizeY);
                ray r = cam.get_ray(u, v);
                col += sample(r, world, 0);
                
            }
            col /= double(ns);
            col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
            if (col[0] > 1) col[0] = 1;
            if (col[1] > 1) col[1] = 1;
            if (col[2] > 1) col[2] = 1;
            int ir = int(255.99*col[0]);
            int ig = int(255.99*col[1]);
            int ib = int(255.99*col[2]);
            result.push_back(ir);
            result.push_back(ig);
            result.push_back(ib);
        }
        //progress = (double)j / ((double)y_end / 100.0);
        //system("CLS");
        //std::cout << "Bucket progres: " << progress << "%\n";
    }
    return true;
}
bool bucket::outputToArray(int* out) {
    int startOffset = ((imageSizeX * y_start) + x_start)*3;
    int incrementalOffset = (imageSizeX - (x_end - x_start))*3;
    int pixels = (x_end - x_start);
    int lines = (y_end - y_start);
    int iter = 0;
    int iterOut = startOffset;
    for (int i = 0; i<lines; ++i) {
        for (int j = 0; j < pixels; ++j) {
            for (int k = 0;k < 3;++k) {
                //std::cout << iter << "\n";
                out[iterOut] = result[iter];
                iter++;
                iterOut++;
            }
        }
        iterOut += incrementalOffset;
    }
    return true;
}
threadsafeStack<bucket*> *bucket::createBuckets(int imageSizeX, int imageSizeY, int numberOfBucketsX, int numberOfBucketsY) {
    threadsafeStack<bucket*> *res = new threadsafeStack<bucket*>;
    int offsetX = imageSizeX / numberOfBucketsX;
    int offsetY = imageSizeY / numberOfBucketsY;
    for (int i = 0; i < numberOfBucketsY; ++i) {
        for (int j = 0; j < numberOfBucketsX; ++j) {
            bucket *tmp = new bucket(j*offsetX, (j + 1)*offsetX, i*offsetY, (i + 1)*offsetY, imageSizeX, imageSizeY);
            res->push(tmp);
        }
    }
    return res;
}
#endif // !BUCKET_H
