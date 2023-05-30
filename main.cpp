#include <iostream>
#include <cmath>
#include <fstream>
#include <limits>
#include <vector>

#include "include/Vec_3d.hpp"
#include "include/Material.hpp"
#include "include/Shape.hpp"

class Object{
private:

public:
    Shape_base *shape;
    Material *material;
    std::string name;

    Object(Shape_base *shape, Material *material, std::string name): shape(shape), material(material), name(name) {};
    ~Object(){
        delete shape;
        delete material;
    };

    std::vector< std::pair< Vec_3d, Vec_3d > > get_intersections(Photon photon){
        return shape->get_intersections(photon);
    };
    void interact(Photon &photon, Vec_3d normal){
        material->interact(photon, normal);
    };
};

Shape_base *make_lens(Vec_3d pos, Vec_3d dir, double r_1, double r_2, double r_size){
    dir /= dir.len();
    double dist_1 = std::sqrt(sqr(r_1) - sqr(r_size));
    double dist_2 = std::sqrt(sqr(r_2) - sqr(r_size));
    Shape_ball *ball_1 = new Shape_ball(pos - dist_1 * dir, r_1);
    Shape_ball *ball_2 = new Shape_ball(pos + dist_2 * dir, r_2);
    Shape_intersection *inter_1 = new Shape_intersection(ball_1, ball_2);
    return inter_1;
}

std::vector<Object *> init_scene_1(){
    Shape_base *lens_shape = make_lens(Vec_3d(-30, 0, 5), Vec_3d(1, 0, 0), 15, 15, 4);
    Object *lens_1 = new Object(lens_shape, new Refracting(2.5), "lens_1");

    Shape_cylinder     *cylinder_1 = new Shape_cylinder(Vec_3d(0, 0, 0), Vec_3d(0, 0, 1), 2);
    Shape_plane        *plane_1    = new Shape_plane(Vec_3d(0, 0, 8), Vec_3d(2, -1, -1));
    Shape_intersection *inter_1    = new Shape_intersection(cylinder_1, plane_1);
    Shape_plane        *plane_2    = new Shape_plane(Vec_3d(0, 0, 0), Vec_3d(0, 0, -1));
    //Shape_ball         *ball_1     = new Shape_ball(Vec_3d(-5, 5, 4), 5);
    //Shape_inversion    *ball_1_inv = new Shape_inversion(ball_1);
    //Shape_intersection *inter_2    = new Shape_intersection(plane_2, ball_1_inv);
    Shape_union        *union_1    = new Shape_union(inter_1, plane_2);

    Object *obj_1 = new Object(union_1, new Strictly_matted, "obj_1");

    std::vector<Object *> scene;
    scene.push_back(lens_1);
    scene.push_back(obj_1);
    return scene;
}

std::vector<Object *> init_scene_2(){
    Shape_base *lens_shape = make_lens(Vec_3d(-30, -5, 5), Vec_3d(1, 0, 0), 15, 15, 5);
    Object *lens_1 = new Object(lens_shape, new Refracting(2.5), "lens_1");

    Shape_plane *plane_1 = new Shape_plane(Vec_3d(0, 0, 0), Vec_3d(0, 0, -1));
    Object *obj_1 = new Object(plane_1, new Strictly_matted, "obj_1");

    Shape_cylinder     *cylinder_1 = new Shape_cylinder(Vec_3d(0, 0, 0), Vec_3d(0, 0, 1), 4);
//    Shape_plane        *plane_1    = new Shape_plane(Vec_3d(0, 0, 8), Vec_3d(2, -1, -1));
//    Shape_intersection *inter_1    = new Shape_intersection(cylinder_1, plane_1);
    Object *obj_2 = new Object(cylinder_1, new Refracting(1.5), "obj_2");

    std::vector<Object *> scene;
    scene.push_back(lens_1);
    scene.push_back(obj_1);
    scene.push_back(obj_2);
    return scene;
}


Photon source(Vec_3d pos, double r){
    return Photon(pos + rand_unit_vec() * r);
}

Photon cone_source (Vec_3d pos, Vec_3d dir, double theta_max){
    double phi = rand_uns(0, 4*std::acos(0));
    double cos_phi = std::cos(phi);
    double sin_phi = std::sin(phi);

    double cos_theta = rand_uns(std::cos(theta_max), 1);
    double sin_theta = std::sqrt(1 - sqr(cos_theta));

    Vec_3d deviation(sin_theta * cos_phi, sin_theta * sin_phi, cos_theta);
    Vec_3d dir_ans = rotate_a_to_b(Vec_3d(0, 0, 1), dir, deviation);

    return Photon(pos, dir_ans);
}

