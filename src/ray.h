// ray.h, from https://raytracing.github.io by Peter Shirley, 2018-2020

#ifndef RAY_H
#define RAY_H

#include "AGLM.h"
#include <sstream>

class ray {
public:
   ray() {}

   ray(const glm::point3& origin, const glm::vec3& direction, float _time =0.0)
      : orig(origin), dir(direction), time(_time)
   {}

   glm::point3 origin() const  { return orig; }
   glm::vec3 direction() const { return dir; }
   float getTime() const { return time; }

   glm::point3 at(float t) const {
      return orig + t*dir;
   }

   std::string str() const {
      std::ostringstream ss;
      ss << "position: " << orig << std::endl;
      ss << "direction: " << dir << std::endl;
      return ss.str();
   }

public:
   glm::point3 orig;
   glm::vec3 dir;
   float time;
};

#endif

