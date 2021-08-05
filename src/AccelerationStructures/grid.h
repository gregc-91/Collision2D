#ifndef __GRID_H__
#define __GRID_H__

#include <vector>

#include "acceleration_structure.h"

class Grid : public AccelerationStructure
{
public:
    Grid();
    Grid(int min_x, int min_y, int max_x, int max_y);
    uint32_t count() const;
    int grow();
    int add(uint32_t id, const AABB& aabb);
    int remove(uint32_t id, const AABB& aabb);
    int move(uint32_t id, const AABB& old_aabb, const AABB& new_aabb);
    HybridVector<uint32_t> query(const AABB& aabb) const;
    int tidy();
private:

    struct Leaf {
        uint32_t next;
        uint32_t id;
    };

    uint32_t m_min_x, m_min_y;
    uint32_t m_max_x, m_max_y;
    uint32_t m_num_cells_x, m_num_cells_y;
    uint32_t m_cell_width, m_cell_height;
    std::vector<uint32_t> m_gridcells;
    std::vector<Leaf> m_leaves;
    uint32_t m_free_index;
    uint32_t m_count;
};

#endif
