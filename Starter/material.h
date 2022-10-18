#pragma once
#include "hittable.h"
#include "ray.h"

class Material {
public:
    virtual bool scatter(
        const Ray& r_in, const Hit_record& rec, RGBColor& attenuation, Ray& scattered
    ) const = 0;
};


class Lambertian : public Material {
public:
    Lambertian(const RGBColor& a) : albedo(a) {}

    virtual bool scatter(
        const Ray& r_in, const Hit_record& rec, RGBColor& attenuation, Ray& scattered
    ) const override {

        //不同的散射方式
        //Point3 target = rec.p + rec.normal + random_in_hemisphere(rec.normal);
        //Point3 target = rec.p + rec.normal + random_unit_vector();
        //Point3 target = rec.p + rec.normal + random_in_unit_sphere();

        Vec3 scatter_direction = rec.normal + random_unit_vector();


        // Catch degenerate scatter direction
        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;


        scattered = Ray(rec.p, scatter_direction);
        attenuation = albedo;
        return true;
    }

public:
    RGBColor albedo;
};


class Metal : public Material {
public:
    Metal(const RGBColor& a) : albedo(a) {}
    Metal(const RGBColor& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

    Vec3 reflect(const Vec3& v, const Vec3& n) const {
        return v - 2 * dot(v, n) * n;
    }

    virtual bool scatter(
        const Ray& r_in, const Hit_record& rec, RGBColor& attenuation, Ray& scattered
    ) const override {
        Vec3 reflected = reflect(unit_vector(r_in.m_dir), rec.normal);
        scattered = Ray(rec.p, reflected + fuzz * random_in_unit_sphere());
        attenuation = albedo;
        return (dot(scattered.m_dir, rec.normal) > 0);
    }

public:
    RGBColor albedo;
    double fuzz{};
};



