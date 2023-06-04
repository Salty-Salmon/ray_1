#pragma once

#include "Body.hpp"

Shape_base *make_lens(Vec_3d pos, Vec_3d dir, double r_1, double r_2, double r_size);
std::vector<Body *> init_scene_1();
std::vector<Body *> init_scene_2();
std::vector<Body *> init_scene_3();
std::pair<Screen, Body *> make_camera(Vec_3d center, Vec_3d dir, double focus);

Photon source(Vec_3d pos, double r);
Photon cone_source (Vec_3d pos, Vec_3d dir, double theta_max);
