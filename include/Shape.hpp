#pragma once

#include <vector>

#include "Vec_3d.hpp"

class Shape_base;

struct Intersection_point{
    Vec_3d pos;
    Shape_base *shape;
    double dist;

    Intersection_point(Vec_3d pos, Shape_base *shape, double dist): pos(pos), shape(shape), dist(dist) { };
    Intersection_point(): pos(Vec_3d(0, 0, 0)), shape(nullptr), dist(std::numeric_limits<double>::infinity()) { };
    bool operator<(Intersection_point const & rha){
        return dist < rha.dist;
    }
    bool operator>(Intersection_point const & rha){
        return dist > rha.dist;
    }
};

class Shape_base{
private:

public:
    Shape_base *parent;

    Shape_base(): parent(nullptr) {};
    virtual ~Shape_base() = default;
    virtual void get_intersections (Photon photon, std::vector<Intersection_point> &ans) = 0;
    virtual Vec_3d get_normal (Vec_3d point) = 0;

    virtual bool point_is_inside (Intersection_point inter) = 0;
};

class Shape_plane: public Shape_base{
private:

public:
    Vec_3d pos, normal;

    Shape_plane(Vec_3d pos, Vec_3d normal):pos(pos), normal(normal/normal.len()) { };

    ~Shape_plane() = default;
    void get_intersections (Photon photon, std::vector<Intersection_point> &ans){
        Vec_3d pos_rel = photon.pos - pos;
        double dist = - ((pos_rel * normal) / (photon.dir * normal));

        if(dist > 0){
            Vec_3d pos_inter = photon.pos + dist * photon.dir;
            ans.push_back(Intersection_point(pos_inter, this, dist));
        }
    };
    Vec_3d get_normal(Vec_3d point){
        return normal;
    };
    bool point_is_inside(Intersection_point inter){
        return this == inter.shape || (inter.pos - pos) * normal < 0;
    };
};

class Shape_cylinder: public Shape_base{
private:

public:
    Vec_3d pos, dir;
    double rad;

    Shape_cylinder(Vec_3d pos, Vec_3d dir, double rad):pos(pos), dir(dir/dir.len()), rad(rad) { };

    ~Shape_cylinder() = default;
    void get_intersections (Photon photon, std::vector<Intersection_point> &ans){
        Vec_3d pos_rel = photon.pos - pos;
        Vec_3d pos_radial = pos_rel    - (pos_rel    * dir) * dir;
        Vec_3d dir_radial = photon.dir - (photon.dir * dir) * dir;

        double scalar_radial = pos_radial * dir_radial;
        double discriminant = sqr(scalar_radial) - (pos_radial.sqr() - sqr(rad)) * dir_radial.sqr();

        if(discriminant < 0){
            return;
        }

        for (int sign=-1; sign<=1; sign+=2){
            double dist = ( -scalar_radial + sign*std::sqrt(discriminant)) / dir_radial.sqr();
            if (dist > 0) {
                Vec_3d pos_inter = photon.pos + dist * photon.dir;
                ans.push_back(Intersection_point(pos_inter, this, dist));
            }
        }
    };
    Vec_3d get_normal(Vec_3d point){
        Vec_3d point_rel = point - pos;
        Vec_3d normal_component = point_rel - (point_rel*dir)*dir;
        return normal_component/normal_component.len();
    };
    bool point_is_inside(Intersection_point inter){
        Vec_3d pos_rel = inter.pos - pos;
        return this == inter.shape || pos_rel.sqr() - sqr(pos_rel * dir) < sqr(rad);
    };
};

class Shape_ball: public Shape_base{
private:

public:
    Vec_3d pos;
    double rad;

    Shape_ball(Vec_3d pos, double rad):pos(pos), rad(rad) { };

