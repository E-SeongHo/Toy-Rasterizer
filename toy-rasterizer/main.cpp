#include <vector>
#include <cmath>
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



int main(int argc, char** argv) 
{
    TGAImage image(width, height, TGAImage::RGB);

    Model* model = new Model("obj\\african_head.obj");

    Vec3f light_dir(0, 0, -1);

    for (int i = 0; i < model->nfaces(); i++)
    {
        std::vector<int> face = model->face(i);
        Vec2i screen_coords[3];
        Vec3f world_coords[3];
        for (int j = 0; j < 3; j++)
        {
            Vec3f v = model->vert(face[j]);
            screen_coords[j] = Vec2i((v.x + 1.0) * width / 2.0, (v.y + 1.0) * height / 2.0);
            world_coords[j] = v;
        }
        Vec3f n = (world_coords[2] - world_coords[1]) ^ (world_coords[1] - world_coords[0]); // cross product
        n.normalize();
        float intensity = n * light_dir; // dot product
        if (intensity > 0)
        {
            filled_triangle(screen_coords[0], screen_coords[1], screen_coords[2], image, TGAColor(intensity * 255, intensity * 255, intensity*255, 255));
        }

    }

    image.flip_vertically();
    image.write_tga_file("output\\output8_gouraud-shading.tga");

    delete model;

    return 0;
}