struct Pixel{
    unsigned int r, g, b;

    void add(unsigned char r_add, unsigned char g_add, unsigned char b_add){
        r += r_add;
        g += g_add;
        b += b_add;

        if (r>255) r = 255;
        if (g>255) g = 255;
        if (b>255) b = 255;
    }
};

void print_ppm(Pixel *pixels, int width, int height, std::string name){
    std::ofstream out(name + ".ppm");
    out << "P3" << " " << width << " " << height << " " << "255" << "\n";
    for (int i_y=0; i_y<height; ++i_y){
        for (int i_x=0; i_x<width; ++i_x){
            Pixel curr = pixels[i_x + width * i_y];
            out << curr.r << " " << curr.g << " " << curr.b << " \n";
        }
    }
    out.close();
}

int main()
{
    double eps = 1E-6;
    size_t ray_amm = 2E8;
    size_t hit_count = 0;

    size_t width = 640, height = 640;
    Pixel *pixels = new Pixel[width*height]();

    std::vector<Object *> scene = init_scene_2();
    Vec_3d screen_pos = Vec_3d(-30, -5, 5) - 6 * (Vec_3d(1, 0, 0) / Vec_3d(1, 0, 0).len());
    Screen screen(screen_pos, Vec_3d(0, 3, 0), Vec_3d(0, 0, -3));

    for (size_t i=1; i<=ray_amm; ++i){
        //Photon photon = source(Vec_3d(0, 10, 10), 0);
        Photon photon = cone_source(Vec_3d(0, 20, 20), Vec_3d(0, -20, 4-20), std::acos(0)/3 );
        //Photon photon(Vec_3d(-10, 20, 20), Vec_3d(7, -15, -20) + 10 * rand_unit_vec());


        size_t itr = 0;
        while (photon.alive && itr < 10) {
            ++itr;

            double min_dist_sqr = std::numeric_limits<double>::infinity();

            Vec_3d closest_inter(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity());
            Vec_3d closest_normal(1, 0, 0);
            Object *closest_obj = nullptr;

            for (auto object : scene){
                std::vector< std::pair< Vec_3d, Vec_3d > > intersections = object->get_intersections(photon);
                for (auto inter : intersections){
                    double dist_sqr = (inter.first - photon.pos).sqr();
                    if (min_dist_sqr > dist_sqr){
                        min_dist_sqr = dist_sqr;

                        closest_inter  = inter.first;
                        closest_normal = inter.second;
                        closest_obj = object;
                    }
                }
            }

            double screen_dist = screen.dist(photon);
            if(sqr(screen_dist) < min_dist_sqr){
                photon.pos += screen_dist * photon.dir;

                ++hit_count;

                double rel_x = ( 1.0 * ((photon.pos - screen.pos) * screen.a) / screen.a.sqr() + 1.0) / 2.0;
                double rel_y = (-1.0 * ((photon.pos - screen.pos) * screen.b) / screen.b.sqr() + 1.0) / 2.0;

                size_t screen_x = rel_x * width;
                size_t screen_y = rel_y * height;

                pixels[screen_x + width * screen_y].add(16, 0, 0);
                photon.alive = false;
            }else if (closest_obj != nullptr){
                //std::cout << min_dist_sqr << "\n";

                photon.pos = closest_inter;
                closest_obj->interact(photon, closest_normal);
                photon.pos += eps * photon.dir;

            }else{
                photon.alive = false;
            }
        }

        if (i%100000 == 0){
            std::cout << i << "\n";
            std::cout << 100.0 * i/ray_amm << "%" << "\n";
            std::cout << hit_count << "\n";
            std::cout << "\n";
        }
        if (i%(10 * ray_amm/100) == 0){
            print_ppm(pixels, width, height, "pic");// + std::to_string(i/(ray_amm/100)));
        }
    }

    std::cout << "\n" << hit_count;

    pixels[width/2 + width * (height/2)].g = 255;

    print_ppm(pixels, width, height, "pic");

    delete[] pixels;
    for (auto object : scene){
        delete object;
    }
    return 0;
}
