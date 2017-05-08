#include <cfloat>
#include <ctime>
#include <fstream>
#include <iostream>
#include <stdlib.h> 
#include <crtdbg.h>
#include <cstring>
#include "sphere.h"
#include "hitablelist.h"
#include "Camera.h"
#include "material.h"
#include "bvh_node.h"
#include "bucket.h"
#include <vector>
#include <algorithm>
#include <thread>
#include <functional>  
#include <memory>
#include "rect.h"
#include "lights.h"
#include "transforms.h"
#include "volumetrics.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define _CRTDBG_MAP_ALLOC 

extern "C" double drand48();
void processBuckets(threadsafeStack<bucket*> *quene, int* output, camera &cam, hitable* world, const int &ns,
    float &progress, float progressIncr, std::mutex &progressMutex) {
    while (!quene->empty()) {
        try {
            bucket* process = quene->pop();
            process->sampler(cam, world, ns);
            process->outputToArray(output);
            delete process;
        }
        catch(emptyStack) {
            break;
        }
        // increment and output progress
        progressMutex.lock();
        progress++;
        std::cout << "Rendering: " << progressIncr * progress << "%\n";
        progressMutex.unlock();
    }
}
#ifdef _WIN64
void process(threadsafeStack<bucket*> *quene, int* output, camera &cam, hitable* world, const int &ns, int numOfBuckets) {
    // +1 process to fully load system
    unsigned long const hardwareThreads = std::thread::hardware_concurrency() ;
    std::cout << "Rendering using: " << hardwareThreads << " threads\n";
    // progress calculation stuff
    float progressIncr = 100.0f / numOfBuckets;
    float progress = 0;
    std::mutex m;
    //setup and run threads
    std::vector<std::thread> threads(hardwareThreads - 1);
    for (unsigned long i = 0; i < hardwareThreads - 1; ++i) {
        threads[i] = std::thread(processBuckets, quene, std::ref(output), cam, world, ns, std::ref(progress), progressIncr, std::ref(m));
    }
    std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
}
#else
void process(threadsafeStack<bucket*> *quene, int* output, camera &cam, hitable* world, const int &ns, int numOfBuckets) {
    float progressIncr = 100.0f / numOfBuckets;
    float progress = 0;
    std::mutex m;
    processBuckets(quene, std::ref(output), cam, world, ns, std::ref(progress), progressIncr, std::ref(m));
}
#endif

