#include <vector>
#include <cmath>
#include <cstdlib>
#include <limits>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);

const int width = 800;
const int height = 800;

void line(Vec2i p0, Vec2i p1, TGAImage& image, TGAColor color) {
    bool steep = false;
    if (std::abs(p0.x - p1.x) < std::abs(p0.y - p1.y))
    {   // transpose to make dx > dy
        std::swap(p0.x, p0.y);
        std::swap(p1.x, p1.y);
        steep = true;
    }
    if (p0.x > p1.x)
    {   // change p1 <-> p2 to start from left
        std::swap(p0.x, p1.x);
        std::swap(p0.y, p1.y);
    }

    int dx = p1.x - p0.x;
    int dy = p1.y - p0.y;
    float derror2 = std::abs(dy)*2;
    float error2 = 0;
    int y = p0.y;

    for (int x = p0.x; x <= p1.x; x++)
    {
        if (steep) image.set(y, x, color);
        else image.set(x, y, color);
        error2 += derror2;
        if (error2 > dx)
        {
            y += (p1.y > p0.y ? 1 : -1);
            error2 -= dx * 2;
        }
    }
}

Vec3f barycentric(Vec3f A, Vec3f B, Vec3f C, Vec3f P)
{
    Vec3f s[2];
    for (int i = 2; i--;) 
    {
        s[i][0] = C[i] - A[i];
        s[i][1] = B[i] - A[i];
        s[i][2] = A[i] - P[i];
    }
    Vec3f u = cross(s[0], s[1]);
    
    if (std::abs(u[2]) > 1e-2) // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
        return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
    return Vec3f(-1, 1, 1); // in this case generate negative coordinates, it will be thrown away by the rasterizator
}
void triangle(Vec2i p0, Vec2i p1, Vec2i p2, TGAImage& image, TGAColor color)
{
    line(p0, p1, image, color);
    line(p1, p2, image, color);
    line(p2, p0, image, color);
}

// Scan line algorithm
void filled_triangle(Vec2i p0, Vec2i p1, Vec2i p2, TGAImage& image, TGAColor color)
{
    if (p0.y == p1.y && p0.y == p2.y) return;
    // sort by y(p0, p1, p2)
    if (p0.y > p1.y) std::swap(p0, p1);
    if (p1.y > p2.y) std::swap(p1, p2);
    if (p0.y > p1.y) std::swap(p0, p1);
 
    int height = p2.y - p0.y;

    for (int i = 0; i < height; i++)
    {
        bool upper = i > p1.y - p0.y || p1.y == p0.y;
        int cutted_height = upper ? p2.y - p1.y : p1.y - p0.y;
        float alpha = static_cast<float>(i) / height;
        float beta = static_cast<float>(i - (upper ? p1.y - p0.y : 0)) / cutted_height;
        Vec2i A = p0 + (p2 - p0) * alpha;
        Vec2i B = upper ? p1 + (p2 - p1) * beta : p0 + (p1 - p0) * beta;
        if (A.x > B.x) std::swap(A, B);
        for (int j = A.x; j <= B.x; j++)
        {
            image.set(j, p0.y + i, color);
        }
    }
}

void triangle(Vec3f* pts, float* zbuffer, TGAImage& image, TGAColor color)
{
    Vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
    Vec2f clamp(image.get_width() - 1, image.get_height() - 1);
    for (int i = 0; i < 3; i++) 
    {
        for (int j = 0; j < 2; j++) 
        {
            bboxmin[j] = std::max(0.0f, std::min(bboxmin[j], pts[i][j]));
            bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
        }
    }
    Vec3f P;
    for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++)
    {
        for (P.y = bboxmin.y; P.y <= bboxmin.y; P.y++)
        {
            Vec3f bc = barycentric(pts[0], pts[1], pts[2], P);
            if (bc.x < 0 || bc.y < 0 || bc.z < 0) continue;
            P.z = 0;
            for (int i = 0; i < 3; i++)
            {
                P.z += pts[i][2] * bc[i]; // find z-index in P with u,v 
            }
            if (zbuffer[static_cast<int>(P.x + P.y * width)] < P.z)
            {
                zbuffer[static_cast<int>(P.x + P.y * width)] = P.z;
                image.set(P.x, P.y, color);
            }
        }
    }

}

Vec3f world2screen(Vec3f v) 
{
    return Vec3f(int((v.x + 1.) * width / 2. + .5), int((v.y + 1.) * height / 2. + .5), v.z);
}
int main(int argc, char** argv) 
{
    TGAImage image(width, height, TGAImage::RGB);

    Model* model = new Model("obj\\african_head.obj");

    Vec3f light_dir(0, 0, -1);

    float* zbuffer = new float[width * height];
    for (int i = width * height; i--;)
    {
        zbuffer[i] = -std::numeric_limits<float>::max();
    }

    for (int i = 0; i < model->nfaces(); i++)
    {
        std::vector<int> face = model->face(i);
        Vec3f pts[3];
        for (int j = 0; j < 3; j++)
        {
            pts[j] = world2screen(model->vert(face[j]));
        }
        triangle(pts, zbuffer, image, TGAColor(rand() % 255, rand() % 255, rand() % 255, 255));
    }
    image.flip_vertically();
    image.write_tga_file("output\\output9_hidden-face-removal.tga");

    delete model;

    return 0;
}
