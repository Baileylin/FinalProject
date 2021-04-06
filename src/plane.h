#ifndef PLANE_H_
#define PLANE_H_

#include "hittable.h"
#include "AGLM.h"

class plane : public hittable {
public:
   plane() : a(0), n(0), mat_ptr(0) {}
   plane(const glm::point3& p, const glm::vec3& normal, 
      std::shared_ptr<material> m) : a(p), n(normal), mat_ptr(m) {};

   virtual bool hit(const ray& r, hit_record& rec) const override
   {
      
      float numerator = glm::dot(glm::vec3(a - r.origin()), n);
      float denominator = glm::dot(r.direction(), n);
      if (fabs(denominator) > 0.0001)
      {
          float t = numerator / denominator;
          rec.t = t; // save the time when we hit the object
          rec.p = r.at(t); // ray.origin + t * ray.direction
          rec.mat_ptr = mat_ptr;

          glm::vec3 outward_normal = normalize(n); // compute unit length normal
          rec.set_face_normal(r, outward_normal);
          return (t >= 0);
      }
      return false;
   }

public:
   glm::vec3 a;
   glm::vec3 n;
   std::shared_ptr<material> mat_ptr;
};

#endif
