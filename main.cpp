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
    Shape *shape;
    Material *material;
    std::string name;

    Object(Shape *shape, Material *material, std::string name): shape(shape), material(material), name(name) {};
    ~Object(){
        delete shape;
        delete material;
    };

    double dist(Photon photon){
        return shape->dist(photon);
    };
    void interact(Photon &photon){
        material->interact(photon, shape->normal(photon));
    };
};

std::vector<Object *> init_scene_1(){
    std::vector<Object *> scene;
    scene.push_back(new Object(new Ball(Vec_3d(-2, 0, 25), 1.5), new Matted,     "ball_1"));
    scene.push_back(new Object(new Ball(Vec_3d( 2, 0, 25), 1.5), new Reflecting, "ball_2"));
    scene.push_back(new Object(new Lens(Vec_3d( 0, 0,  10), Vec_3d(0, 0, 1), 3, 12, 1E6), new Refracting(3), "lens_1"));

    return scene;
}

Photon source(Vec_3d pos, double r){
    return Photon(pos + rand_unit_vec() * r);
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
    size_t ray_amm = 1E10;
    size_t hit_count = 0;

    size_t width = 1080, height = 1080;
    Pixel *pixels = new Pixel[width*height];

    std::vector<Object *> scene = init_scene_1();
    Rectangle rect(Vec_3d(0, 0, 0), Vec_3d(5, 0, 0), Vec_3d(0, 5, 0));
    Object screen(&rect, new Absorbing, "scene");
    scene.push_back(&screen);

    for (size_t i=1; i<=ray_amm; ++i){
        Photon photon = source(Vec_3d(0, 5, 24), 0.5);
        //Photon photon(Vec_3d(3.0 * i/ray_amm, 0.5, 20), Vec_3d(0, 0, -1));

        Object *closest_obj = nullptr;
        size_t itr = 0;
        while (photon.alive) {
            double min_dist = std::numeric_limits<double>::infinity();
            closest_obj = nullptr;

            for (auto object : scene){
                double dist = object->dist(photon);
                if (min_dist > dist){
                    min_dist = dist;
                    closest_obj = object;
                }
            }

            if (closest_obj != nullptr){
//                std::cout << closest_obj->name << "\n";
//                std::cout << photon.pos << " " << photon.dir << "\n";

                photon.pos += min_dist * photon.dir;
                closest_obj->interact(photon);
                photon.pos += eps * photon.dir;

            }else{
                photon.alive = false;
            }
            ++itr;
            if (itr==10){
                photon.alive = false;
            }
        }

        if (closest_obj == &screen){
            ++hit_count;

            double rel_x = ( 1.0 * ((photon.pos - rect.pos) * rect.a) / rect.a.sqr() + 1.0) / 2.0;
            double rel_y = (-1.0 * ((photon.pos - rect.pos) * rect.b) / rect.b.sqr() + 1.0) / 2.0;

            size_t screen_x = rel_x * width;
            size_t screen_y = rel_y * height;

            pixels[screen_x + height * screen_y].add(1, 0, 0);
        }

        if (i%10000 == 0){
            std::cout << i << "\n";
            std::cout << 100.0 * i/ray_amm << "%" << "\n";
            std::cout << hit_count << "\n";
            std::cout << "\n";
        }
        if (i%(ray_amm/100) == 0){
            print_ppm(pixels, width, height, "pic");
        }
    }

    std::cout << "\n" << hit_count;

    scene.pop_back();
    screen.shape = nullptr;
    for (auto object : scene){
        delete object;
    }
    return 0;
}
