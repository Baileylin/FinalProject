// Raytracer framework from https://raytracing.github.io by Peter Shirley, 2018-2020
// alinen 2021, modified to use glm and ppm_image class

#include "ppm_image.h"
#include "AGLM.h"
#include "ray.h"
#include "sphere.h"
#include "triangle.h"
#include "plane.h"
#include "camera.h"
#include "material.h"
#include "hittable_list.h"
#include "texture.h"
#include "moving_sphere.h"

using namespace glm;
using namespace agl;
using namespace std;

//this is the ray_color function for light source
color ray_color_emit(const ray& r, const color& background, const hittable_list& world, int depth)
{
	hit_record rec;
	if (depth <= 0)
	{
		return color(0);
	}

	if (!world.hit(r, 0.001f, infinity, rec))
		return background;

	ray scattered;
	color attenuation;
	color emitColor = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
	if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
		return emitColor;
	return emitColor + attenuation * ray_color_emit(scattered, background, world, depth - 1);
}

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

void ray_trace(ppm_image& image)
{
	// Image
	int height = image.height();
	int width = image.width();
	float aspect = width / float(height);
	int samples_per_pixel = 10; // higher => more anti-aliasing
	int max_depth = 10; // higher => less shadow acne

	// Camera
	vec3 camera_pos(0, 0, 6);
	float viewport_height = 2.0f;
	float focal_length = 4.0;
	camera cam(camera_pos, viewport_height, aspect, focal_length);
	color background = color(0.0, 0.0, 0.0);
	int cases = 5;
	if (cases == 0)
	{
		shared_ptr<texture> number_texture = make_shared<image_texture>("../images/numberGrid.png");
		shared_ptr<material> number_surface = make_shared<lambertian>(number_texture);
		shared_ptr<texture> wood_texture = make_shared<image_texture>("../images/wood.jpg");
		shared_ptr<material> wood_surface = make_shared<lambertian>(wood_texture);
		shared_ptr<material> gray = make_shared<lambertian>(color(0.5f));
		shared_ptr<texture> checker = make_shared<checker_texture>(color(0.945, 0.356, 0.356), color(0.964, 0.972, 0.407));

		hittable_list world;
		world.add(make_shared<sphere>(point3(0, 0, -1), 0.5f, number_surface));
		world.add(make_shared<sphere>(point3(0, -100.5, -1), 100, make_shared<lambertian>(checker)));
		world.add(make_shared<triangle>(point3(-2, 0, 0), point3(-1, 0, 1), point3(-2, 1, 1), make_shared<lambertian>(checker)));
		world.add(make_shared<triangle>(point3(2, 0, 0), point3(1, 0, 1), point3(2, 1, 1), number_surface));
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

		image.save("basic_checker_texture.png");
	}
	else if (cases == 1)
	{
		shared_ptr<material> gray = make_shared<lambertian>(color(0.5f));
		shared_ptr<material> matteGreen = make_shared<lambertian>(color(0, 0.5f, 0));
		shared_ptr<material> metalRed = make_shared<metal>(color(1, 0, 0), 0.3f);
		shared_ptr<material> glass = make_shared<dielectric>(1.5f);
		shared_ptr<material> phongDefault = make_shared<phong>(camera_pos);
		shared_ptr<texture> checker = make_shared<checker_texture>(color(0.945, 0.356, 0.356), color(0.294, 0.278, 0.941));

		hittable_list world;
		world.add(make_shared<sphere>(point3(-2.25, 0, -1), 0.5f, phongDefault));
		world.add(make_shared<sphere>(point3(-0.75, 0, -1), 0.5f, glass));
		world.add(make_shared<sphere>(point3(2.25, 0, -1), 0.5f, metalRed));
		world.add(make_shared<sphere>(point3(0.75, 0, -1), 0.5f, make_shared<lambertian>(checker)));
		world.add(make_shared<sphere>(point3(0, -100.5, -1), 100, gray));
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

		image.save("materials_check_texture.png");
	}
	else if (cases == 2)
	{
		shared_ptr<texture> earth_texture = make_shared<image_texture>("../images/earth.jpg");
		shared_ptr<material> earth_surface = make_shared<lambertian>(earth_texture);

		hittable_list world;
		world.add(make_shared<sphere>(point3(0, 0, -1), 1.3f, earth_surface));
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

		image.save("image_texture.png");
	}
	else if (cases == 3)
	{
		color background = color(0, 0, 0);
		shared_ptr<material> gray = make_shared<lambertian>(color(0.5f));
		shared_ptr<texture> checker = make_shared<checker_texture>(color(0.945, 0.356, 0.356), color(0.964, 0.972, 0.407));
		shared_ptr<material> emitLight = make_shared<emit_light>(color(4, 4, 4));

		hittable_list world;
		world.add(make_shared<sphere>(point3(2, 0, -1), 0.5f, emitLight));
		world.add(make_shared<sphere>(point3(0, -100.5, -1), 100, make_shared<lambertian>(checker)));
		world.add(make_shared<triangle>(point3(0, 0, 0), point3(-1, 0, 1), point3(1, 1, 1), emitLight));
		//world.add(make_shared<plane>(point3(-2, 2, 0), vec3(0,0,1), emitLight));


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
					c += ray_color_emit(r, background, world, max_depth);
				}
				c = normalize_color(c, samples_per_pixel);
				image.set_vec3(j, i, c);
			}
		}

		image.save("light_sources.png");
	}
	else if (cases == 4)
	{
		hittable_list world;

		auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
		auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
		auto material_left = make_shared<dielectric>(1.5);
		auto material_right = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);

		world.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));
		world.add(make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, material_center));
		world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
		world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), -0.45, material_left));
		world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));
		point3 lookfrom(3, 3, 2);
		point3 lookat(0, 0, -1);
		vec3 vup(0, 1, 0);
		auto dist_to_focus = length(lookfrom - lookat);
		auto aperture = 2.0;

		camera cam1(lookfrom, lookat, vup, 20, aspect, aperture, dist_to_focus);
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

		image.save("defocus_blur.png");
	}
	else if (cases == 5)
	{
		hittable_list world;

		shared_ptr<material> bottom = make_shared<lambertian>(color(0.5f));
		shared_ptr<material> lambertian_red = make_shared<lambertian>(color(0.898, 0.243, 0.243));
		shared_ptr<material> lambertian_blue = make_shared<lambertian>(color(0.2, 0.435, 0.858));

		point3 center1(0.2,0.2,0.2);
		point3 center2(2, 0.5, 0.9);
		point3 center3(-2, 0.5, 0.9);
		point3 center4(-1, 0.5, 0.9);
		world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, bottom));
		world.add(make_shared<moving_sphere>(center1, point3(0, random_float(0, 0.5), 0) + center1, 0.0, 1.0, 0.2, lambertian_red));
		world.add(make_shared<moving_sphere>(center2, point3(random_float(0, 0.5), 0, 0) + center2, 0.0, 1.0, 0.2, lambertian_blue));
		world.add(make_shared<moving_sphere>(center3, point3( 0, random_float(0, 0.5), 0) + center3, 0.0, 1.0, 0.2, lambertian_blue));
		world.add(make_shared<moving_sphere>(center4, point3(0, random_float(0, 0.5), 0) + center4, 0.0, 1.0, 0.2, lambertian_red));

		point3 lookfrom(13, 3, 2);
		point3 lookat(0, 0, -1);
		vec3 vup(0, 1, 0);
		float dist_to_focus = length(lookfrom - lookat);
		float aperture = 2.0;

		camera cam1(lookfrom, lookat, vup, 20, aspect, aperture, dist_to_focus);
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

		image.save("motion_blur.png");
	}

}
