// ray.h, from https://raytracing.github.io by Peter Shirley, 2018-2020
#ifndef MOVING_SPHERE_H
#define MOVING_SPHERE_H

#include "hittable.h"
#include "AGLM.h"
#include <cmath>

class moving_sphere : public hittable {
public:
    moving_sphere() {}
    moving_sphere(glm::point3 cen0, glm::point3 cen1, double _time0, double _time1, double r, std::shared_ptr<material> m)
        : center0(cen0), center1(cen1), time0(_time0), time1(_time1), radius(r), mat_ptr(m)
    {};

    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override;
    glm::point3 center(float time) const;

public:
   glm::point3 center0, center1;
   float radius, time0, time1;
   std::shared_ptr<material> mat_ptr;

private:
    static void get_uv_coordinates(const glm::point3& p, float& u, float& v) {
        const float pi = glm::pi<float>();
        float theta = acos(-p.y);
        float phi = atan2(-p.z, p.x) + pi;
        u = phi / (2 * pi);
        v = theta / pi;
    }
};

glm::point3 moving_sphere::center(float time) const {
    return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0);
}

bool moving_sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    glm::vec3 oc = r.origin() - center(r.getTime());
    float a = glm::dot(r.direction(), r.direction());
    float half_b = glm::dot(oc, r.direction());
    float c = glm::length2(oc) - radius*radius;
    
    float discriminant = half_b*half_b - a*c;
    if (discriminant < 0) return false;
    float sqrtd = sqrt(discriminant);
    
    float t = (-half_b - sqrtd) / a;
    if (t < t_min || t_max < t) {
        t = (-half_b + sqrtd) / a;
        if (t < t_min || t_max < t)
            return false;
    }
    
       // save relevant data in hit record
    rec.t = t; // save the time when we hit the object
    rec.p = r.at(t); // ray.origin + t * ray.direction
    rec.mat_ptr = mat_ptr; 
    
       // save normal
    glm::vec3 outward_normal = normalize(rec.p - center(r.getTime())); // compute unit length normal
    rec.set_face_normal(r, outward_normal);
    
    return true;
}

#endif

