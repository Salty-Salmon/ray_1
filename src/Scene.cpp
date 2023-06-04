#include "../include/Scene.hpp"

Shape_base *make_lens(Vec_3d pos, Vec_3d dir, double r_1, double r_2, double r_size){
    dir /= dir.len();
    double dist_1 = std::sqrt(sqr(r_1) - sqr(r_size));
    double dist_2 = std::sqrt(sqr(r_2) - sqr(r_size));
    Shape_ball *ball_1 = new Shape_ball(pos - dist_1 * dir, r_1);
    Shape_ball *ball_2 = new Shape_ball(pos + dist_2 * dir, r_2);
    Shape_intersection *inter_1 = new Shape_intersection(ball_1, ball_2);
    return inter_1;
}

std::vector<Body *> init_scene_1(){
    Shape_base *lens_1 = make_lens(Vec_3d(0, -6, 0), Vec_3d(0, 1, 0), 9, 9, 3);
    Body *body_1 = new Body(lens_1, new Refracting(2.5), "lens_1");

    Shape_base *lens_2 = make_lens(Vec_3d(0,  6, 0), Vec_3d(0, 1, 0), 9, 9, 3);
    Body *body_2 = new Body(lens_2, new Refracting(2.5), "lens_2");

    Shape_base *plane_1 = new Shape_plane(Vec_3d(0, 12, 0), Vec_3d(0, 1, 0));
    Body *body_3 = new Body(plane_1, new Lambertian, "surf");

    std::vector<Body *> scene;
    scene.push_back(body_1);
    scene.push_back(body_2);
    scene.push_back(body_3);
    return scene;
}

std::vector<Body *> init_scene_2(){
    double pow_index = 0.5;

    Shape_plane *plane_0 = new Shape_plane(Vec_3d(0, 0, 0), Vec_3d(0, 0, -1));
    Body *body_0 = new Body(plane_0, new Lambertian, "surf");

    Shape_ball *ball_1 = new Shape_ball(Vec_3d(-2, -2, 1), 2);
    Body *body_1 = new Body(ball_1, new Lambertian_cos(pow_index), "ball_1");

    Shape_ball *ball_2 = new Shape_ball(Vec_3d(-2, +2, 1), 2);
    Body *body_2 = new Body(ball_2, new Lambertian_cos(pow_index), "ball_2");

    Shape_ball *ball_3 = new Shape_ball(Vec_3d(0, 0, 9), 2);
    Body *body_3 = new Body(ball_3, new Lambertian_cos(pow_index), "ball_3");

    Shape_cylinder  *cylinder_1 = new Shape_cylinder(Vec_3d(    0, 0.0, 0.0), Vec_3d(0, 0, 1), 2);
    Shape_plane        *plane_1 = new Shape_plane(Vec_3d(0, 0, 9), Vec_3d(0, 0, 1));
    Shape_intersection *inter_1 = new Shape_intersection(cylinder_1, plane_1);
    Body *body_4 = new Body(inter_1, new Lambertian_cos(pow_index), "cyl");

    std::vector<Body *> scene;
    scene.push_back(body_0);
    scene.push_back(body_1);
    scene.push_back(body_2);
    scene.push_back(body_3);
    scene.push_back(body_4);
    return scene;
}

std::vector<Body *> init_scene_3(){
    Shape_plane *plane_0 = new Shape_plane(Vec_3d(0, 0, 0), Vec_3d(0, 0, -1));
    Body *body_0 = new Body(plane_0, new Lambertian, "surf");

    Shape_plane *plane_1 = new Shape_plane(Vec_3d( 0,  0,  8), Vec_3d( 0,  0, -1));
    Shape_plane *plane_2 = new Shape_plane(Vec_3d( 0,  0, 12), Vec_3d( 0,  0, +1));
    Shape_plane *plane_3 = new Shape_plane(Vec_3d(+2,  0, 10), Vec_3d(+1,  0,  0));
    Shape_plane *plane_4 = new Shape_plane(Vec_3d(-2,  0, 10), Vec_3d(-1,  0,  0));
    Shape_plane *plane_5 = new Shape_plane(Vec_3d( 0, +2, 10), Vec_3d( 0, +1,  0));
    Shape_plane *plane_6 = new Shape_plane(Vec_3d( 0, -2, 10), Vec_3d( 0, -1,  0));

    Shape_intersection *inter_1 = new Shape_intersection(plane_1, plane_2);
    Shape_intersection *inter_2 = new Shape_intersection(plane_3, plane_4);
    Shape_intersection *inter_3 = new Shape_intersection(plane_5, plane_6);
    Shape_intersection *inter_4 = new Shape_intersection(inter_1, inter_2);
    Shape_intersection *cube_1  = new Shape_intersection(inter_3, inter_4);

    Shape_ball *ball_1 = new Shape_ball(Vec_3d(-2, 2, 12), 2);
    Shape_inversion *inv_1 = new Shape_inversion(ball_1);

    Shape_intersection *inter_5 = new Shape_intersection(cube_1, inv_1);

    Body *body_1 = new Body(inter_5, new Lambertian, "body_1");

    Shape_cylinder *cyl_1 = new Shape_cylinder(Vec_3d(-8, -10, 0), Vec_3d(0, 0, 1), 4);

    Body *body_2 = new Body(cyl_1, new Reflecting, "cyl_1");

    std::vector<Body *> scene;
    scene.push_back(body_0);
    scene.push_back(body_1);
    scene.push_back(body_2);
    return scene;
}

std::pair<Screen, Body *> make_camera(Vec_3d center, Vec_3d dir, double focus){
    dir /= dir.len();
    double r_1 = 15;
    double r_2 = 15;
    double refr_ind = 2.5;
    double focal_dist = 1 / ((refr_ind - 1) * (1/r_1 + 1/r_2));
    double lens_dist = (focus - std::sqrt(sqr(focus) - 4*focus*focal_dist)) / 2;

    Shape_base *lens_shape = make_lens(center + lens_dist * dir, dir, r_1, r_2, 4);
    Body *lens_1 = new Body(lens_shape, new Refracting(2.5), "lens_1");

    double screen_width  = 3;
    double screen_height = 3;

    Vec_3d screen_a(0, screen_width, 0);
    Vec_3d screen_b(0, 0, screen_height);

    Vec_3d z_axis(0, 0, 1);
    Vec_3d dir_hor = dir - (dir*z_axis)*z_axis / z_axis.sqr();
    Vec_3d dir_ver = rotate_a_to_b(dir_hor, Vec_3d(1, 0, 0), dir);

    screen_a = rotate_a_to_b(Vec_3d(1, 0, 0), dir_hor, screen_a);
    screen_b = rotate_a_to_b(Vec_3d(1, 0, 0), dir_ver, screen_b);

    Screen screen(center, screen_a, screen_b);

    return std::make_pair(screen, lens_1);
}

Photon source(Vec_3d pos, double r){
    return Photon(pos + rand_unit_vec() * r);
}

Photon cone_source (Vec_3d pos, Vec_3d dir, double theta_max){
    return Photon(pos, rand_unit_segment(dir, theta_max));
}
