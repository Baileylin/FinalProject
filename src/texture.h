#ifndef TEXTURE_H
#define TEXTURE_H

#include <cmath>
#include <iostream>
#include "AGLM.h"
#include "ray.h"
#include "hittable.h"
#include "hittable_list.h"
#include <cassert>
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

//the texture class will transform all colors into one of the textures
class texture {
public:
	virtual glm::color value(double u, double v, const glm::point3& p) const = 0;
};

//this class is responsible for returning the color(no textures is applied)
class constant_texture: public texture {
public:
	constant_texture() {};
	constant_texture(glm::color c) { colorValue = c; }
	virtual glm::color value(double u, double v, const glm::point3& p) const 
	{
		return colorValue;
	}

private:
	glm::color colorValue;
};

class checker_texture : public texture {
public:
	checker_texture() {};
	checker_texture(shared_ptr<texture> texture1, shared_ptr<texture> texture2) {
		odd = texture1;
		even = texture2;
	}

	checker_texture(glm::color c1, glm::color c2) {
		odd = make_shared<constant_texture>(c1);
		even = make_shared<constant_texture>(c2);
	}
		
	virtual glm::color value(double u, double v, const glm::point3& p) const
	{
		float sines = sin(10 * p.x) * sin(10 * p.y) * sin(10 * p.z);
		if (sines < 0)
			return odd->value(u, v, p);
		else
			return even->value(u, v, p);
	}

public:
	shared_ptr<texture> odd;
	shared_ptr<texture> even;
};

class image_texture : public texture 
{
public:
    const static int bytes_per_pixel = 3;

    image_texture()
        : data(nullptr), width(0), height(0), bytes_per_scanline(0) {}

    image_texture(const char* filename) {
        auto components_per_pixel = bytes_per_pixel;

        data = stbi_load(filename, &width, &height, &components_per_pixel, components_per_pixel);

        if (!data) {
            std::cerr << "ERROR: Could not load texture image file '" << filename << "'.\n";
            width = height = 0;
        }

        bytes_per_scanline = bytes_per_pixel * width;
    }

    ~image_texture() {
        delete data;
    }

    virtual glm::color value(double u, double v, const glm::vec3& p) const override {
        if (data == nullptr)
            return glm::color(0, 1, 1);

        // Clamp input texture coordinates to [0,1] x [1,0]
        u = glm::clamp(u, 0.0, 1.0);
        v = 1.0 - glm::clamp(v, 0.0, 1.0);  // Flip V to image coordinates

        auto i = static_cast<int>(u * width);
        auto j = static_cast<int>(v * height);

        // Clamp integer mapping, since actual coordinates should be less than 1.0
        if (i >= width)  i = width - 1;
        if (j >= height) j = height - 1;

        const auto color_scale = 1.0 / 255.0;
        auto pixel = data + j * bytes_per_scanline + i * bytes_per_pixel;

        return glm::color(color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]);
    }

private:
    unsigned char* data;
    int width, height;
    int bytes_per_scanline;
};
#endif

