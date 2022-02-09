#include <vector>
#include <cmath>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);

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

void filled_triangle(Vec2i p0, Vec2i p1, Vec2i p2, TGAImage& image, TGAColor color)
{
    // sort by y(p0, p1, p2)
    if (p0.y > p1.y) std::swap(p0, p1);
    if (p1.y > p2.y) std::swap(p1, p2);
    if (p0.y > p1.y) std::swap(p0, p1);
 
    int height = p2.y - p0.y;

    // upper side
    for (int y = p2.y; y >= p1.y; y--)
    {
        int upper_height = p2.y - p1.y + 1;
        float alpha = float(p2.y - y) / height;
        float beta = float(p2.y - y) / upper_height;

        Vec2i A = p0 * alpha + p2 * (1 - alpha);
        Vec2i B = p1 * beta + p2 * (1 - beta);
        if (A.x > B.x) std::swap(A.x, B.x);
        for (int i = A.x; i <= B.x; i++)
            image.set(i, y, color);
    }

    // lower side
    for (int y = p0.y; y <= p1.y; y++)
    {
        int lower_hegiht = p1.y - p0.y;
        float alpha = float(y - p0.y) / height;
        float beta = float(y - p0.y) / lower_hegiht;

        Vec2i A = p2 * alpha + p0 * (1 - alpha);
        Vec2i B = p1 * beta + p0 * (1 - beta);
        if (A.x > B.x) std::swap(A.x, B.x);
        for (int i = A.x; i <= B.x; i++)
            image.set(i, y, color);
    }
}



int main(int argc, char** argv) 
{
    TGAImage image(200, 200, TGAImage::RGB);

    Vec2i t0[3] = { Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80) };
    Vec2i t1[3] = { Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180) };
    Vec2i t2[3] = { Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180) };
    filled_triangle(t0[0], t0[1], t0[2], image, red);
    filled_triangle(t1[0], t1[1], t1[2], image, white);
    filled_triangle(t2[0], t2[1], t2[2], image, green);

    image.flip_vertically();
    image.write_tga_file("output\\output6.tga");

    return 0;
}
