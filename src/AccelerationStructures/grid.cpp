#include "grid.h"

#include <stdio.h>

Grid::Grid()
{

}

Grid::Grid(int min_x, int min_y, int max_x, int max_y) :
    m_min_x(min_x), 
    m_min_y(min_y),
    m_num_cells_x(256), 
    m_num_cells_y(256),
    m_cell_width((max_x-min_x+m_num_cells_x-1) / m_num_cells_x), 
    m_cell_height((max_y-min_y+m_num_cells_y-1) / m_num_cells_y),
    m_gridcells(m_num_cells_x * m_num_cells_y, 0xFFFFFFFF),
    m_leaves(65536),
    m_free_index(0),
    m_count(0)
{
    for (unsigned i = 0; i < m_leaves.size(); i++) {
        m_leaves[i].next = i+1;
    }
    m_leaves[m_leaves.size()-1].next = 0xFFFFFFFF;

    AccelerationStructure::m_type = AccelerationStructureType::Grid;
}

uint32_t Grid::count() const
{
    return m_count;
}

int Grid::grow()
{
    uint32_t old_size = m_leaves.size();
    m_leaves.resize(old_size * 2);

    for (unsigned i = old_size; i < old_size * 2; i++) {
        m_leaves[i].next = i+1;
    }
    m_leaves[m_leaves.size()-1].next = 0xFFFFFFFF;
    m_free_index = old_size;

    return 0;
}

int Grid::add(uint32_t id, const AABB& aabb)
{
    int min_cell_x = (aabb.min[0] - m_min_x) / m_cell_width;
    int min_cell_y = (aabb.min[1] - m_min_y) / m_cell_height;
    int max_cell_x = (aabb.max[0] - m_min_x) / m_cell_width;
    int max_cell_y = (aabb.max[1] - m_min_y) / m_cell_height;

    assert(min_cell_x >= 0 && max_cell_x < int(m_num_cells_x));
    assert(min_cell_y >= 0 && max_cell_y < int(m_num_cells_y));

    for (int y = min_cell_y; y <= max_cell_y; y++)
    for (int x = min_cell_x; x <= max_cell_x; x++) {

        if (m_free_index == 0xFFFFFFFF) {
            grow();
        }
        assert(m_free_index != 0xFFFFFFFF);

        uint32_t index = m_free_index;
        m_free_index = m_leaves[m_free_index].next;
        m_leaves[index].id = id;
        m_leaves[index].next = m_gridcells[y * m_num_cells_x + x];
        m_gridcells[y * m_num_cells_x + x] = index;
    }
    m_count++;

    return 0;
}

int Grid::remove(uint32_t id, const AABB& aabb)
{
    int min_cell_x = (aabb.min[0] - m_min_x) / m_cell_width;
    int min_cell_y = (aabb.min[1] - m_min_y) / m_cell_height;
    int max_cell_x = (aabb.max[0] - m_min_x) / m_cell_width;
    int max_cell_y = (aabb.max[1] - m_min_y) / m_cell_height;

    assert(min_cell_x >= 0 && max_cell_x < int(m_num_cells_x));
    assert(min_cell_y >= 0 && max_cell_y < int(m_num_cells_y));

    for (int y = min_cell_y; y <= max_cell_y; y++)
    for (int x = min_cell_x; x <= max_cell_x; x++) {
        uint32_t *p_index = &m_gridcells[y * m_num_cells_x + x];
        while (*p_index != 0xFFFFFFFF) {
            if (m_leaves[*p_index].id == id) {
                uint32_t index = *p_index;
                assert(index != 0xFFFFFFFF);
                *p_index = m_leaves[index].next;
                m_leaves[index].next = m_free_index;
                m_free_index = index;
            } else {
                p_index = &m_leaves[*p_index].next;
            }
        }
    }
    m_count--;

    return 0;
}

int Grid::move(uint32_t id, const AABB& old_aabb, const AABB& new_aabb)
{
    remove(id, old_aabb);
    add(id, new_aabb);

    return 0;
}

// Note that this can return multiple references to the same primitive
HybridVector<uint32_t> Grid::query(const AABB& aabb) const
{
    HybridVector<uint32_t> result;

    int min_cell_x = (aabb.min[0] - m_min_x) / m_cell_width;
    int min_cell_y = (aabb.min[1] - m_min_y) / m_cell_height;
    int max_cell_x = (aabb.max[0] - m_min_x) / m_cell_width;
    int max_cell_y = (aabb.max[1] - m_min_y) / m_cell_height;

    assert(min_cell_x >= 0 && max_cell_x < int(m_num_cells_x));
    assert(min_cell_y >= 0 && max_cell_y < int(m_num_cells_y));

    for (int y = min_cell_y; y <= max_cell_y; y++)
    for (int x = min_cell_x; x <= max_cell_x; x++) {
        uint32_t index = m_gridcells[y * m_num_cells_x + x];
        while (index != 0xFFFFFFFF) {
            result.push_back(m_leaves[index].id);
            index = m_leaves[index].next;
        }
    }
    return result;
}
   
int Grid::tidy()
{
    return 0;
}