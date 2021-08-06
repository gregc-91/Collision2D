#ifndef __SIMILATION_H__
#define __SIMULATION_H__

#include <vector>

#include "../DataStructures/aabb.h"
#include "../AccelerationStructures/grid.h"
#include "../AccelerationStructures/quadtree.h"

struct Entity {
    AABB aabb;
    uint8_t colour[4];

    Entity(const AABB &aabb);
};

class Simulation
{
public:
    Simulation();
    void setup(uint32_t width, uint32_t height, uint32_t n, AccelerationStructureType type);
    void step();
    void reset();
    void pause();
    void unpause();
    bool paused();
    bool collision(uint32_t i, const AABB& aabb) const;
    std::vector<Entity>& entities();

private:
    std::vector<Entity> m_entities;
    AccelerationStructure* m_accel;
    uint32_t m_width;
    uint32_t m_height;
    uint32_t m_num_particles;
    bool m_paused;
};

#endif