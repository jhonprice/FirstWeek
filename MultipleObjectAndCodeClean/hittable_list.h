#pragma once

#include "hittable.h"
#include<memory>
#include<vector>


using std::shared_ptr;
using std::make_shared;


//<<可命中对象列表>>
class hittable_list :public hittable {
//<<初始化列表与增减操作>>
#pragma region InitAndModify
public:
	hittable_list() {}
	hittable_list(shared_ptr<hittable> object) { add(object); }
	void clear() { objects.clear(); }
	void add(shared_ptr<hittable> object) { objects.push_back(object); }
#pragma endregion

//<<集合中的所有物体与光线的相交情况>> - 1
	virtual bool hit(
		const ray& r, double t_min, double t_max, hit_record& rec) const override;

public:
	std::vector<shared_ptr<hittable>> objects;
};


//1： <<集合中的所有物体与光线的相交情况>>
bool hittable_list::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    hit_record temp_rec;
    bool hit_anything = false;
    auto closest_so_far = t_max;

    //<<循环判断每个物体的相交情况，并包含了深度检测>>
    for (const auto& object : objects) {
        if (object->hit(r, t_min, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }

    return hit_anything;
}