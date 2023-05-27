#pragma once

#include "Vec_3d.hpp"


class Shape{
private:

public:
    virtual ~Shape() = default;
    virtual double dist(Photon photon) = 0;
    virtual Vec_3d normal(Photon photon) = 0;
};

class Rectangle: public Shape{
private:

public:
    Vec_3d pos, a, b, dir_normal;

    Rectangle(Vec_3d pos, Vec_3d a, Vec_3d b_):pos(pos), a(a) {
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

class Ball: public Shape{
private:

public:
    Vec_3d pos;
    double r;

    Ball(Vec_3d pos, double r):pos(pos), r(r) {};

    double dist(Photon photon){
        Vec_3d delta_pos = photon.pos - pos;
        double dot_1 = delta_pos * photon.dir;
        if (delta_pos.sqr() > r*r && dot_1 > 0){
            return std::numeric_limits<double>::infinity();
        }
        Vec_3d pos_normal = delta_pos - (delta_pos * photon.dir) * photon.dir;
        if (pos_normal.sqr() > r*r) {
            return std::numeric_limits<double>::infinity();
        }
        double half_chord = std::sqrt(r*r - pos_normal.sqr());
        double ans = - dot_1 - half_chord;
        if(ans > 0){
            return ans;
        }else{
            return ans + 2 * half_chord;
        }
    };
    Vec_3d normal(Photon photon){
        return (photon.pos - pos) / (photon.pos - pos).len();
    };
};

class Lens: public Shape{
private:

public:
    Vec_3d pos, dir;
    double r_lens, r_rear, r_front;

    Lens(Vec_3d pos, Vec_3d dir, double r_lens, double r_rear, double r_front):pos(pos), dir(dir), r_lens(r_lens), r_rear(r_rear), r_front(r_front) {
        dir /= dir.len();
        if (r_rear < r_lens){
            r_rear = r_lens;
        }
        if (r_front < r_lens){
            r_front = r_lens;
        }
    };

    double dist(Photon photon){
        Ball ball_tmp_1(pos - std::sqrt(sqr(r_rear) - sqr(r_lens))*dir, r_rear);
        double dist_1 = ball_tmp_1.dist(photon);
        if( (photon.pos + dist_1 * photon.dir - pos).sqr() > sqr(r_lens) ){
            dist_1 = std::numeric_limits<double>::infinity();
        }

        Ball ball_tmp_2(pos + std::sqrt(sqr(r_front) - sqr(r_lens))*dir, r_front);
        double dist_2 = ball_tmp_2.dist(photon);
        if( (photon.pos + dist_2 * photon.dir - pos).sqr() > sqr(r_lens) ){
            dist_2 = std::numeric_limits<double>::infinity();
        }
        if (dist_1 < dist_2){
            return dist_1;
        }
        return dist_2;
    };
    Vec_3d normal(Photon photon){
        Vec_3d delta_pos = photon.pos - pos;
        if (delta_pos * dir > 0){
            delta_pos += r_rear  * dir;
        }else{
            delta_pos -= r_front * dir;
        }
        return delta_pos/delta_pos.len();
    };
};
