#ifndef MATERIAL_H
#define MATERIAL_H

#include <cmath>
#include "AGLM.h"
#include "ray.h"
#include "hittable.h"
#include "texture.h"

class material {
public:
    virtual glm::color emitted(double u, double v, const glm::point3& p) const {
        return glm::color(0, 0, 0);
    }
    virtual bool scatter(const ray& r_in, const hit_record& rec, glm::color& attenuation, ray& scattered) const = 0;
    virtual ~material() {}
};

class emit_light : public material 
{
public:
    emit_light(shared_ptr<texture> a) : emit(a) {}
    emit_light(glm::color c) : emit(make_shared<constant_texture>(c)) {}

    virtual bool scatter(const ray& r_in, const hit_record& rec, glm::color& attenuation, ray& scattered) const override 
    {
        return false;
    }

    virtual glm::color emitted(double u, double v, const glm::point3& p) const override {
        return emit->value(u, v, p);
    }

public:
    shared_ptr<texture> emit;
};

class lambertian : public material {
public:
    lambertian(const glm::color& a) : albedo(make_shared<constant_texture>(a)) {}
    lambertian(shared_ptr<texture> a) : albedo(a) {}

  virtual bool scatter(const ray& r_in, const hit_record& rec, 
     glm::color& attenuation, ray& scattered) const override 
  {
      using namespace glm;
      vec3 scatter_direction = rec.normal + random_unit_vector();
      if (near_zero(scatter_direction)) 
      {
          scatter_direction = rec.normal;
      }
      scattered = ray(rec.p, scatter_direction, r_in.getTime());
      attenuation = albedo->value(rec.u, rec.v, rec.p);
      return true;
  }

public:
    shared_ptr<texture> albedo;
};

class phong : public material {
public:
  phong(const glm::vec3& view) :
     diffuseColor(0,0,1), 
     specColor(1,1,1),
     ambientColor(.01f, .01f, .01f),
     lightPos(5,5,0),
     viewPos(view), 
     kd(0.45), ks(0.45), ka(0.1), shininess(10.0) 
  {}

  phong(const glm::color& idiffuseColor, 
        const glm::color& ispecColor,
        const glm::color& iambientColor,
        const glm::point3& ilightPos, 
        const glm::point3& iviewPos, 
        float ikd, float iks, float ika, float ishininess) : 
     diffuseColor(idiffuseColor), 
     specColor(ispecColor),
     ambientColor(iambientColor),
     lightPos(ilightPos),
     viewPos(iviewPos), kd(ikd), ks(iks), ka(ika), shininess(ishininess) 
  {}

  virtual bool scatter(const ray& r_in, const hit_record& hit, 
     glm::color& attenuation, ray& scattered) const override 
  {
      glm::color Ia = ka * ambientColor;

      glm::color light_color(1.0f, 1.0f, 1.0f);
      glm::color Id = kd * glm::max(0.0f, glm::dot(normalize(lightPos), normalize(hit.normal))) * diffuseColor* light_color;

      float product = glm::max(0.0f, dot(normalize(viewPos), normalize(reflect(-lightPos, hit.normal))));
      glm::color Is = ks * specColor * (float)pow(product, shininess);

      attenuation = Ia + Id+ Is;
      return false;
  }

public:
  glm::color diffuseColor;
  glm::color specColor;
  glm::color ambientColor;
  glm::point3 lightPos;
  glm::point3 viewPos; 
  float kd; 
  float ks;
  float ka; 
  float shininess;
};

class metal : public material {
public:
   metal(const glm::color& a, float f) : albedo(a), fuzz(glm::clamp(f,0.0f,1.0f)) {}

   virtual bool scatter(const ray& r_in, const hit_record& rec, 
      glm::color& attenuation, ray& scattered) const override 
   {
       glm::vec3 reflected = glm::reflect(glm::normalize(r_in.direction()), rec.normal);
       scattered = ray(rec.p, reflected + fuzz * random_unit_vector(), r_in.getTime());
       attenuation = albedo;
       return (dot(scattered.direction(), rec.normal) > 0);
   }

public:
   glm::color albedo;
   float fuzz;
};

class dielectric : public material {
public:
  dielectric(float index_of_refraction) : ir(index_of_refraction) {}

  virtual bool scatter(const ray& r_in, const hit_record& rec, 
     glm::color& attenuation, ray& scattered) const override 
   {
      attenuation = glm::color(1.0, 1.0, 1.0);
      float refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

      glm::vec3 unit_direction = glm::normalize((r_in.direction()));
      float cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
      float sin_theta = sqrt(1.0 - cos_theta * cos_theta);

      bool cannot_refract = refraction_ratio * sin_theta > 1.0;
      glm::vec3 direction;

      if (cannot_refract)
          direction = glm::reflect(unit_direction, rec.normal);
      else
      {
          glm::vec3 r_out_perp = refraction_ratio * (unit_direction + cos_theta * rec.normal);
          glm::vec3 r_out_parallel = (float)-sqrt(fabs(1.0 - length(r_out_perp)*length(r_out_perp))) * rec.normal;
          direction = r_out_perp + r_out_parallel;
      }
      scattered = ray(rec.p, direction, r_in.getTime());
      return true;
   }

public:
  float ir; // Index of Refraction
};


#endif

