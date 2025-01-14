#ifndef SPHERE_H
#define SPHERE_H

#include "hitable.hpp"
#include "material.hpp"
#include "ray.hpp"
#include "vec3.hpp"

#include <cmath>

class sphere: public hitable {
  private:
    vec3 center;
    float radius;
    material *mat_ptr;

  public:
    sphere() {}
    sphere(vec3 cen, float r, material *mat_ptr)
        : center(cen), radius(r), mat_ptr(mat_ptr){};
    virtual bool hit(const ray &r, float t_min, float t_max,
                     hit_record &rec) const;
};

bool sphere::hit(const ray &r, float t_min, float t_max,
                 hit_record &rec) const {
    vec3 oc = r.origin() - center;
    float a = dot(r.direction(), r.direction());
    float b = dot(oc, r.direction());
    float c = dot(oc, oc) - radius * radius;
    float discriminant = b * b - a * c;
    if(discriminant > 0) {
        float temp = (-b - std::sqrt(b * b - a * c)) / a;
        if(temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center) / radius;
            rec.mat_ptr = mat_ptr;
            return true;
        }
        temp = (-b + std::sqrt(b * b - a * c)) / a;
        if(temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center) / radius;
            rec.mat_ptr = mat_ptr;
            return true;
        }
    }
    return false;
}

#endif