hitable *two_speheres() {
    texture* checker(new checker_texture(new constant_texture(vec3(0.2, 0.3, 0.1)), new constant_texture(vec3(0.9, 0.9, 0.9))));

    int n = 50;
    hitable **list = new hitable*[n + 1];
    list[0] = new sphere(vec3(0, -10, 0), 10, new lambertian(checker));
    list[1] = new sphere(vec3(0, 10, 0), 10, new lambertian(checker));
    hitable* out = new bvh_node(list, 2, 0.0, 1.0);
    delete[] list;
    return out;
}
camera two_speheres_cam(double nx, double ny) {
    vec3 lookfrom(25, 5, 10);
    vec3 lookat(0, 2.5, 0);
    double dist_to_focus = 10.0;
    double aperture = 0.0;
    return camera(lookfrom, lookat, vec3(0, 1, 0), 20, double(nx) / double(ny), aperture, dist_to_focus, 0.0, 1.0);
   
}
hitable *random_scene() {
    int nx, ny, nn;
    unsigned char *texData = stbi_load("earth.jpg", &nx, &ny, &nn, 0);
    texture* perText = new noise_texture(8,vec3(0.2, 0.4, 0.4));
    texture* checker = (new checker_texture(new constant_texture(vec3(0.1, 0.1, 0.1)), new constant_texture(vec3(0.9, 0.9, 0.9))));
    int n = 500;
    hitable **list = new hitable*[n + 1];
    list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(checker));
    int i = 1;
    for (int a = -11; a < 11;a++) {
        for (int b = -11;b < 11;b++) {
            double choose_mat = drand48();
            vec3 center(a + 0.9*drand48(), 0.2, b + 0.9*drand48());
            if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
                if (choose_mat < 0.8) {
                    list[i++] = new moving_sphere(center, center+vec3(0,0.5*drand48(),0),0.1,1.0, 0.2, 
                        new lambertian(new constant_texture(vec3(drand48()*drand48(), drand48()*drand48(), drand48()*drand48()))));
                }
                else if (choose_mat < 0.95) {
                    list[i++] = new sphere(center, 0.2, new metal(perText, drand48()));
                    //list[i++] = new sphere(center, 0.2, new metal(vec3(0.5*(1 + drand48()), 0.5*(1 + drand48()), 0.5*(1 + drand48())), drand48()));
                }
                else {
                    list[i++] = new sphere(center, 0.2, new dielectric(1.5));
                }
            }
        }
    }
    list[i++] = new sphere(vec3(0, 1, 0), 1, new dielectric(1.5));
    list[i++] = new sphere(vec3(-4, 1, 0), 1, new lambertian(new image_texture(texData, nx, ny)));
    list[i++] = new sphere(vec3(4, 1, 0), 1, new metal(perText, 0.05));
    hitable* out = new bvh_node(list, i, 0.0, 1.0);
    delete[] list;
    return out;
    
}
camera random_scene_cam(double nx, double ny) {
    vec3 lookfrom(13, 2, 3);
    vec3 lookat(0, 0, 0);
    double dist_to_focus = 10.0;
    double aperture = 0.1;
    return camera(lookfrom, lookat, vec3(0, 1, 0), 20, double(nx) / double(ny), aperture, dist_to_focus, 0.0, 1.0);
}
hitable *two_speheres_2() {
    int nx, ny, nn;
    unsigned char *texData = stbi_load("earth.jpg", &nx, &ny, &nn,0);
    texture* perText = new noise_texture(4, vec3(0.4, 0.2, 0.1));

    int n = 2;
    hitable **list = new hitable*[n];
    list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(new image_texture(texData, nx, ny)));
    list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(new image_texture(texData, nx, ny)));
    hitable* out = new bvh_node(list, 2, 0.0, 1.0);
    delete[] list;
    return out;
}
hitable *simple_light() {
    texture *pertext = new noise_texture(4);
    hitable **list = new hitable*[4];
    list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(pertext));
    list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(pertext));
    list[2] = new sphere(vec3(0, 7, 0), 2, new diffuse_light(new constant_texture(vec3(4, 4, 4))));
    list[3] = new xy_rect(3, 5, 1, 3, -2, new diffuse_light(new constant_texture(vec3(4, 4, 4))));
    hitable* out = new bvh_node(list, 4, 0.0, 1.0);
    delete[] list;
    return out;
}
camera cornell_cam(double nx, double ny) {
    vec3 lookfrom(278, 278, -800);
    vec3 lookat(278, 278, 0);
    double dist_to_focus = 10.0;
    double aperture = 0.0;
    double vfov = 40.0;
    return camera(lookfrom, lookat, vec3(0, 1, 0), vfov, double(nx) / double(ny), aperture, dist_to_focus, 0.0, 1.0);

}
hitable *cornell_box() {
    hitable **list = new hitable*[20];
    int i = 0;
    material *red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
    material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
    material *green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
    material *light = new diffuse_light(new constant_texture(vec3(15, 15, 15)));
    list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, red));
    list[i++] = new yz_rect(0, 555, 0, 555, 0, green);
    list[i++] = new xz_rect(213, 343, 227, 332, 554, light);
    list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
    list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
    list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));
    list[i++] = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 165, 165), white),-18),vec3(130,0,65));
    list[i++] = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 330, 165), white), 15), vec3(265, 0, 295));
    hitable* out = new bvh_node(list, i, 0.0, 1.0);
    delete[] list;
    return out;
}
hitable *cornell_box_smoke() {
    hitable **list = new hitable*[20];
    int i = 0;
    texture *noise = new noise_texture(2);
    material *red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
    material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
    material *green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
    material *light = new diffuse_light(new constant_texture(vec3(6, 6, 6)));
    list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, red));
    list[i++] = new yz_rect(0, 555, 0, 555, 0, green);
    list[i++] = new xz_rect(113, 443, 127, 432, 554, light);
    list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
    list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
    list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));
    hitable* b1= new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 165, 165), white), -18), vec3(130, 0, 65));
    hitable* b2 = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 330, 165), white), 15), vec3(265, 0, 295));
    list[i++] = new nonuniform_medium(b1, noise, 0.01, noise);
    list[i++] = new constant_medium(b2, 0.01, new constant_texture(vec3(1.0, 1.0, 1.0)));
    hitable* out = new bvh_node(list, i, 0.0, 1.0);
    delete[] list;
    return out;
}
hitable *transform_test() {
    hitable **list = new hitable*[20];
    int i = 0;
    texture *noise = new noise_texture(0.5);
    
    material *light = new diffuse_light(new constant_texture(vec3(15, 15, 15)));
    material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
    hitable* b1 = new rotate_x(new box(vec3(-10, -10, -10), vec3(10, 10, 10), new lambertian(noise)), -45);
    list[i++] = new xz_rect(-20,20 , -20, 20, 50, light);
    //list[i++] = new rotate_x(new box(vec3(-10, -10, -10), vec3(10, 10, 10), new lambertian(noise)),45);
    list[i++] = new flip_normals(new xz_rect(-20, 20, -20, 20, -75, light));
    //list[i++] = new constant_medium(b1, 0.01, new constant_texture(vec3(1.0, 1.0, 1.0)));
    list[i++] = new nonuniform_medium(b1, noise, 0.08, new constant_texture(vec3(1.0, 1.0, 1.0)));
    hitable* out = new bvh_node(list, i, 0.0, 1.0);
    delete[] list;
    return out;
}
camera transform_test_cam(double nx, double ny) {
    vec3 lookfrom(0, 30, -50);
    vec3 lookat(0, 0, 0);
    double dist_to_focus = 10.0;
    double aperture = 0.0;
    double vfov = 40.0;
    return camera(lookfrom, lookat, vec3(0, 1, 0), vfov, double(nx) / double(ny), aperture, dist_to_focus, 0.0, 1.0);
}
hitable *final() {
    int nb = 20;
    hitable **list = new hitable*[30];
    hitable **boxlist = new hitable*[10000];
    hitable **boxlist2 = new hitable*[10000];
    material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
    material *ground = new lambertian(new constant_texture(vec3(0.48, 0.83, 0.53)));
    texture *noise = new noise_texture(0.0001);
    int b = 0;
    for (int i = 0; i < nb; i++) {
        for (int j = 0; j < nb; j++) {
            float w = 100;
            float x0 = -1000 + i*w;
            float z0 = -1000 + j*w;
            float y0 = 0;
            float x1 = x0 + w;
            float y1 = 100 * (drand48() + 0.01);
            float z1 = z0 + w;
            boxlist[b++] = new box(vec3(x0, y0, z0), vec3(x1, y1, z1), ground);
        }
    }
    int l = 0;
    list[l++] = new bvh_node(boxlist, b, 0, 1);
    material *light = new diffuse_light(new constant_texture(vec3(7, 7, 7)));
    list[l++] = new xz_rect(123, 423, 147, 412, 554, light);
    vec3 center(400, 400, 200);
    list[l++] = new moving_sphere(center, center + vec3(30, 0, 0), 0, 1, 50, new lambertian(new constant_texture(vec3(0.7, 0.3, 0.1))));
    list[l++] = new sphere(vec3(260, 150, 45), 50, new dielectric(1.5));
    list[l++] = new sphere(vec3(0, 150, 145), 50, new metal(new constant_texture(vec3(0.8, 0.8, 0.9)), 10.0));
    hitable *boundary = new sphere(vec3(360, 150, 145), 70, new dielectric(1.5));
    list[l++] = new sphere(vec3(360, 150, 145), 70, new dielectric(1.5));
    list[l++] = new constant_medium(boundary, 0.2, new constant_texture(vec3(0.2, 0.4, 0.9)));
    boundary = new sphere(vec3(0, 0, 0), 5000, new dielectric(1.5));
    list[l++] = new nonuniform_medium(boundary, noise, 0.0015, new constant_texture(vec3(1.0, 1.0, 1.0)));
    int nx, ny, nn;
    unsigned char *tex_data = stbi_load("earth.jpg", &nx, &ny, &nn, 0);
    material *emat = new lambertian(new image_texture(tex_data, nx, ny));
    list[l++] = new sphere(vec3(400, 200, 400), 100, emat);
    texture *pertext = new noise_texture(0.1);
    list[l++] = new sphere(vec3(220, 280, 300), 80, new lambertian(pertext));
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        boxlist2[j] = new sphere(vec3(165 * drand48(), 165 * drand48(), 165 * drand48()), 10, white);
    }
    list[l++] = new translate(new rotate_y(new bvh_node(boxlist2, ns, 0.0, 1.0), 15), vec3(-100, 270, 395));
    hitable* out = new bvh_node(list, l, 0.0, 1.0);
    delete[] list;
    delete[] boxlist;
    delete[] boxlist2;
    return out;
}
camera final(double nx, double ny) {
    vec3 lookfrom(478, 278, -600);
    vec3 lookat(278, 278, 0);
    float dist_to_focus = 10.0;
    float aperture = 0.0;
    float vfov = 40.0;
    return camera(lookfrom, lookat, vec3(0, 1, 0), vfov, double(nx) / double(ny), aperture, dist_to_focus, 0.0, 1.0);
}
int main()
{
    // open output file
    const char* fileName = "test37.ppm";
    std::ofstream fOut(fileName);
    if (!fOut.is_open()) {
        std::cerr << "Couldn`t open file" << fileName << "\n";
        exit(1);
    }
    // start time
    clock_t start, finish;
    start = clock();
    // setup scene
    const int nx = 800;
    const int ny = 800;
    int ns = 1;
    int nBucketsX = 8;
    int nBucketsY = 8;
    hitable* world = transform_test();
    camera cam = final(nx,ny);

    // declare and initialize output array;
    // initialization is not really nesesary but lets follow 
    // safety rules
    int *output = new int[nx*ny * 3];
    std::memset(output, 0, sizeof(output));
    // create and render buckets
    threadsafeStack<bucket*>  * quene = bucket::createBuckets(nx, ny, nBucketsX,nBucketsY);
    // number of buckets passed for progress calculation
    process(quene, output, cam, world, ns, nBucketsX*nBucketsY);
    // output header and pixel array to file
    fOut << "P3\n" << nx << " " << ny << "\n255\n";
    for (int i = 0; i < nx*ny*3; ++i) {
        fOut << output[i] << " ";
    }
    //render time calculate and output
    finish = clock();
    std::cout <<"Render Time " << (finish - start) / CLOCKS_PER_SEC << " seconds\n" ,
    // clean up
    fOut.close();
    delete quene;
    delete world;
    delete[] output;
    // leaks check
    _CrtDumpMemoryLeaks(); 
    // to keep console open after render complete;
    system("pause");
    return 0;  
}
