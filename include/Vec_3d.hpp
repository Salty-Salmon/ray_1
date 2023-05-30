#pragma once

#include <cmath>
#include <iostream>

#include <random>
#include <chrono>

class Vec_3d{
private:

public:
    double x, y, z;
    double &operator[](size_t);

    Vec_3d(double, double, double);
    Vec_3d();

    Vec_3d(Vec_3d const &) = default;
    ~Vec_3d() = default;
    Vec_3d& operator=(Vec_3d const &) = default;

    Vec_3d operator+(Vec_3d const &) const;
    Vec_3d operator-() const;
    Vec_3d operator-(Vec_3d const &) const;
    Vec_3d& operator+=(Vec_3d const &);
    Vec_3d& operator-=(Vec_3d const &);

    Vec_3d operator*(double const &) const;
    friend Vec_3d operator*(double const &, Vec_3d const &);
    Vec_3d operator/(double const &) const;
    Vec_3d& operator*=(double const &);
    Vec_3d& operator/=(double const &);

    double operator*(Vec_3d const &) const;
    double sqr() const;
    double len() const;

    friend std::ostream& operator<<(std::ostream& os, const Vec_3d& rha);
};

Vec_3d rotate_a_to_b(Vec_3d a, Vec_3d b, Vec_3d p);

double rand_uns(double min, double max);
Vec_3d rand_unit_vec();
double sqr(double x);

class Photon{
private:

public:
    Vec_3d pos, dir;
    bool alive;

    Photon(Vec_3d pos, Vec_3d dir): pos(pos), dir(dir/dir.len()), alive(true) {};
    Photon(Vec_3d pos): Photon(pos, rand_unit_vec()) {};
    Photon(): Photon(Vec_3d()) {};
};
