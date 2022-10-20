#pragma once
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
        // ͨ��3D�����3��������sinֵ���
        auto sines = sin(10 * p.x()) * sin(10 * p.y()) * sin(10 * p.z());
        // ͨ�����sinֵ�Ľ���������ж����ĸ���ɫ�������ɫ
        if (sines < 0)
            return odd->value(u, v, p);
        else
            return even->value(u, v, p);
    }

public:
    std::shared_ptr<Texture> odd;  // ��ɫ����
    std::shared_ptr<Texture> even; // ��������
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