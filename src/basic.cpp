// Raytracer framework from https://raytracing.github.io by Peter Shirley, 2018-2020
// alinen 2021, modified to use glm and ppm_image class

#include "ppm_image.h"
#include "AGLM.h"
#include "ray.h"
#include "sphere.h"
#include "camera.h"
#include "plane.h"
#include "triangle.h"
#include "material.h"
#include "hittable_list.h"

using namespace glm;
using namespace agl;
using namespace std;

//this method is used to implement the original basic.png
color ray_color(const ray& r, const hittable_list& world, int depth)
{
   hit_record rec;
   if (depth <= 0)
   {
      return color(0);
   }

   if (world.hit(r, 0.001f, infinity, rec))
   {
      ray scattered;
      color attenuation;
      if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
      {
         color recurseColor = ray_color(scattered, world, depth - 1);
         return attenuation * recurseColor;
      }
      return attenuation;
   }
   vec3 unit_direction = normalize(r.direction());
   auto t = 0.5f * (unit_direction.y + 1.0f);
   return (1.0f - t) * color(1, 1, 1) + t * color(0.5f, 0.7f, 1.0f);
}


//Monte Carlo Path Tracing
void pathTrace(ppm_image& image)
{
    int height = image.height();
    int width = image.width();
    float aspect = width / float(height);
    int samples_per_pixel = 10; // higher => more anti-aliasing
    int max_depth = 10; // higher => less shadow acne

    // World
    shared_ptr<material> gray = make_shared<lambertian>(color(0.5f));

    hittable_list world;
    world.add(make_shared<sphere>(point3(0, 0, -1), 0.5f, gray));
    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100, gray));

    // Camera
    vec3 camera_pos(0);
    float viewport_height = 2.0f;
    float focal_length = 1.0;
    camera cam1(camera_pos, viewport_height, aspect, focal_length);
    camera cam2(point3(-2, 2, 1), point3(0, 0, -1), vec3(0, 1, 0), 90, aspect);

    // Ray trace
    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            color c(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; s++) // antialias
            {
                float u = float(i + random_float()) / (width - 1);
                float v = float(height - j - 1 - random_float()) / (height - 1);

                ray r = cam1.get_ray(u, v);
                c += ray_color(r, world, max_depth);
            }
            c = normalize_color(c, samples_per_pixel);
            image.set_vec3(j, i, c);
        }
    }

    image.save("basic.png");
}

color normalize_color(const color& c, int samples_per_pixel)
{
    // todo: implement me!
    float scale = 1.0f / samples_per_pixel;
    float r = std::min(0.999f, std::max(0.0f, c.r * scale));
    float g = std::min(0.999f, std::max(0.0f, c.g * scale));
    float b = std::min(0.999f, std::max(0.0f, c.b * scale));

    // apply gamma correction 
    r = sqrt(r);
    g = sqrt(g);
    b = sqrt(b);

    return color(r, g, b);
}
/*
//this method is used to implement the unique image. Uncomment this method and comment the above ray_color() method to implement the unique image
color ray_color(const ray& r, const hittable_list& world, int depth)
{
   hit_record rec;
   if (depth <= 0)
   {
      return color(0);
   }

   if (world.hit(r, 0.001f, infinity, rec))
   {
      ray scattered;
      color attenuation;
      if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
      {
         color recurseColor = ray_color(scattered, world, depth - 1);
         return attenuation * recurseColor;
      }
      return attenuation;
   }
   vec3 unit_direction = normalize(r.direction());
   auto t = 0.5f * (unit_direction.y + 1.0f);
   return (1.0f - t) * color(1, 1, 1) + t * color(0.8, 0.3f, 0.4f);
}*/



