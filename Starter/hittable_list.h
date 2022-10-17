#pragma once
#include "hittable.h"

#include <memory>
#include <vector>

using std::shared_ptr;
using std::make_shared;

class Hittable_list : public Hittable {
public:
    Hittable_list() {}
    Hittable_list(shared_ptr<Hittable> object) { add(object); }

    void clear() { objects.clear(); }
    void add(shared_ptr<Hittable> object) { objects.push_back(object); }

    virtual bool hit(
        Ray& r, Hit_record& rec) const override;

public:
    std::vector<shared_ptr<Hittable>> objects;
};

bool Hittable_list::hit(Ray& r, Hit_record& rec) const {
    Hit_record temp_rec;
    bool hit_anything = false;
    auto closest_so_far = r.tMax;

    for (const auto& object : objects) {
        if (object->hit(r, temp_rec)) {
            hit_anything = true;
            r.tMax = temp_rec.t;
            rec = temp_rec;
        }
    }

    return hit_anything;
}