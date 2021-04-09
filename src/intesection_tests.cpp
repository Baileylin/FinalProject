#include <cassert>
#include "AGLM.h"
#include "material.h"
#include "ray.h"
#include "sphere.h"
#include "box.h"
#include "plane.h"
#include "triangle.h"
#include "hittable.h"

using namespace glm;
using namespace std;

float eps = 0.0001f; // epsilon for comparing floats

template <class T>
bool vecEquals(T a, T b)
{
   return all(epsilonEqual(a, b, eps));
}

template <class T>
bool equals(T a, T b)
{
   return abs<T>(a - b) < eps;
}

void check(bool val, const std::string& message, const hit_record& hit, const ray& ray)
{
   if (!val)
   {
      cout << message << endl;
      cout << hit.str() << endl;
      cout << ray.str() << endl;
   }
   assert(val);
}

void test_sphere(const sphere& s, const ray& r, bool hits, const hit_record& desired) {
   hit_record hit;
   bool result = s.hit(r, hit);

   check(result == hits, "error: ray should hit", hit, r);
   if (hits) {
      check(vecEquals(hit.p, desired.p), "error: position incorrect:", hit, r);
      check(vecEquals(hit.normal, desired.normal), "error: normal incorrect:", hit, r);
      check(equals(hit.t, desired.t), "error: hit time incorrect", hit, r);
      check(hit.front_face == desired.front_face, "error: front facing incorrect", hit, r);
   }
}


void test_plane(const plane& s, const ray& r, bool hits, const hit_record& desired) {
    hit_record hit;
    bool result = s.hit(r, hit);

    check(result == hits, "error: ray should hit", hit, r);
    if (hits) {
        check(vecEquals(hit.p, desired.p), "error: position incorrect:", hit, r);
        check(vecEquals(hit.normal, desired.normal), "error: normal incorrect:", hit, r);
        check(equals(hit.t, desired.t), "error: hit time incorrect", hit, r);
        check(hit.front_face == desired.front_face, "error: front facing incorrect", hit, r);
    }
}

void test_triangle(const triangle& s, const ray& r, bool hits, const hit_record& desired) {
    hit_record hit;
    bool result = s.hit(r, hit);

    check(result == hits, "error: ray should hit", hit, r);
    if (hits) {
        check(vecEquals(hit.p, desired.p), "error: position incorrect:", hit, r);
        check(vecEquals(hit.normal, desired.normal), "error: normal incorrect:", hit, r);
        check(equals(hit.t, desired.t), "error: hit time incorrect", hit, r);
        check(hit.front_face == desired.front_face, "error: front facing incorrect", hit, r);
    }
}

int main(int argc, char** argv)
{
   shared_ptr<material> empty = 0; 
   hit_record none = hit_record{ point3(0), point3(0), -1.0f, false, empty};

   sphere s(point3(0), 2.0f, empty);
   test_sphere(s, 
               ray(point3(0, 0, 3), vec3(0, 0, -1)), // ray outside/towards sphere
               true, 
               hit_record{vec3(0,0,2), vec3(0,0,1), 1, true, empty}); 

   test_sphere(s, 
               ray(point3(0, 0, 0), vec3(0, 0, -1)), // ray inside sphere
               true, 
               hit_record{ vec3(0,0,-2), vec3(0,0,1), 2, false, empty}); 

   test_sphere(s, 
               ray(point3(0, 0, 3), vec3(0, 0, 1)), // ray outside/away sphere
               false, 
               none); 

   test_sphere(s, 
               ray(point3(0, 0, 3), vec3(0, 5,-1)), // ray outside/towards sphere (miss)
               false, 
               none); 

   test_sphere(s, 
               ray(point3(0, 0, 3), vec3(0, 1,-3)), // ray outside/towards sphere (hit)
               true, 
               hit_record{vec3(0,0.3432f, 1.9703f), vec3(0,0.1716f, 0.9851f), 0.3432f, true, empty}); 
     


   /*************Tests for planes*************/
   plane newPlane(point3(0), vec3(0.0, 0.0, 1.0), empty);
   test_plane(newPlane,
              ray(point3(0, 0, 3), vec3(0, 0, -1)), //A ray outside the plane which hits the plane
              true,
              hit_record{ vec3(0,0,0), vec3(0,0,1), 3.0f, true, empty });

   test_plane(newPlane,
              ray(point3(0, 0, 0), vec3(1, 0, 0)), // ray inside the plane
              false,
              none);

   test_plane(newPlane,
              ray(point3(0, 0, 3), vec3(0, 0, 1)), // A ray outside, pointing away from the plane(misses)
              false,
              none);

   /*************Tests for triangles*************/
   triangle newTriangle(point3(2.0, -2.0, 0.0), point3(0.0, 2.0, 0.0), point3(-2.0, -2.0, 0.0), empty);
   test_triangle(newTriangle,
                 ray(point3(0, 0, 3), vec3(0, 0, -1)), //A ray outside the triangle which hits the triangle
                 true,
                 hit_record{ vec3(0,0,0), vec3(0,0,1), 3.0f, true, empty });

   test_triangle(newTriangle,
                 ray(point3(0, 0, 3), vec3(0, 0, 11)), // A ray outside, pointing away from the triangle(misses)
                 false,
                 none);

   test_triangle(newTriangle,
                 ray(point3(0, 0, 10), vec3(1, 0, -1)), //A ray outside, pointing towards the triangle that misses
                 false,
                 none);

   test_triangle(newTriangle,
                 ray(point3(0, 0, 0), vec3(0, 1, 0)), //A ray inside the triangle (hits). For this case, the algorithm doesn't recognize triangle as a shape
                 false,
                 none);
}