/*
//this method is used to implement the unique image. Uncomment this method and comment the below ray_trace() method to implement the unique image
void ray_trace(ppm_image& image)
{
    // Image
    int height = image.height();
    int width = image.width();
    float aspect = width / float(height);
    int samples_per_pixel = 10; // higher => more anti-aliasing
    int max_depth = 10; // higher => less shadow acne

    // Camera
    vec3 camera_pos(0, 5, 20);
    float viewport_height = 2.0f;
    float focal_length = 4;
    camera cam(camera_pos, viewport_height, aspect, focal_length);
    //camera cam(point3(3, -3, 0), point3(-3, 3, 1), point3(0.0, 1.0, 0.0), 90, aspect);

    // World
    shared_ptr<material> metal_red = make_shared<metal>(color(1.0, 0.0, 0.0), 0.3f);
    shared_ptr<material> metal_blue = make_shared<metal>(color(0.254, 0.901, 0.768), 0.8f);
    shared_ptr<material> metal_green = make_shared<metal>(color(0.411, 0.913, 0.305), 0.1f);
    shared_ptr<material> metal_purple = make_shared<metal>(color(0.431, 0.152, 0.725), 1.0f);
    shared_ptr<material> phong_blue = make_shared<phong>(camera_pos);
    shared_ptr<material> lambertian_gray = make_shared<lambertian>(color(0.5f));
    shared_ptr<material> lambertian_yellow = make_shared<lambertian>(color(0.913, 0.882, 0.305));
    shared_ptr<material> lambertian_pink = make_shared<lambertian>(color(0.913, 0.305, 0.847));
    shared_ptr<material> dielectric_ = make_shared<dielectric>(0.7f);

    hittable_list world;

    //draw a tetrahedron
    world.add(make_shared<triangle>(point3(1, 1, 1), point3(8, 1, 0), point3(3.5, 3, 7), phong_blue));
    world.add(make_shared<triangle>(point3(1, 1, 1), point3(3.5, 3, 7), point3(3, 8, 4.5), metal_blue));
    world.add(make_shared<triangle>(point3(3, 8, 4.5), point3(3.5, 3, 7), point3(8, 1, 0), metal_red));
    world.add(make_shared<triangle>(point3(8, 1, 0), point3(3, 8, 4.5), point3(1, 1, 1), metal_red));

    //draw two planes
    world.add(make_shared<plane>(point3(0, 0, -10), vec3(1, 1, 1), lambertian_gray));
    world.add(make_shared<plane>(point3(0, 0, -10), vec3(-1, 1, 1), lambertian_yellow));

    //draw five spheres with four materials (snowflake pattern)
    world.add(make_shared<sphere>(point3(-5, 5, 0), 0.6f, phong_blue));
    world.add(make_shared<sphere>(point3(-7, 5, 0), 0.6f, dielectric_));
    world.add(make_shared<sphere>(point3(-5, 7, 0), 0.6f, metal_green));
    world.add(make_shared<sphere>(point3(-3, 5, 0), 0.6f, lambertian_pink));
    world.add(make_shared<sphere>(point3(-5, 3, 0), 0.6f, metal_purple));


    // Ray trace
    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            color c(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; s++) // antialias
            {
                float u = float(i + random_float()) / (width - 1);
                float v = float(height - j - 1 - random_float()) / (height - 1);

                ray r = cam.get_ray(u, v);
                c += ray_color(r, world, max_depth);
            }
            c = normalize_color(c, samples_per_pixel);
            image.set_vec3(j, i, c);
        }
    }
    image.save("unique_image.png");
}*/



//this method is used to implement the original basic.png
/*void ray_trace(ppm_image& image)
{
   // Image
   int height = image.height();
   int width = image.width();
   float aspect = width / float(height);
   int samples_per_pixel = 10; // higher => more anti-aliasing
   int max_depth = 10; // higher => less shadow acne

   // World
   shared_ptr<material> gray = make_shared<lambertian>(color(0.5f));

   hittable_list world;
   world.add(make_shared<sphere>(point3(0, 0, -1), 0.5f, gray));
   world.add(make_shared<sphere>(point3(0, -100.5, -1), 100, gray));

   // Camera
   vec3 camera_pos(0);
   float viewport_height = 2.0f;
   float focal_length = 1.0; 
   camera cam1(camera_pos, viewport_height, aspect, focal_length);
   camera cam2(point3(-2, 2, 1), point3(0, 0, -1), vec3(0, 1, 0), 90, aspect);

   // Ray trace
   for (int j = 0; j < height; j++)
   {
      for (int i = 0; i < width; i++)
      {
         color c(0, 0, 0);
         for (int s = 0; s < samples_per_pixel; s++) // antialias
         {
            float u = float(i + random_float()) / (width - 1);
            float v = float(height - j - 1 - random_float()) / (height - 1);

            ray r = cam1.get_ray(u, v);
            c += ray_color(r, world, max_depth);
         }
         c = normalize_color(c, samples_per_pixel);
         image.set_vec3(j, i, c);
      }
   }

   image.save("basic.png");
}*/


