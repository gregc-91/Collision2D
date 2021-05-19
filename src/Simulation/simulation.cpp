#include "simulation.h"
#include "../Utils/colour.h"

#include <stdlib.h>
#include <stdio.h>

Entity::Entity(const AABB &aabb) :
    aabb(aabb)
{
    int colour_index = rand() % 5;
    for (unsigned i = 0; i < 4; i++)
        colour[i] = box_colours[colour_index][i];
}

Simulation::Simulation()
{

}

void Simulation::setup(uint32_t width, uint32_t height)
{
    srand(0x12345);

    m_width = width;
    m_height = height;
    m_paused = false;
    m_grid = Grid(0, 0, width, height);

    for (unsigned i = 0; i < 32000; i++) {

        AABB aabb;
        unsigned attempts = 0;
        do {
            int x1 = rand() % width;
            int y1 = rand() % height;
            int x2 = std::min(x1 + (rand() & 0x3) + 1, int(width-1));
            int y2 = std::min(y1 + (rand() & 0x3) + 1, int(height-1));
            aabb = AABB(x1, y1, x2, y2);
            attempts++;
        } while (collision(i, aabb) && attempts < 64);

        if (attempts != 64) {
            m_entities.push_back(Entity(aabb));
            m_grid.add(i, aabb);
        }
    }
}

bool Simulation::collision(uint32_t id, const AABB& aabb) const
{
#if 1
    auto query = m_grid.query(aabb);
    for (unsigned i = 0; i < query.size(); i++) {
        if (query[i] != id && aabb.overlaps(m_entities[query[i]].aabb)) {
            return true;
        }
    }
#else
    for (auto &other : m_entities) {
        if (aabb.overlaps(other.aabb)) {
            return true;
        }
    }
#endif
    return false;
}

void Simulation::step()
{
    if (!m_paused) {
        for (unsigned i = 0; i < m_entities.size(); i++) {
            AABB &aabb = m_entities[i].aabb;

            int delta_x = (rand() % 3) - 1;
            int delta_y = (rand() % 3) - 1;

            AABB new_aabb = aabb;
            new_aabb.move(delta_x, delta_y);
            
            if (new_aabb.min[0] < 0 || new_aabb.min[1] < 0 || 
                new_aabb.max[0] >= int(m_width) || new_aabb.max[1] >= int(m_height)) {
                continue;
            }

            if (!collision(i, new_aabb)) {
                m_grid.move(i, aabb, new_aabb);
                aabb.move(delta_x, delta_y);
            }
        }
    }
}

void Simulation::reset()
{
    m_entities.clear();
    setup(m_width, m_height);
}

void Simulation::pause()
{
    m_paused = true;
}

void Simulation::unpause()
{
    m_paused = false;
}

bool Simulation::paused()
{
    return m_paused;
}

std::vector<Entity>& Simulation::entities()
{
    return m_entities;
}