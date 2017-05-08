#ifndef BVHNODE_H
#define BVHNODE_H

#include <cstdlib>
#include <cfloat>
#include <iostream>
#include "aabb.h"
#include "Hitable.h"

extern "C" double drand48();

class bvh_node : public hitable {
public:
    static int box_x_compare(const void *a, const void *b) {
        aabb box_left, box_right;
        hitable *ah = *(hitable**)a;
        hitable *bh = *(hitable**)b;
        if (!ah->bounding_box(0, 0, box_left) || !bh->bounding_box(0, 0, box_right))
            std::cerr << "no bounding box in bvh constructor\n";
        if (box_left.min()[0] - box_right.min()[0] < 0.0)
            return -1;
        else
            return 1;
    }
    static int box_y_compare(const void *a, const void *b) {
        aabb box_left, box_right;
        hitable *ah = *(hitable**)a;
        hitable *bh = *(hitable**)b;
        if (!ah->bounding_box(0, 0, box_left) || !bh->bounding_box(0, 0, box_right))
            std::cerr << "no bounding box in bvh constructor\n";
        if (box_left.min()[1] - box_right.min()[1] < 0.0)
            return -1;
        else
            return 1;
    }
    static int box_z_compare(const void *a, const void *b) {
        aabb box_left, box_right;
        hitable *ah = *(hitable**)a;
        hitable *bh = *(hitable**)b;
        if (!ah->bounding_box(0, 0, box_left) || !bh->bounding_box(0, 0, box_right))
            std::cerr << "no bounding box in bvh constructor\n";
        if (box_left.min()[2] - box_right.min()[2] < 0.0)
            return -1;
        else
            return 1;
    }
    bvh_node() {}
    bvh_node(hitable **l, int n, double time0, double time1);
    ~bvh_node() {
        delete right;
        delete left;
        
    }
    virtual bool hit(const ray& r, const double t_min, const double t_max, hit_record& rec) const;
    virtual bool bounding_box(double t0, double t1, aabb& b) const {
        b = box;
        return true;
    }
    // need proper destructor

    hitable *left;
    hitable *right;
    aabb box;
};
bvh_node::bvh_node(hitable **l, int n, double time0, double time1) {
    aabb *boxes = new aabb[n];
    // calculate and store aabb for all objects
    for (int i = 0; i < n; ++i)
        bool tmp = l[i]->bounding_box(time0, time1, boxes[i]);
    // calculate overal scene aabb
    aabb mainBox = boxes[0];
    for (int i = 1; i < n; ++i)
        mainBox = aabb::surrounding_box(boxes[i], mainBox); 
    // shoose longest axis and sort along it
    int axis = mainBox.longestAxis();
    if (axis == 0)
        qsort(l, n, sizeof(hitable*), box_x_compare);
    else if (axis == 1)
        qsort(l, n, sizeof(hitable*), box_y_compare);
    else
        qsort(l, n, sizeof(hitable*), box_z_compare);
    // calculate areas of hierarchy built aabbs from begin of the sorted list to end
    // each iteration add next object aabb to the previous aabb calculate area of new aabb 
    // and store area in array using sorted object index as area array index.
    double *leftArea = new double[n];
    leftArea[0] = boxes[0].area();
    aabb leftBox = boxes[0];
    for (int i = 1 ;i < n - 1 ; ++i) {
        leftBox = aabb::surrounding_box(leftBox, boxes[i]);
        leftArea[i] = leftBox.area();
    }
    // calculation from end to begining
    double *rightArea = new double[n];
    rightArea[n - 1] = boxes[n - 1].area();
    aabb rightBox = boxes[n - 1];
    for (int i = n - 2 ;i > 0 ; --i) {
        rightBox = aabb::surrounding_box(rightBox, boxes[i]);
        rightArea[i] = rightBox.area();
    }
    // SAH calculation
    double minSAH = DBL_MAX;
    int minSahIdx= n/2;
    for (int i = 0; i < n - 1; ++i) {
        double SAH = i*leftArea[i] + (n - 1 - i)*rightArea[i + 1];
        if (SAH < minSAH) {
            minSahIdx = i;
            minSAH = SAH;
        }
    }
    // build BVH
    if (minSahIdx == 0)
        left = l[0];
    else
        left = new bvh_node(l, minSahIdx + 1, time0, time1);
    if (minSahIdx == n - 2)
        right = l[minSahIdx + 1];
    else
        right = new bvh_node(l + minSahIdx + 1, n - minSahIdx - 1, time0, time1);
    box = mainBox;
    //clean up
    delete[] boxes;
    delete[] leftArea;
    delete[] rightArea;
}
bool bvh_node::hit(const ray& r, const double t_min, const double t_max, hit_record& rec) const {
    if (box.hit(r, t_min, t_max)) {
        hit_record left_rec, right_rec;
        bool hit_left = left->hit(r, t_min, t_max, left_rec);
        bool hit_right = right->hit(r, t_min, t_max, right_rec);
        if (hit_left && hit_right) {
            if (left_rec.t < right_rec.t)
                rec = left_rec;
            else
                rec = right_rec;
            return true;
        }
        else if (hit_left) {
            rec = left_rec;
            return true;
            }
        else if (hit_right) {
            rec = right_rec;
            return true;
            }
        else
            return false;
        }
    else
        return false;
}


#endif // !BVHNODE_H
