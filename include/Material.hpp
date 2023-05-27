#pragma once

#include "Vec_3d.hpp"


class Material{
private:

public:
    virtual ~Material() = default;
    virtual void interact(Photon &photon, Vec_3d normal) = 0;
};

class Absorbing: public Material{
private:

public:
    void interact(Photon &photon, Vec_3d normal){
        photon.alive = false;
    };
};

class Matted: public Material{
private:

public:
    void interact(Photon &photon, Vec_3d normal){
        photon.dir = rand_unit_vec();
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
