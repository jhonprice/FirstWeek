#pragma once
#include "hittable.h"
#include "material.h"
#include "texture.h"

class Constant_medium : public Hittable {
public:
    Constant_medium(std::shared_ptr<Hittable> b, double d, std::shared_ptr<Texture> a)
        : boundary(b),
        neg_inv_density(-1 / d),
        phase_function(std::make_shared<Isotropic>(a))
    {}

    Constant_medium(std::shared_ptr<Hittable> b, double d, RGBColor c)
        : boundary(b),
        neg_inv_density(-1 / d),
        phase_function(std::make_shared<Isotropic>(c))
    {}

    virtual bool hit(Ray& r,Hit_record& rec) const override;

    virtual bool bounding_box(double time0, double time1, AABB& output_box) const override {
        return boundary->bounding_box(time0, time1, output_box);
    }

public:
    static constexpr double  infinity = std::numeric_limits<double>::infinity();
    std::shared_ptr<Hittable> boundary;
    std::shared_ptr<Material> phase_function;
    double neg_inv_density;
};


bool Constant_medium::hit(Ray& r, Hit_record& rec) const {
    // Print occasional samples when debugging. To enable, set enableDebug true.
    const bool enableDebug = false;
    const bool debugging = enableDebug;

    Hit_record rec1, rec2;

    Ray longRay = r;
    longRay.tMax = infinity;
    longRay.tMin = -infinity;

    // �����ж�����������˶��ӳ������޳����Ƿ�������������İ�Χ���ཻ
    if (!boundary->hit(longRay, rec1))
        // ������ཻ�򷵻�false
        return false;

    longRay.tMin = rec1.t + 0.0001;
    longRay.tMax = infinity;

    // �������ڵ�һ���ཻ�� ��һ���ӳ������޳����Ƿ��������ڶ����ཻ��������ܵڶ����ཻ�򷵻�false
    if (!boundary->hit(longRay, rec2))
        return false;

    if (debugging) std::cerr << "\nt_min=" << rec1.t << ", t_max=" << rec2.t << '\n';

    // �������ཻ��ʱ�������� t_min��t_max֮��
    if (rec1.t < longRay.tMin) rec1.t = r.tMin;
    if (rec2.t > longRay.tMax) rec2.t = r.tMax;

    // �����һ���ཻ��ʱ��ȵڶ��λ�Ҫ���򷵻�false
    if (rec1.t >= rec2.t)
        return false;


    // �ѵ�һ���ཻ��ʱ��������0���ϣ������ߵ������
    if (rec1.t < 0)
        rec1.t = 0;

    // �����ܳ���
    const auto ray_length = r.m_dir.length();
    // ������������ڵĳ���
    const auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
    // ���һ�������� ������ �ܶ� ��������һ����ײ�����
    const auto hit_distance = neg_inv_density * log(random_double());

    // �����ǰ������������ڵĳ��� �� ��ǰ����������ײ����� ��Ҫ�̣��򷵻�false
    if (hit_distance > distance_inside_boundary)
        return false;

    // ��ײʱ�� = ��һ����ײ���ʱ��t + �����/�����ܳ���
    rec.t = rec1.t + hit_distance / ray_length;
    rec.p = r.at(rec.t);

    if (debugging) {
        std::cerr << "hit_distance = " << hit_distance << '\n'
            << "rec.t = " << rec.t << '\n'
            << "rec.p = " << rec.p << '\n';
    }

    rec.normal = Vec3(1, 0, 0);  // ��ײ��������
    rec.front_face = true;     // ����ײ��ǰ����
    rec.mat_ptr = phase_function;

    return true;
}