    ~Shape_ball() = default;
    void get_intersections (Photon photon, std::vector<Intersection_point> &ans){
        Vec_3d pos_rel = photon.pos - pos;

        double pos_dot_dir = pos_rel * photon.dir;
        double discriminant = sqr(pos_dot_dir) - pos_rel.sqr() + sqr(rad);

        if(discriminant < 0){
            return;
        }

        for (int sign=-1; sign<=1; sign+=2){
            double dist = ( -pos_dot_dir + sign*std::sqrt(discriminant));
            if (dist > 0) {
                Vec_3d pos_inter = photon.pos + dist * photon.dir;
                ans.push_back(Intersection_point(pos_inter, this, dist));
            }
        }
    };
    Vec_3d get_normal(Vec_3d point){
        Vec_3d point_rel = point - pos;
        return point_rel/point_rel.len();
    };
    bool point_is_inside(Intersection_point inter){
        return this == inter.shape || (inter.pos - pos).sqr() < sqr(rad);
    };
};

class Shape_inversion: public Shape_base{
private:

public:
    Shape_base *shape;

    Shape_inversion(Shape_base *shape): shape(shape){
        shape->parent = this;
    };

    ~Shape_inversion(){
        delete shape;
    };
    void get_intersections (Photon photon, std::vector<Intersection_point> &ans){
        shape->get_intersections(photon, ans);
    };
    Vec_3d get_normal(Vec_3d point){
        std::cout << "aboba\n";
        return Vec_3d(1, 0, 0);
    };
    bool point_is_inside(Intersection_point inter){
        return !(shape->point_is_inside(inter));
    };
};

class Shape_union: public Shape_base{
private:

public:
    Shape_base *shape_1;
    Shape_base *shape_2;

    Shape_union(Shape_base *shape_1, Shape_base *shape_2): shape_1(shape_1), shape_2(shape_2){
        shape_1->parent = this;
        shape_2->parent = this;
    };

    ~Shape_union(){
        delete shape_1;
        delete shape_2;
    };
    void get_intersections (Photon photon, std::vector<Intersection_point> &ans){
        shape_1->get_intersections(photon, ans);
        shape_2->get_intersections(photon, ans);
    };
    Vec_3d get_normal(Vec_3d point){
        std::cout << "aboba\n";
        return Vec_3d(1, 0, 0);
    };
    bool point_is_inside(Intersection_point inter){
        return shape_1->point_is_inside(inter) || shape_2->point_is_inside(inter);
    };
};

class Shape_intersection: public Shape_base{
private:

public:
    Shape_base *shape_1;
    Shape_base *shape_2;

    Shape_intersection(Shape_base *shape_1, Shape_base *shape_2): shape_1(shape_1), shape_2(shape_2){
        shape_1->parent = this;
        shape_2->parent = this;
    }

    ~Shape_intersection(){
        delete shape_1;
        delete shape_2;
    };
    void get_intersections (Photon photon, std::vector<Intersection_point> &ans){
        shape_1->get_intersections(photon, ans);
        shape_2->get_intersections(photon, ans);
    };
    Vec_3d get_normal(Vec_3d point){
        std::cout << "aboba\n";
        return Vec_3d(1, 0, 0);
    };
    bool point_is_inside(Intersection_point inter){
        return shape_1->point_is_inside(inter) && shape_2->point_is_inside(inter);
    };
};

class Screen{
private:

public:
    Vec_3d pos, a, b, dir_normal;

    Screen(Vec_3d pos, Vec_3d a, Vec_3d b_):pos(pos), a(a) {
        b = b_ - ((b_ * a)/a.sqr())*a;
        dir_normal = Vec_3d(a.y*b.z - b.y*a.z, a.z*b.x - b.z*a.x, a.x*b.y - b.x*a.y);
        dir_normal /= dir_normal.len();
    };

    double dist(Photon photon){
        double ans = - (dir_normal * (photon.pos - pos))/(dir_normal * photon.dir);
        Vec_3d hit_pos = photon.pos + ans*photon.dir - pos;
        if( ans < 0 || sqr(hit_pos * a) > sqr(a.sqr()) || sqr(hit_pos * b) > sqr(b.sqr())){
            return std::numeric_limits<double>::infinity();
        }
        return ans;
    };
    Vec_3d normal(Photon photon){
        return dir_normal;
    };
};
