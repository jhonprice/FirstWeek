#pragma once
#include "../MultipleObjectAndCodeClean/rtweekend.h"
#include "../RayCameraAndBackGround/ray.h"
#include "../MultipleObjectAndCodeClean/hittable.h"
struct hit_record;

//<<材质与散射的抽象类>>
class material {
public:
	virtual bool scatter(const ray& r_in,const hit_record&rec,color&attenuation,ray&scattered)  const = 0;
};

//<<朗波散射材质抽象>>
//+=<<散射接近0的结果处理>> - 1
class lambertian : public material {
public:
    lambertian(const color& a) : albedo(a) {}

    //注意到衰减率的设置
    virtual bool scatter(
        const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
    ) const override {
        auto scatter_direction = rec.normal + random_unit_vector();
        // 1
        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;

        scattered = ray(rec.p, scatter_direction);
        attenuation = albedo;
        return true;
    }

public:
    color albedo;
};


//<<镜面反射>>
//+=<<模糊反射参数>> - 2
class metal : public material {
public:
    metal(const color& a) : albedo(a) {}
    metal(const color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

    virtual bool scatter(
        const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
    ) const override {
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        //scattered = ray(rec.p, reflected);
        //在镜面反射的基础上加一个单位圆内的偏移向量
        scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere()); // 2
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }

public:
    color albedo;
    double fuzz=0;
};

//<<折射材质>> - 3
//+=<<全内反射>> - 4
//+=<<石里克近似>> - 5
class dielectric : public material {
public:
    dielectric(double index_of_refraction) : ir(index_of_refraction) {}

    virtual bool scatter(
        const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
    ) const override {
        attenuation = color(1.0, 1.0, 1.0);
        double refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

        vec3 unit_direction = unit_vector(r_in.direction());
        //vec3 refracted = refract(unit_direction, rec.normal, refraction_ratio); // 3
        //scattered = ray(rec.p, refracted); // 3

// 4
#pragma region MyRegion
        double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

        bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        vec3 direction;

        //5: if (cannot_refract)
        if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double()) //5
            direction = reflect(unit_direction, rec.normal);
        else
            direction = refract(unit_direction, rec.normal, refraction_ratio);

        scattered = ray(rec.p, direction);
#pragma endregion


        return true;
    }

public:
    double ir; // Index of Refraction
private:
    // 4
    static double reflectance(double cosine, double ref_idx) {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1 - ref_idx) / (1 + ref_idx);
        r0 = r0 * r0;
        return r0 + (1 - r0) * pow((1 - cosine), 5);
    }
};