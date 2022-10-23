#pragma once
#include "hittable.h"
#include "ray.h"
#include "texture.h"
#include "onb.h"

class Material {
public:
    virtual bool scatter(
        const Ray& r_in, const Hit_record& rec, RGBColor& attenuation, Ray& scattered, double& pdf
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
        const Ray& r_in, const Hit_record& rec, RGBColor& attenuation, Ray& scattered, double& pdf
    ) const override {

        ONB uvw;
        uvw.build_from_w(rec.normal);
        auto scatter_direction = uvw.local(random_cosine_direction());


        scattered = Ray(rec.p, unit_vector(scatter_direction), r_in.m_time);
        attenuation = albedo->value(rec.uv.first, rec.uv.second,rec.p);

        pdf = dot(rec.normal, scattered.m_dir) / pi; //µã³ËÇócos
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
