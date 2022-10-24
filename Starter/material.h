#pragma once
#include "hittable.h"
#include "ray.h"
#include "texture.h"
#include "onb.h"
#include "pdf.h"

struct Scatter_record {
    Ray specular_ray;
    bool is_specular;
    RGBColor attenuation;
    std::shared_ptr<pdf> pdf_ptr;
};

class Material {
public:
    virtual bool scatter(
        const Ray& r_in, const Hit_record& rec, Scatter_record& srec
    ) const {
        return false;
    }

    virtual double  scatter_pdf(
        const Ray& r_in, const Hit_record& rec, Ray& scattered
    ) const {
        return 0;
    }


    virtual RGBColor emitted(const Ray& r_in, const Hit_record& rec, double u, double v, const Point3& p) const  {
        return RGBColor(0, 0, 0);
    }
};


class Lambertian : public Material {
public:

    Lambertian(const RGBColor& a) : albedo(std::make_shared<SolidColor>(a)) {}
    Lambertian(std::shared_ptr<Texture> a) : albedo(a) {}

    virtual bool scatter(
        const Ray& r_in, const Hit_record& rec, Scatter_record& srec
    ) const override {

        srec.is_specular = false;
        srec.attenuation = albedo->value(rec.uv.first, rec.uv.second, rec.p);
        srec.pdf_ptr = std::make_shared<Cosine_pdf>(Cosine_pdf(rec.normal));

        return true;
    }

    //virtual bool scatter(
    //    const Ray& r_in, const Hit_record& rec, RGBColor& alb, Ray& scattered, double& pdf
    //) const override {
    //    auto direction = random_in_hemisphere(rec.normal);
    //    scattered = Ray(rec.p, unit_vector(direction), r_in.m_time);
    //    alb = albedo->value(rec.uv.first, rec.uv.second, rec.p);
    //    pdf = 0.5 / pi;
    //    return true;
    //}

    virtual double  scatter_pdf(
        const Ray& r_in, const Hit_record& rec, Ray& scattered
    ) const {
        auto cosine = dot(rec.normal, unit_vector(scattered.m_dir));
        return cosine < 0 ? 0 : cosine / pi;
        //return 0.5 / pi;
    }

public:
    //RGBColor albedo;
    std::shared_ptr<Texture> albedo;
};


class DiffuseLight : public Material {
public:
    DiffuseLight(std::shared_ptr<Texture> a) : emit(a) {}
    DiffuseLight(RGBColor c) : emit(std::make_shared<SolidColor>(c)) {}


    virtual RGBColor emitted(const Ray& r_in, const Hit_record& rec, double u, double v, const Point3& p) const override {
        
        if (rec.front_face)
            return emit->value(u, v, p);
        else
            return RGBColor(0, 0, 0);
    }

public:
    std::shared_ptr<Texture> emit;
};

inline Vec3 reflect(const Vec3& v, const Vec3& n) {
    return v - 2 * dot(v, n) * n;
}

inline Vec3 refract(const Vec3& uv, const Vec3& n, double etai_over_etat) {
    auto cos_theta = std::min(dot(-uv, n), 1.0);
    Vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
    Vec3 r_out_parallel = -sqrt(std::abs(1.0 - r_out_perp.length_squared())) * n;
    return r_out_perp + r_out_parallel;
}

class Metal : public Material {
public:
    Metal(const RGBColor& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}
    virtual bool scatter(
        const Ray& r_in, const Hit_record& rec, Scatter_record& srec
    ) const override {
        Vec3 reflected = reflect(unit_vector(r_in.m_dir), rec.normal);
        srec.specular_ray = Ray(rec.p, reflected + fuzz * random_in_unit_sphere());
        srec.attenuation = albedo;
        srec.is_specular = true;
        srec.pdf_ptr = nullptr;
        return true;
    }

public:
    RGBColor albedo;
    double fuzz;
};


class Dielectric : public Material {
public:
    Dielectric(double index_of_refraction) : ir(index_of_refraction) {}

    virtual bool scatter(
        const Ray& r_in, const Hit_record& rec, Scatter_record& srec
    ) const override {
        srec.is_specular = true;
        srec.pdf_ptr = nullptr;
        srec.attenuation = RGBColor(1.0, 1.0, 1.0);
        double refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

        Vec3 unit_direction = unit_vector(r_in.m_dir);
        double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

        bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        Vec3 direction;

        if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
            direction = reflect(unit_direction, rec.normal);
        else
            direction = refract(unit_direction, rec.normal, refraction_ratio);

        srec.specular_ray = Ray(rec.p, direction, r_in.m_time);
        return true;
    }

public:
    double ir; // Index of Refraction

private:
    static double reflectance(double cosine, double ref_idx) {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1 - ref_idx) / (1 + ref_idx);
        r0 = r0 * r0;
        return r0 + (1 - r0) * pow((1 - cosine), 5);
    }
};