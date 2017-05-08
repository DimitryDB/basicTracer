#ifndef VEC_H
#define VEC_H

#include <cmath>
#include <cstdlib>
#include <iostream>

// basic vector class
class vec3 {
public:
	vec3() {}
	vec3(double e0, double e1, double e2) { e[0] = e0; e[1] = e1; e[2] = e2; }
	double x() const { return e[0]; }
	double y() const { return e[1]; }
	double z() const { return e[2]; }
	double r() const { return e[0]; }
	double g() const { return e[1]; }
	double b() const { return e[2]; }

	const vec3& operator+() const { return *this; }
	vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
	double operator[] (int i) const { return e[i]; }
	double& operator[] (int i) { return e[i]; }
    operator double() const { return (x() + y() + z()) / 3; }

	vec3& operator+=(const vec3& v2);
	vec3& operator-=(const vec3& v2);
	vec3& operator*=(const vec3& v2);
	vec3& operator*=(const double t);
	vec3& operator/=(const vec3& v2);
	vec3& operator/=(const double t);
	
	double length() const {
		return sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]); }
	double squared_length() const {
		return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];	}
	void make_unti_vector();
	double e[3];
};

// function implementations
inline std::istream& operator >> (std::istream &is, vec3 &t) {
	is >> t.e[0] >> t.e[1] >> t.e[2];
	return is;
}
inline std::istream& operator >> (std::istream &is, vec3 &&t) {
	is >> t.e[0] >> t.e[1] >> t.e[2];
	return is;
}
inline std::ostream& operator << (std::ostream &os, vec3 &&t) {
	os << t.e[0] << " " << t.e[1] << " " << t.e[2];
	return os;
}
inline std::ostream& operator << (std::ostream &os, vec3 &t) {
	os << t.e[0] << " " << t.e[1] << " " << t.e[2];
	return os;
}
inline void vec3::make_unti_vector() {
	double k = 1.0 / vec3::length();
	e[0] *= k; e[1] *= k; e[2] *= k;
}
inline vec3 operator+(const vec3& v1, const vec3& v2) {
	return vec3(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1], v1.e[2] + v2.e[2]);
}
inline vec3 operator-(const vec3& v1, const vec3& v2) {
	return vec3(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1], v1.e[2] - v2.e[2]);
}
inline vec3 operator*(const vec3& v1, const vec3& v2) {
	return vec3(v1.e[0] * v2.e[0], v1.e[1] * v2.e[1], v1.e[2] * v2.e[2]);
}
inline vec3 operator*(const double t , const vec3& v) {
	return vec3(v.e[0] * t, v.e[1] * t, v.e[2] * t);
}
inline vec3 operator*(const vec3& v, const double t ) {
	return vec3(v.e[0] * t, v.e[1] * t, v.e[2] * t);
}
inline vec3 operator/(const vec3& v1, const vec3& v2) {
	return vec3(v1.e[0] / v2.e[0], v1.e[1] / v2.e[1], v1.e[2] / v2.e[2]);
}
inline vec3 operator/(const vec3& v, const double t) {
	return vec3(v.e[0] / t, v.e[1] / t, v.e[2] / t);
}
inline double dot(const vec3& v1, const vec3& v2) {
	return v1.e[0] * v2.e[0] + v1.e[1] * v2.e[1] + v1.e[2] * v2.e[2];
}
inline vec3 cross(const vec3& v1, const vec3& v2) {
	return vec3((v1.e[1] * v2.e[2] - v1.e[2] * v2.e[1]),
				(-(v1.e[0] * v2.e[2] - v1.e[2] * v2.e[0])),
				(v1.e[0] * v2.e[1] - v1.e[1] * v2.e[0]));
}
inline vec3& vec3::operator+=(const vec3& v2) {
	e[0] += v2.e[0];
	e[1] += v2.e[1];
	e[2] += v2.e[2];
    return *this;
}
inline vec3& vec3::operator-=(const vec3& v2) {
	e[0] -= v2.e[0];
	e[1] -= v2.e[1];
	e[2] -= v2.e[2];
    return *this;
}
inline vec3& vec3::operator*=(const vec3& v2) {
	e[0] *= v2.e[0];
	e[1] *= v2.e[1];
	e[2] *= v2.e[2];
    return *this;
}
inline vec3& vec3::operator*=(const double t) {
	e[0] *= t;
	e[1] *= t;
	e[2] *= t;
    return *this;
}
inline vec3& vec3::operator/=(const double t) {
	e[0] /= t;
	e[1] /= t;
	e[2] /= t;
    return *this;
}
inline vec3& vec3::operator/=(const vec3& v2) {
	e[0] /= v2.e[0];
	e[1] /= v2.e[1];
	e[2] /= v2.e[2];
    return *this;
}
inline vec3 unit_vector(vec3 v) {
	return v / v.length();
}

#endif // VEC_H
