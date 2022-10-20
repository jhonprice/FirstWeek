#pragma once
#include "hittable.h"
#include "ray.h"
#include "texture.h"

class Material {
public:
    virtual bool scatter(
        const Ray& r_in, const Hit_record& rec, RGBColor& attenuation, Ray& scattered
    ) const = 0;

    virtual RGBColor emitted(double u, double v, const Point3& p) const  {
        return RGBColor(0, 0, 0);
    }
};


class Lambertian : public Material {
public:

    Lambertian(const RGBColor& a) : albedo(std::make_shared<SolidColor>(a)) {}
    Lambertian(std::shared_ptr<Texture> a) : albedo(a) {}

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


        scattered = Ray(rec.p, scatter_direction, r_in.m_time);
        attenuation = albedo->value(rec.uv.first, rec.uv.second,rec.p);
        return true;
    }

public:
    //RGBColor albedo;
    std::shared_ptr<Texture> albedo;
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
        scattered = Ray(rec.p, reflected + fuzz * random_in_unit_sphere(), r_in.m_time);
        attenuation = albedo;
        return (dot(scattered.m_dir, rec.normal) > 0);
    }

public:
    RGBColor albedo;
    double fuzz{};
};


class Dielectric : public Material {
public:
    Dielectric(double index_of_refraction) : ir(index_of_refraction) {}

    //折射+全内反射
    Vec3 refract(const Vec3& uv, const Vec3& n, double etai_over_etat) const {
        auto cos_theta = fmin(dot(-uv, n), 1.0);
        auto sin_theta = sqrt(1.0 - cos_theta * cos_theta);

        bool cannot_refract = etai_over_etat* sin_theta > 1.0;
        if (cannot_refract || reflectance(cos_theta, etai_over_etat) > random_double()) {
            return uv - 2 * dot(uv, n) * n; //反射
        }
        else {
            //折射
            Vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
            Vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * n;
            return r_out_perp + r_out_parallel;
        }


    }


    virtual bool scatter(
        const Ray& r_in, const Hit_record& rec, RGBColor& attenuation, Ray& scattered
    ) const override {
        attenuation = RGBColor(1.0, 1.0, 1.0);
        double refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

        Vec3 unit_direction = unit_vector(r_in.m_dir);


        Vec3 refracted = refract(unit_direction, rec.normal, refraction_ratio);

        scattered = Ray(rec.p, refracted, r_in.m_time);
        return true;
    }

public:
    double ir; // Index of Refraction

private:
    static double reflectance(double cosine, double ref_idx) {
        // Schlick 近似公式
        auto r0 = (1 - ref_idx) / (1 + ref_idx);
        r0 = r0 * r0;
        return r0 + (1 - r0) * pow((1 - cosine), 5);
    }
};



class DiffuseLight : public Material {
public:
    DiffuseLight(std::shared_ptr<Texture> a) : emit(a) {}
    DiffuseLight(RGBColor c) : emit(std::make_shared<SolidColor>(c)) {}

    virtual bool scatter(
        const Ray& r_in, const Hit_record& rec, RGBColor& attenuation, Ray& scattered
    ) const override {
        return false;
    }

    virtual RGBColor emitted(double u, double v, const Point3& p) const override {
        return emit->value(u, v, p);
    }

public:
    std::shared_ptr<Texture> emit;
};



class Isotropic : public Material {
public:
    Isotropic(RGBColor c) : albedo(std::make_shared<SolidColor>(c)) {}
    Isotropic(shared_ptr<Texture> a) : albedo(a) {}

    virtual bool scatter(
        const Ray& r_in, const Hit_record& rec, RGBColor& attenuation, Ray& scattered
    ) const override {
        scattered = Ray(rec.p, random_in_unit_sphere(), r_in.m_time);
        attenuation = albedo->value(rec.uv.first, rec.uv.second, rec.p);
        return true;
    }

public:
    std::shared_ptr<Texture> albedo;
};