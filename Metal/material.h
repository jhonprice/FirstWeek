#pragma once
#include "../MultipleObjectAndCodeClean/rtweekend.h"
#include "../RayCameraAndBackGround/ray.h"
#include "../MultipleObjectAndCodeClean/hittable.h"
struct hit_record;

//<<������ɢ��ĳ�����>>
class material {
public:
	virtual bool scatter(const ray& r_in,const hit_record&rec,color&attenuation,ray&scattered)  const = 0;
};

//<<�ʲ�ɢ����ʳ���>>
//+=<<ɢ��ӽ�0�Ľ������>> - 1
class lambertian : public material {
public:
    lambertian(const color& a) : albedo(a) {}

    //ע�⵽˥���ʵ�����
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


//<<���淴��>>
//+=<<ģ���������>> - 2
class metal : public material {
public:
    metal(const color& a) : albedo(a) {}
    metal(const color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

    virtual bool scatter(
        const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
    ) const override {
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        //scattered = ray(rec.p, reflected);
        //�ھ��淴��Ļ����ϼ�һ����λԲ�ڵ�ƫ������
        scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere()); // 2
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }

public:
    color albedo;
    double fuzz=0;
};