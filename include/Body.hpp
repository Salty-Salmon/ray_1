#pragma once

#include "Material.hpp"
#include "Shape.hpp"

#include <algorithm>

class Body{
private:

public:
    Shape_base *shape;
    Material *material;
    std::string name;

    std::vector< Intersection_point > intersections;

    Body(Shape_base *shape, Material *material, std::string name): shape(shape), material(material), name(name) {};

    ~Body(){
        delete shape;
        delete material;
    };

    Intersection_point get_intersection(Photon photon){
        intersections.clear();
        shape->get_intersections(photon, intersections);

        std::sort(intersections.begin(), intersections.end());

        for (auto inter : intersections){
            if(shape->point_is_inside(inter)){
                return inter;
            }
        }
        return Intersection_point();
    };
    void interact(Photon &photon, Vec_3d normal){
        material->interact(photon, normal);
    };
};
