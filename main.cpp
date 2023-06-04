#include <iostream>
#include <cmath>
#include <fstream>
#include <limits>
#include <vector>

#include "include/Vec_3d.hpp"
#include "include/Material.hpp"
#include "include/Shape.hpp"
#include "include/Scene.hpp"

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

enum class Photon_event {stray, screen, object, fog};

int main()
{
    bool fog_present = false;
    double fog_coef = 0.0;

    size_t max_itr = 15;
    size_t *itr_counter = new size_t[max_itr]();
    double eps = 1E-6;
    size_t ray_amm = 5E8;
    size_t hit_count = 0;

    size_t width = 640, height = 640;
    Pixel *pixels = new Pixel[width*height]();

//    std::ifstream in("pic.ppm");
//    {
//        std::string line_1;
//        std::getline(in, line_1);
//    }
//    for (size_t i=0; i<width*height; ++i){
//            in >> pixels[i].r >> pixels[i].g >> pixels[i].b;
//            pixels[i].r = pixels[i].r;
//            pixels[i].g = pixels[i].g;
//            pixels[i].b = pixels[i].b;
//            std::cout << pixels[i].r << " " << pixels[i].g << " " << pixels[i].b << "\n";
//    }
//    in.close();

    std::vector<Body *> scene = init_scene_3();

    Vec_3d camera_pos (-15,  30,  15);
    Vec_3d camera_targ( -3,   0,   6);
    std::pair<Screen, Body *> camera = make_camera(camera_pos, camera_targ-camera_pos, (camera_targ-camera_pos).len());

    Screen screen = camera.first;
    scene.push_back(camera.second);

    for (size_t i=1; i<=ray_amm; ++i){
        Photon photon = cone_source(Vec_3d(-10, 5, 25), Vec_3d(10, -5, -15), std::acos(0)/8);

        size_t itr = 0;
        while (photon.alive && itr < max_itr) {
            ++itr;

            double screen_dist = screen.dist(photon);

            Intersection_point closest_inter;
            Body *closest_body = nullptr;
            for (auto body : scene){
                Intersection_point inter = body->get_intersection(photon);
                if (closest_inter > inter){
                    closest_inter = inter;
                    closest_body = body;
                }
            }

            double fog_dist = std::numeric_limits<double>::infinity();
            if (fog_present){
                fog_dist = -1.0 * std::log(rand_uns(0, 1.0)) / fog_coef;
            }

            double min_dist = std::numeric_limits<double>::infinity();
            Photon_event event = Photon_event::stray;
            if (min_dist > screen_dist){
                min_dist = screen_dist;
                event = Photon_event::screen;
            }
            if (min_dist > closest_inter.dist){
                min_dist = closest_inter.dist;
                event = Photon_event::object;
            }
            if (min_dist > fog_dist){
                min_dist = fog_dist;
                event = Photon_event::fog;
            }

            if(event == Photon_event::stray){
                photon.alive = false;
            }else if(event == Photon_event::screen){
                photon.pos += screen_dist * photon.dir;

                double rel_x = (-1.0 * ((photon.pos - screen.pos) * screen.a) / screen.a.sqr() + 1.0) / 2.0;
                double rel_y = ( 1.0 * ((photon.pos - screen.pos) * screen.b) / screen.b.sqr() + 1.0) / 2.0;
                size_t screen_x = rel_x * width;
                size_t screen_y = rel_y * height;
                pixels[screen_x + width * screen_y].add(1, 0, 0);

                ++hit_count;
                photon.alive = false;
            }else if (event == Photon_event::object){
                photon.pos = closest_inter.pos;
                Vec_3d normal = closest_inter.shape->get_normal(closest_inter.pos);
                closest_body->interact(photon, normal);
                photon.pos += eps * photon.dir;
            }else if (event == Photon_event::fog){
                photon.pos += photon.dir * fog_dist;
                photon.dir = rand_unit_vec();
                //photon.alive = false;
            }
        }
        ++itr_counter[itr-1];
        if (i%100000 == 0){
            std::cout << 100.0 * i/ray_amm << "%" << "\n";
            std::cout << i << "\n";
            std::cout << hit_count << "\n";
            std::cout << 1.0 * hit_count/i << "\n";
            std::cout << "\n";
            for(size_t i=0; i<max_itr; ++i){
                std::cout << i+1 << ":  " << itr_counter[i] << "\n";
            }
            std::cout << "\n\n";
        }
//        if (i%(10 * ray_amm/100) == 0){
//            print_ppm(pixels, width, height, "pic");// + std::to_string(i/(ray_amm/100)));
//        }
    }
    std::cout << "\n" << hit_count << "\n";
    print_ppm(pixels, width, height, "pic");

//    size_t ans = 0;
//    for (size_t i=0; i<width*height; ++i){
//        ans+=pixels[i].r;
//    }
//    std::cout << ans << "\n";

    delete[] pixels;
    for (auto body : scene){
        delete body;
    }
    return 0;
}
