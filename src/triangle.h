#ifndef TRIANGLE_H_
#define TRIANGLE_H_


#include "hittable.h"
#include "AGLM.h"

class triangle : public hittable {
public:
   triangle() : a(0), b(0), c(0), mat_ptr(0) {}
   triangle(const glm::point3& v0, const glm::point3& v1, const glm::point3& v2, 
      std::shared_ptr<material> m) : a(v0), b(v1), c(v2), mat_ptr(m) {};

   virtual bool hit(const ray& r, hit_record& rec) const override
   {
       glm::vec3 e1 = b - a;
       glm::vec3 e2 = c - a;
       glm::vec3 p = glm::cross(r.direction(), e2);
       glm::vec3 normal_vector = glm::cross(e1, e2);
       
       float product = glm::dot(e1, p);
       float product_in_plane = glm::dot(normal_vector, r.direction());
       float eps = 0.0000001;

       if (product_in_plane < eps && product_in_plane >= 0.0) return false;//when the ray is on the same plane with the triangle, a "no hit" will be returned

       if (fabs(product) < eps) return false;
       float f = 1 / product;
       
       glm::vec3 s = r.origin() - a;
       float u = f * (glm::dot(s, p));
       if (u < 0.0 || u > 1.0) return false;

       glm::vec3 q = cross(s, e1);
       float v = f * (glm::dot(r.direction(), q));
       if (v < 0.0 || ((u + v) > 1.0)) return false;

       float t = f * (glm::dot(e2, q));
       if (t < 0.0) return false;

       rec.t = t; // save the time when we hit the object
       rec.p = r.at(t); // ray.origin + t * ray.direction
       rec.mat_ptr = mat_ptr;

       // save normal
       
       glm::vec3 outward_normal = normalize(normal_vector); // compute unit length normal
       rec.set_face_normal(r, outward_normal);

       return true;
   }

public:
   glm::point3 a;
   glm::point3 b;
   glm::point3 c;
   std::shared_ptr<material> mat_ptr;
};

#endif
