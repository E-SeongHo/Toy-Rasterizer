#pragma once

#include "geometry.h"
#include "tgaimage.h"

extern Matrix ModelView;
extern Matrix Projection;
extern Matrix ViewPort;

Matrix lookat(Vec3f eye, Vec3f center, Vec3f up);

Matrix projection(float coeff);

Matrix viewport(int x, int y, int w, int h);

struct IShader
{
	virtual ~IShader() {}
	virtual Vec3f vertex(int iface, int nvert) = 0;
	virtual bool fragment(Vec3f bar, TGAColor& color) = 0;
};

Vec3f barycentric(Vec3f A, Vec3f B, Vec3f C, Vec3f P);

void line(Vec2i p0, Vec2i p1, TGAImage& image, TGAColor color);

void triangleLines(Vec2i p0, Vec2i p1, Vec2i p2, TGAImage& image, TGAColor color);

void triangle(Vec3i* pts, IShader& shader, TGAImage& image, TGAImage& zbuffer);

