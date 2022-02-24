#include <cmath>
#include <cstdlib>
#include <limits>
#include "model.h"
#include "myGL.h"

const int width = 800;
const int height = 800;

Vec3f light_dir = Vec3f(1, -1, 1).normalize();
Vec3f camera(1, 1, 3);
Vec3f center(0, 0, 0);

Model* model = nullptr;

struct Shader : IShader
{
    Vec2i varying_uv[3];
    float varying_intensity[3];

    virtual ~Shader() {}
    virtual Vec3f vertex(int iface, int nvert)
    {
        varying_uv[nvert] = model->uv(iface, nvert);
        varying_intensity[nvert] = model->norm(iface, nvert) * light_dir;

        Vec3f vertex = model->vert(iface, nvert);
        Vec3f transformed_vertex = ViewPort * Projection * ModelView * Matrix(vertex);
        return transformed_vertex;
    }
    virtual bool fragment(Vec3f bar, TGAColor& color)
    {
        // get uv, intensity at bar
        Vec2i uv = varying_uv[0] * bar.x + varying_uv[1] * bar.y + varying_uv[2] * bar.z;
        float intensity = bar.x * varying_intensity[0] + bar.y * varying_intensity[1] + bar.z * varying_intensity[2];
        intensity = std::max(0.0f, std::min(1.0f, intensity));

        color = model->diffuse(uv) * intensity;
        return false;
    }
};

struct GouraudShader : IShader
{
    // varying : share value in vertex shader and fragment shader
    float varying_intensity[3];

    virtual ~GouraudShader() {}
    virtual Vec3f vertex(int iface, int nvert)
    {
        varying_intensity[nvert] = model->norm(iface, nvert) * light_dir;
        Vec3f vertex = model->vert(iface, nvert);
        Vec3f transformed_vertex = ViewPort * Projection * ModelView * Matrix(vertex);
        return transformed_vertex;
    }
    virtual bool fragment(Vec3f bar, TGAColor& color)
    {
        float intensity = bar[0] * varying_intensity[0] + bar[1] * varying_intensity[1] + bar[2] * varying_intensity[2];
        intensity = std::max(0.0f, std::min(1.0f, intensity));
        color = TGAColor(255, 255, 255) * intensity;
        return false;
    }
};

int main(int argc, char** argv) 
{
    TGAImage image(width, height, TGAImage::RGB);
    TGAImage zbuffer(width, height, TGAImage::GRAYSCALE);

    model = new Model("obj\\african_head.obj");

    lookat(camera, center, Vec3f(0, 1, 0));
    viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
    projection(-1.0f / (camera - center).norm());

    Shader shader;
    GouraudShader gShader;

    for (int i = 0; i < model->nfaces(); i++)
    {
        Vec3i screen_coords[3];
        // Vertex Shader
        for (int j = 0; j < 3; j++)
        {
            screen_coords[j] = shader.vertex(i, j);
        }
        // Rasterizer (callback Fragment Shader each pixel)
        triangle(screen_coords, shader, image, zbuffer);
    }

    image.flip_vertically();
    image.write_tga_file("output\\output14.tga");
    zbuffer.flip_vertically();
    zbuffer.write_tga_file("zbuffer.tga");

    delete model;

    return 0;
}
