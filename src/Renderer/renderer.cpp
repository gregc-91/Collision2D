#include "renderer.h"
#include "../Simulation/simulation.h"

#include <string.h>
#include <GLFW/glfw3.h>

Renderer::Renderer(std::vector<Entity> &entities, uint32_t width, uint32_t height) :
    window_width(width),
    window_height(height),
    render_width(width),
    render_height(height),
    start_x(0),
    start_y(0),
    start_float_x(0.0f),
    start_float_y(0.0f),
    m_zoom(1.0f),
    entities(entities),
    framebuffer(width*height*4)
{
}

Renderer::~Renderer()
{

}

void Renderer::draw_rect(const AABB &aabb, uint8_t colour[4])
{
    for (int y = aabb.min[1]; y < aabb.max[1]; y++)
    for (int x = aabb.min[0]; x < aabb.max[0]; x++) {
        if (x >= int(start_x) && x < int(start_x+render_width) && y >= int(start_y) && y < int(start_y + render_height)) {
            framebuffer[((y-start_y)*render_width + x-start_x) * 4 + 0] = colour[0];
            framebuffer[((y-start_y)*render_width + x-start_x) * 4 + 1] = colour[1];
            framebuffer[((y-start_y)*render_width + x-start_x) * 4 + 2] = colour[2];
        }
    }
}

void Renderer::clear()
{
    const uint8_t background_shade = 0x20;
    memset(framebuffer.data(), background_shade, render_width*render_height*4);
    for (unsigned i = 0; i < render_width*render_height; i++) {
        framebuffer[i*4+3] = 0xFF;
    }
}

void Renderer::draw()
{
    clear();

    for (auto &entity : entities) {
        draw_rect(entity.aabb, entity.colour);
    }

    glPixelZoom(m_zoom, m_zoom);
    glDrawPixels(render_width, render_height, GL_RGBA, GL_UNSIGNED_BYTE, framebuffer.data());
}

void Renderer::translate(int x, int y)
{
    start_float_x += float(x) / m_zoom;
    start_float_y += float(y) / m_zoom;
    start_x = int(start_float_x);
    start_y = int(start_float_y);
}

void Renderer::zoom(float direction)
{
    if (direction > 0.0f) {
        m_zoom *= 1.25f;
    } else {
        m_zoom /= 1.25f;
    }

    int width_delta = int(render_width) - window_width / m_zoom;
    int height_delta = int(render_height) - window_height / m_zoom;

    start_float_x += width_delta / 2;
    start_float_y += height_delta / 2;
    start_x = int(start_float_x);
    start_y = int(start_float_y);
    render_width = window_width / m_zoom;
    render_height = window_height / m_zoom;
    framebuffer.resize(render_width*render_height*4);
}

void Renderer::resize(uint32_t new_width, uint32_t new_height)
{
    window_width = new_width;
    window_height = new_height;
    render_width = window_width * m_zoom;
    render_height = window_height * m_zoom;
    framebuffer.resize(render_width*render_height*4);
}