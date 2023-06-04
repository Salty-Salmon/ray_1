#pragma once

#include "Vec_3d.hpp"


class Material{
private:

public:
    virtual ~Material() = default;
    virtual void interact(Photon &photon, Vec_3d normal) = 0;
};

class Transparent: public Material{
private:

public:
    void interact(Photon &photon, Vec_3d normal){

    };
};

class Absorbing: public Material{
private:

public:
    void interact(Photon &photon, Vec_3d normal){
        photon.alive = false;
    };
};

class Lambertian: public Material{
private:

public:
    void interact(Photon &photon, Vec_3d normal){
        double phi = rand_uns(0, 4*std::acos(0));
        double cos_phi = std::cos(phi);
        double sin_phi = std::sin(phi);

        double theta = std::acos(std::sqrt(rand_uns(0, 1)));
        double cos_theta = std::cos(theta);
        double sin_theta = std::sin(theta);

        Vec_3d deviation(sin_theta * cos_phi, sin_theta * sin_phi, cos_theta);
        Vec_3d new_dir = rotate_a_to_b(Vec_3d(0, 0, 1), normal, deviation);

        if ( (new_dir * normal) * (photon.dir * normal) > 0 ){
            new_dir = -new_dir;
        }
        photon.dir = new_dir;
    };
};

class Lambertian_cos: public Material{
private:

public:
    double pow_index;

    Lambertian_cos(double pow_index):pow_index(pow_index) {};

    void interact(Photon &photon, Vec_3d normal){
        double phi = rand_uns(0, 4*std::acos(0));
        double cos_phi = std::cos(phi);
        double sin_phi = std::sin(phi);

        double theta = std::acos(std::pow(rand_uns(0, 1), 1/(2+pow_index)));
        double cos_theta = std::cos(theta);
        double sin_theta = std::sin(theta);

        Vec_3d deviation(sin_theta * cos_phi, sin_theta * sin_phi, cos_theta);
        Vec_3d new_dir = rotate_a_to_b(Vec_3d(0, 0, 1), normal, deviation);

        if ( (new_dir * normal) * (photon.dir * normal) > 0 ){
            new_dir = -new_dir;
        }
        photon.dir = new_dir;
    };
};

class Reflecting: public Material{
private:

public:
    void interact(Photon &photon, Vec_3d normal){
        photon.dir -= 2*(photon.dir*normal) * normal;
    };
};

class Refracting: public Material{
private:

public:
    double refr_ind;

    Refracting(double refr_ind): refr_ind(refr_ind){};

    void interact(Photon &photon, Vec_3d normal){
        double rel_refr_ind = refr_ind;
        if (photon.dir * normal > 0) {
            rel_refr_ind = 1.0/refr_ind;
        }

        Vec_3d dir_tangent = photon.dir - (photon.dir * normal) * normal;
        double discr = 1 - dir_tangent.sqr() / sqr(rel_refr_ind);
        if (discr < 0){
            photon.alive = false;
            return;
        }
        if(normal * photon.dir < 0){
            normal = -normal;
        }
        photon.dir = dir_tangent/rel_refr_ind + std::sqrt(discr)*normal;
    };
};
