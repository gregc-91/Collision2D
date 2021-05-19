#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <vector>
#include <stdint.h>

#include "../DataStructures/AABB.h"

struct Entity;

class Renderer
{
public:
    Renderer(std::vector<Entity> &entities, uint32_t width, uint32_t height);
    ~Renderer();

    void clear();
    void draw();
    void draw_rect(const AABB &aabb, uint8_t colour[4]);
    void translate(int x, int y);
    void zoom(float direction);
    void resize(uint32_t new_width, uint32_t new_height);

private:
    uint32_t window_width;
    uint32_t window_height;
    uint32_t render_width;
    uint32_t render_height;
    uint32_t start_x;
    uint32_t start_y;
    float start_float_x;
    float start_float_y;
    float m_zoom;
    std::vector<Entity> &entities;
    std::vector<uint8_t> framebuffer;
};

#endif