// ray.h, from https://raytracing.github.io by Peter Shirley, 2018-2020
#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "AGLM.h"
#include <cmath>

class sphere : public hittable {
public:
   sphere() : radius(0), center(0), mat_ptr(0) {}
   sphere(const glm::point3& cen, float r, std::shared_ptr<material> m) : 
      center(cen), radius(r), mat_ptr(m) {};

   virtual bool hit(const ray& r, hit_record& rec) const override;
 
   

public:
   glm::point3 center;
   float radius;
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

bool sphere::hit(const ray& r, hit_record& rec) const {
    float t = 0;
    float length = glm::length(r.direction());
    glm::vec3 el = center - r.origin();
    glm::vec3 unitDirection = r.direction() / length;
    float s = glm::dot(el, unitDirection);
    float elSqr = glm::dot(el, el);
    float rSqr = radius * radius;

    if (s < 0 && elSqr > rSqr)
        return false;

    float mSqr = elSqr - s * s;
    if (mSqr > rSqr)
        return false;

    float q = sqrt(rSqr - mSqr);
    if (elSqr > rSqr)
        t = s - q;
    else
        t = s + q;

    // save relevant data in hit record
    rec.t = t / length; // save the time when we hit the object
    rec.p = r.at(t / length); // ray.origin + t * ray.direction
    rec.mat_ptr = mat_ptr;

    // save normal
    glm::vec3 outward_normal = normalize(rec.p - center); // compute unit length normal
    rec.set_face_normal(r, outward_normal);
    get_uv_coordinates(outward_normal, rec.u, rec.v);

    return true;
}
//analytical approach
//bool sphere::hit(const ray& r, hit_record& rec) const {
//   glm::vec3 oc = r.origin() - center;
//   float a = glm::dot(r.direction(), r.direction());
//   float half_b = glm::dot(oc, r.direction());
//   float c = glm::length2(oc) - radius*radius;
//
//   float discriminant = half_b*half_b - a*c;
//   if (discriminant < 0) return false;
//   float sqrtd = sqrt(discriminant);
//
//   float t = (-half_b - sqrtd) / a;
//   if (t < 0) t = (-half_b + sqrtd) / a;
//   if (t < 0) return false;
//
//   // save relevant data in hit record
//   rec.t = t; // save the time when we hit the object
//   rec.p = r.at(t); // ray.origin + t * ray.direction
//   rec.mat_ptr = mat_ptr; 
//
//   // save normal
//   glm::vec3 outward_normal = normalize(rec.p - center); // compute unit length normal
//   rec.set_face_normal(r, outward_normal);
//
//   std::cout << t << std::endl;
//   return true;
//}


#endif

