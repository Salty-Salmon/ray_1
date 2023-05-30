#pragma once

#include <vector>

#include "Vec_3d.hpp"

class Shape_base{
private:

public:
    virtual ~Shape_base() = default;
    virtual bool point_is_inside(Vec_3d point) = 0;
    virtual std::vector< std::pair< Vec_3d, Vec_3d > > get_intersections (Photon photon) = 0;
};

class Shape_plane: public Shape_base{
private:

public:
    Vec_3d pos, normal;

    Shape_plane(Vec_3d pos, Vec_3d normal):pos(pos), normal(normal/normal.len()) { };

    ~Shape_plane() = default;
    bool point_is_inside(Vec_3d point){
        return (point - pos) * normal > 0;
    };
    std::vector< std::pair< Vec_3d, Vec_3d > > get_intersections (Photon photon){
        std::vector< std::pair< Vec_3d, Vec_3d > > ans;

        Vec_3d pos_rel = photon.pos - pos;
        double dist = - ((pos_rel * normal) / (photon.dir * normal));

        if(dist > 0){
            Vec_3d pos_inter = photon.pos + dist * photon.dir;
            ans.push_back(std::make_pair(pos_inter, normal));
        }
        return ans;
    };
};

class Shape_cylinder: public Shape_base{
private:

public:
    Vec_3d pos, dir;
    double rad;

    Shape_cylinder(Vec_3d pos, Vec_3d dir, double rad):pos(pos), dir(dir/dir.len()), rad(rad) { };

    ~Shape_cylinder() = default;
    bool point_is_inside(Vec_3d point){
        Vec_3d pos_rel = point - pos;
        return (pos_rel - (pos_rel * dir) * dir).sqr() < sqr(rad);
    };
    std::vector< std::pair< Vec_3d, Vec_3d > > get_intersections (Photon photon){
        std::vector< std::pair< Vec_3d, Vec_3d > > ans;

        Vec_3d pos_rel = photon.pos - pos;
        Vec_3d pos_radial = pos_rel    - (pos_rel    * dir) * dir;
        Vec_3d dir_radial = photon.dir - (photon.dir * dir) * dir;

        double scalar_radial = pos_radial * dir_radial;
        double discriminant = sqr(scalar_radial) - (pos_radial.sqr() - sqr(rad)) * dir_radial.sqr();

        if(discriminant < 0){
            return ans;
        }

        for (int sign=-1; sign<=1; sign+=2){
            double dist = ( -scalar_radial + sign*std::sqrt(discriminant)) / dir_radial.sqr();
            if (dist > 0) {
                Vec_3d pos_inter = photon.pos + dist * photon.dir;
                Vec_3d normal = pos_radial + dist * dir;
                normal /= normal.len();
                ans.push_back(std::make_pair(pos_inter, normal));
            }
        }
        return ans;
    };
};

class Shape_ball: public Shape_base{
private:

public:
    Vec_3d pos;
    double rad;

    Shape_ball(Vec_3d pos, double rad):pos(pos), rad(rad) { };

    ~Shape_ball() = default;
    bool point_is_inside(Vec_3d point){
        Vec_3d pos_rel = point - pos;
        return pos_rel.sqr() < sqr(rad);
    };
    std::vector< std::pair< Vec_3d, Vec_3d > > get_intersections (Photon photon){
        std::vector< std::pair< Vec_3d, Vec_3d > > ans;

        Vec_3d pos_rel = photon.pos - pos;

        double scalar = pos_rel * photon.dir;
        double discriminant = sqr(scalar) - pos_rel.sqr() + sqr(rad);

        if(discriminant < 0){
            return ans;
        }

        for (int sign=-1; sign<=1; sign+=2){
            double dist = ( -scalar + sign*std::sqrt(discriminant));
            if (dist > 0) {
                Vec_3d pos_inter_rel = pos_rel + dist * photon.dir;
                Vec_3d pos_inter = pos_inter_rel + pos;
                Vec_3d normal = pos_inter_rel / pos_inter_rel.len();
                ans.push_back(std::make_pair(pos_inter, normal));
            }
        }
        return ans;
    };
};

class Shape_inversion: public Shape_base{
private:

public:
    Shape_base *shape;

    Shape_inversion(Shape_base *shape): shape(shape) { };

    ~Shape_inversion(){
        delete shape;
    };
    bool point_is_inside(Vec_3d point){
        return !(shape->point_is_inside(point));
    };
    std::vector< std::pair< Vec_3d, Vec_3d > > get_intersections (Photon photon){
        return shape->get_intersections(photon);
    };
};

class Shape_union: public Shape_base{
private:

public:
    Shape_base *shape_1;
    Shape_base *shape_2;

    Shape_union(Shape_base *shape_1, Shape_base *shape_2): shape_1(shape_1), shape_2(shape_2) { };

    ~Shape_union(){
        delete shape_1;
        delete shape_2;
    };
    bool point_is_inside(Vec_3d point){
        return shape_1->point_is_inside(point) && shape_2->point_is_inside(point);
    };
    std::vector< std::pair< Vec_3d, Vec_3d > > get_intersections (Photon photon){
        std::vector< std::pair< Vec_3d, Vec_3d > > ans;
        std::vector< std::pair< Vec_3d, Vec_3d > > ans_1 = shape_1->get_intersections(photon);
        std::vector< std::pair< Vec_3d, Vec_3d > > ans_2 = shape_2->get_intersections(photon);

        for(auto inter : ans_1){
            if (! shape_2->point_is_inside(inter.first) ){
                ans.push_back(inter);
            }
        }
        for(auto inter : ans_2){
            if (! shape_1->point_is_inside(inter.first) ){
                ans.push_back(inter);
            }
        }

        return ans;
    };
};

