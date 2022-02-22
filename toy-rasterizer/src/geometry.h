#pragma once

#include <cmath>
#include <iostream>
#include <vector>

class Matrix;

template <typename T> struct Vec2
{
	union 
	{
		struct { T u, v; }; // about texture element (texel)
		struct { T x, y; }; 
		T raw[2];
	};
	Vec2() : u(0), v(0) {}
	Vec2(T _u, T _v) : u(_u), v(_v) {}
	inline Vec2<T> operator+(const Vec2<T>& V) const { return Vec2<T>(u + V.u, v + V.v); }
	inline Vec2<T> operator-(const Vec2<T>& V) const { return Vec2<T>(u - V.u, v - V.v); }
	inline Vec2<T> operator*(float f) const { return Vec2<T>(u * f, v * f); }
	inline T& operator[](const int i) { return i==0 ? x : y; }
	template <typename T> friend std::ostream& operator<<(std::ostream& s, Vec2<T>& v);
};

template <typename T> struct Vec3 {
	union
	{
		struct { T x, y, z; };
		struct { T ivert, iuv, inorm; };
		T raw[3];
	};
	Vec3() : x(0), y(0), z(0) {}
	Vec3(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}
	Vec3(Matrix m);
	template <typename U> Vec3(const Vec3<U>& v);

	inline Vec3<T> operator ^(const Vec3<T>& v) const { return Vec3<T>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); }
	inline Vec3<T> operator +(const Vec3<T>& v) const { return Vec3<T>(x + v.x, y + v.y, z + v.z); }
	inline Vec3<T> operator -(const Vec3<T>& v) const { return Vec3<T>(x - v.x, y - v.y, z - v.z); }
	inline Vec3<T> operator *(float f)          const { return Vec3<T>(x * f, y * f, z * f); }
	inline T       operator *(const Vec3<T>& v) const { return x * v.x + y * v.y + z * v.z; }
	inline T&	   operator[](const int i)		 { return i == 0 ? x : (i == 1 ? y : z); }
	
	float norm() const { return std::sqrt(x * x + y * y + z * z); }
	Vec3<T>& normalize(T l = 1) { *this = (*this) * (l / norm()); return *this; }
	
	
	template <typename T> friend std::ostream& operator<<(std::ostream& s, Vec3<T>& v);
};

typedef Vec2<float> Vec2f;
typedef Vec2<int>   Vec2i;
typedef Vec3<float> Vec3f;
typedef Vec3<int>   Vec3i;

template<> template<> Vec3<int>::Vec3(const Vec3<float>& v);
template<> template<> Vec3<float>::Vec3(const Vec3<int>& v);

template <typename T> std::ostream& operator<<(std::ostream& s, Vec2<T>& v) {
	s << "(" << v.x << ", " << v.y << ")\n";
	return s;
}

template <typename T> std::ostream& operator<<(std::ostream& s, Vec3<T>& v) {
	s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
	return s;
}

template <typename T> Vec3<T> cross(Vec3<T> v1, Vec3<T> v2) {
	return Vec3<T>(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
}


class Matrix
{
private:
	std::vector<std::vector<float>> m;
	int rows, cols;

public:
	Matrix(int r = 4, int c = 4);
	Matrix(Vec3f v);
	inline int nrows();
	inline int ncols();

	static Matrix identity(int dimensions);

	std::vector<float>& operator[](const int i); 
	Matrix operator*(const Matrix& a);

	Matrix transpose();
	Matrix inverse();

	friend std::ostream& operator<<(std::ostream& s, Matrix& m);
};

