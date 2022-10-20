#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"
#include "vec3.h"
#include "perlin.h"
class Texture {
public:
	virtual RGBColor value(double u, double v, const Point3& p) const = 0;
};

class SolidColor : public Texture {
public:
    SolidColor() {}
    SolidColor(RGBColor c) : color_value(c) {}

    SolidColor(double red, double green, double blue)
        : SolidColor(RGBColor(red, green, blue)) {}

    virtual RGBColor value(double u, double v, const Vec3& p) const override {
        return color_value;
    }

private:
    RGBColor color_value;
};


class CheckerTexture : public Texture {
public:
    CheckerTexture() {}

    CheckerTexture(std::shared_ptr<Texture> _even, std::shared_ptr<Texture> _odd)
        : even(_even), odd(_odd) {}

    CheckerTexture(RGBColor c1, RGBColor c2)
        : even(std::make_shared<SolidColor>(c1)), odd(std::make_shared<SolidColor>(c2)) {}

    virtual RGBColor value(double u, double v, const Point3& p) const override {
        // 通过3D坐标的3个分量的sin值相乘
        auto sines = sin(10 * p.x()) * sin(10 * p.y()) * sin(10 * p.z());
        // 通过相乘sin值的结果符号来判断用哪个单色纹理的颜色
        if (sines < 0)
            return odd->value(u, v, p);
        else
            return even->value(u, v, p);
    }

public:
    std::shared_ptr<Texture> odd;  // 白色纹理
    std::shared_ptr<Texture> even; // 深绿纹理
};




class NoiseTexture : public Texture {
public:
    NoiseTexture() {}
    NoiseTexture(double sc):scale(sc) {}

    virtual RGBColor value(double u, double v, const Point3& p) const override {
        return RGBColor(1, 1, 1) * noise.noise(scale*p);
    }

public:
    Perlin noise;
    double scale;
};



class NoiseTextureVec : public Texture {
public:
    NoiseTextureVec() {}
    NoiseTextureVec(double sc) :scale(sc) {}

    virtual RGBColor value(double u, double v, const Point3& p) const override {
        //return RGBColor(1, 1, 1) * 0.5 * (1.0 + noise.noise(scale * p));
        //return RGBColor(1, 1, 1) * noise.turb(scale * p);
        return RGBColor(1, 1, 1) * 0.5 * (1 + sin(scale * p.z() + 10 * noise.turb(p)));
    }

public:
    PerlinVec noise;
    double scale;
};


class ImageTexture : public Texture {
public:
    const static int bytes_per_pixel = 3;

    ImageTexture()
        : data(nullptr), width(0), height(0), bytes_per_scanline(0) {}

    ImageTexture(const char* filename) {
        auto components_per_pixel = bytes_per_pixel;

        data = stbi_load(
            filename, &width, &height, &components_per_pixel, components_per_pixel);

        if (!data) {
            std::cerr << "ERROR: Could not load texture image file '" << filename << "'.\n";
            width = height = 0;
        }

        bytes_per_scanline = bytes_per_pixel * width;
    }
    ~ImageTexture() {
        delete data;
    }

    virtual RGBColor value(double u, double v, const Vec3& p) const override {
        // If we have no texture data, then return solid cyan as a debugging aid.
        if (data == nullptr)
            return RGBColor(0, 1, 1);

        // Clamp input texture coordinates to [0,1] x [1,0]
        u = clamp(u, 0.0, 1.0);
        v = 1.0 - clamp(v, 0.0, 1.0);  // Flip V to image coordinates

        auto i = static_cast<int>(u * width);
        auto j = static_cast<int>(v * height);

        // Clamp integer mapping, since actual coordinates should be less than 1.0
        if (i >= width)  i = width - 1;
        if (j >= height) j = height - 1;

        const auto color_scale = 1.0 / 255.0;
        auto pixel = data + j * bytes_per_scanline + i * bytes_per_pixel;

        return RGBColor(color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]);
    }

private:
    unsigned char* data;
    int width{};
    int height{};
    int bytes_per_scanline{};
};