class Shape_intersection: public Shape_base{
private:

public:
    Shape_base *shape_1;
    Shape_base *shape_2;

    Shape_intersection(Shape_base *shape_1, Shape_base *shape_2): shape_1(shape_1), shape_2(shape_2) { };

    ~Shape_intersection(){
        delete shape_1;
        delete shape_2;
    };
    bool point_is_inside(Vec_3d point){
        return shape_1->point_is_inside(point) && shape_2->point_is_inside(point);
    };
    std::vector< std::pair< Vec_3d, Vec_3d > > get_intersections (Photon photon){
        std::vector< std::pair< Vec_3d, Vec_3d > > ans;
        std::vector< std::pair< Vec_3d, Vec_3d > > ans_1 = shape_1->get_intersections(photon);
        std::vector< std::pair< Vec_3d, Vec_3d > > ans_2 = shape_2->get_intersections(photon);

        for(auto inter : ans_1){
            if (shape_2->point_is_inside(inter.first) ){
                ans.push_back(inter);
            }
        }
        for(auto inter : ans_2){
            if (shape_1->point_is_inside(inter.first) ){
                ans.push_back(inter);
            }
        }

        return ans;
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

///

//class Shape{
//private:
//
//public:
//    virtual ~Shape() = default;
//    virtual double dist(Photon photon) = 0;
//    virtual Vec_3d normal(Photon photon) = 0;
//};
//
//class Rectangle: public Shape{
//private:
//
//public:
//    Vec_3d pos, a, b, dir_normal;
//
//    Rectangle(Vec_3d pos, Vec_3d a, Vec_3d b_):pos(pos), a(a) {
//        b = b_ - ((b_ * a)/a.sqr())*a;
//        dir_normal = Vec_3d(a.y*b.z - b.y*a.z, a.z*b.x - b.z*a.x, a.x*b.y - b.x*a.y);
//        dir_normal /= dir_normal.len();
//    };
//
//    double dist(Photon photon){
//        double ans = - (dir_normal * (photon.pos - pos))/(dir_normal * photon.dir);
//        Vec_3d hit_pos = photon.pos + ans*photon.dir - pos;
//        if( ans < 0 || sqr(hit_pos * a) > sqr(a.sqr()) || sqr(hit_pos * b) > sqr(b.sqr())){
//            return std::numeric_limits<double>::infinity();
//        }
//        return ans;
//    };
//    Vec_3d normal(Photon photon){
//        return dir_normal;
//    };
//};
//
//class Ball: public Shape{
//private:
//
//public:
//    Vec_3d pos;
//    double r;
//
//    Ball(Vec_3d pos, double r):pos(pos), r(r) {};
//
//    double dist(Photon photon){
//        Vec_3d delta_pos = photon.pos - pos;
//        double dist_1 = delta_pos * photon.dir;
//        if (delta_pos.sqr() > r*r && dist_1 > 0){
//            return std::numeric_limits<double>::infinity();
//        }
//        Vec_3d pos_normal = delta_pos - (delta_pos * photon.dir) * photon.dir;
//        if (pos_normal.sqr() > r*r) {
//            return std::numeric_limits<double>::infinity();
//        }
//        double half_chord = std::sqrt(r*r - pos_normal.sqr());
//        double ans = - dist_1 - half_chord;
//        if(ans > 0){
//            return ans;
//        }else{
//            return ans + 2 * half_chord;
//        }
//    };
//    Vec_3d normal(Photon photon){
//        return (photon.pos - pos) / (photon.pos - pos).len();
//    };
//};
//
//class Lens: public Shape{
//private:
//
//public:
//    Vec_3d pos, dir;
//    double r_lens, r_rear, r_front;
//
//    Lens(Vec_3d pos, Vec_3d dir, double r_lens, double r_rear, double r_front):pos(pos), dir(dir), r_lens(r_lens), r_rear(r_rear), r_front(r_front) {
//        dir /= dir.len();
//        if (r_rear < r_lens){
//            r_rear = r_lens;
//        }
//        if (r_front < r_lens){
//            r_front = r_lens;
//        }
//    };
//
//    double dist(Photon photon){
//        Ball ball_tmp_1(pos - std::sqrt(sqr(r_rear) - sqr(r_lens))*dir, r_rear);
//        double dist_1 = ball_tmp_1.dist(photon);
//        if( (photon.pos + dist_1 * photon.dir - pos).sqr() > sqr(r_lens) ){
//            dist_1 = std::numeric_limits<double>::infinity();
//        }
//
//        Ball ball_tmp_2(pos + std::sqrt(sqr(r_front) - sqr(r_lens))*dir, r_front);
//        double dist_2 = ball_tmp_2.dist(photon);
//        if( (photon.pos + dist_2 * photon.dir - pos).sqr() > sqr(r_lens) ){
//            dist_2 = std::numeric_limits<double>::infinity();
//        }
//        if (dist_1 < dist_2){
//            return dist_1;
//        }
//        return dist_2;
//    };
//    Vec_3d normal(Photon photon){
//        Vec_3d delta_pos = photon.pos - pos;
//        if (delta_pos * dir > 0){
//            delta_pos += r_rear  * dir;
//        }else{
//            delta_pos -= r_front * dir;
//        }
//        return delta_pos/delta_pos.len();
//    };
//};
