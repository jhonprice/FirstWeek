#pragma once
#include "vec3.h"
#include "onb.h"
class pdf {
public:
    virtual ~pdf() {}

    virtual double value(const Vec3& direction) const = 0;
    virtual Vec3 generate() const = 0;
};

class Cosine_pdf : public pdf {
public:
    Cosine_pdf(const Vec3& w) { uvw.build_from_w(w); }

    virtual double value(const Vec3& direction) const override {
        auto cosine = dot(unit_vector(direction), uvw.w());
        return (cosine <= 0) ? 0 : cosine / pi;
    }

    virtual Vec3 generate() const override {
        return uvw.local(random_cosine_direction());
    }

public:
    ONB uvw;
};


class Hittable_pdf : public pdf {
public:
    Hittable_pdf(shared_ptr<Hittable> p, const Point3& origin) : ptr(p), o(origin) {}

    virtual double value(const Vec3& direction) const override {
        return ptr->pdf_value(o, direction);
    }

    virtual Vec3 generate() const override {
        return ptr->random(o);
    }

public:
    Point3 o;
    std::shared_ptr<Hittable> ptr;
};

class Mixture_pdf : public pdf {
public:
    Mixture_pdf(std::shared_ptr<pdf> p0, std::shared_ptr<pdf> p1) {
        p[0] = p0;
        p[1] = p1;
    }

    virtual double value(const Vec3& direction) const override {
        return 0.5 * p[0]->value(direction) + 0.5 * p[1]->value(direction);
    }

    virtual Vec3 generate() const override {
        if (random_double() < 0.5)
            return p[0]->generate();
        else
            return p[1]->generate();
    }

public:
    std::shared_ptr<pdf> p[2];
};