#include "../include/Vec_3d.hpp"

double& Vec_3d::operator[](size_t ind){
    if (ind == 0) {return x;}
    if (ind == 1) {return y;}
    if (ind == 2) {return z;}
    return x;
}

Vec_3d::Vec_3d(double x, double y, double z):x(x), y(y), z(z){}

Vec_3d::Vec_3d():Vec_3d(0, 0, 0){}

Vec_3d Vec_3d::operator+(Vec_3d const &rha) const{
    return Vec_3d(x + rha.x, y + rha.y, z + rha.z);
}

Vec_3d Vec_3d::operator-() const{
    return Vec_3d(-x, -y, -z);
}

Vec_3d Vec_3d::operator-(Vec_3d const &rha) const{
    return *this + (-rha);
}

Vec_3d& Vec_3d::operator+=(Vec_3d const &rha){
    *this = *this + rha;
    return *this;
}

Vec_3d& Vec_3d::operator-=(Vec_3d const &rha){
    *this = *this - rha;
    return *this;
}


Vec_3d Vec_3d::operator*(double const &k) const{
    return Vec_3d(k * this->x, k * this->y, k * this->z);
}

Vec_3d operator*(double const &k, Vec_3d const &rha){
    return rha * k;
}

Vec_3d Vec_3d::operator/(double const &k) const{
    return *this * (1/k);
}

Vec_3d& Vec_3d::operator*=(double const &k){
    *this = *this * k;
    return *this;
}

Vec_3d& Vec_3d::operator/=(double const &k){
    *this = *this / k;
    return *this;
}


double Vec_3d::operator*(Vec_3d const &rha) const{
    return x*rha.x + y*rha.y + z*rha.z;
}

double Vec_3d::sqr() const{
    return (*this) * (*this);
}

double Vec_3d::len() const{
    return std::sqrt(this->sqr());
}

std::ostream& operator<<(std::ostream &os, const Vec_3d &rha) {
    os << "(" << rha.x << ", " << rha.y << ", " << rha.z << ")";
    return os;
}


Vec_3d rotate_a_to_b(Vec_3d a, Vec_3d b, Vec_3d p){
    a /= a.len();
    b /= b.len();
    if((a-b).sqr() == 0){return  p;}
    if((a+b).sqr() == 0){return -p;}
    double a_dot_b = a * b;
    double inv_sin_sqr = 1 / (1 - sqr(a_dot_b));
    double p_dot_a = p * a;
    double p_dot_b = p * b;

    double a_part = (p_dot_a - p_dot_b * a_dot_b) * inv_sin_sqr;
    double b_part = (p_dot_b - p_dot_a * a_dot_b) * inv_sin_sqr;

    Vec_3d normal_part  = p - (a_part * a + b_part * b);
    Vec_3d rotated_part = -b_part * a + (a_part + 2 * a_dot_b * b_part) * b;

    return normal_part + rotated_part;
}

///
double sqr(double x){
    return x*x;
}

double rand_uns(double min, double max) {
    static unsigned seed = std::chrono::steady_clock::now().time_since_epoch().count();
    static std::default_random_engine e(seed);
    std::uniform_real_distribution<double> d(min, max);
    return d(e);
}

Vec_3d rand_unit_vec(){
    Vec_3d ans;
    double eps = 1E-6;
    while(ans.sqr() > 1.0 || ans.sqr() < eps*eps){
        ans.x = rand_uns(-1.0, 1.0);
        ans.y = rand_uns(-1.0, 1.0);
        ans.z = rand_uns(-1.0, 1.0);
    }
    ans /= ans.len();
    return ans;
}
