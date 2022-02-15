#include "geometry.h"

template<> template<> Vec3<int>::Vec3(const Vec3<float>& v)
	: x(v.x + 0.5f), y(v.y + 0.5f), z(v.z + 0.5f) { }
template<> template<> Vec3<float>::Vec3(const Vec3<int>& v)
	: x(v.x), y(v.y), z(v.z) { }