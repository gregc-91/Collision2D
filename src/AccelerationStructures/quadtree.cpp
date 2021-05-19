#include "quadtree.h"

Quadtree::Quadtree()
{

}

Quadtree::Quadtree(int min_x, int min_y, int max_x, int max_y) :
    m_min_x(min_x),
    m_min_y(min_y),
    m_max_x(max_x),
    m_max_y(max_y),
    m_nodes(65536),
    m_leaves(65536),
    m_root(0xFFFFFFFF),
    m_free_node(0),
    m_free_leaf(0),
    m_count(0)
{
    for (unsigned i = 0; i < m_nodes.size(); i++) {
        m_nodes[i].child = i+1;
    }
    m_nodes[m_nodes.size()-1].child = 0xFFFFFFFF;

    for (unsigned i = 0; i < m_leaves.size(); i++) {
        m_leaves[i].next = i+1;
    }
    m_leaves[m_leaves.size()-1].next = 0xFFFFFFFF;

    m_root = pop_node();
    m_nodes[m_root].child = 0xFFFFFFFF;
    m_nodes[m_root].count = 0;
}

uint32_t count() const
{
    return m_count;
}

int add(uint32_t id, const AABB& aabb)
{
    // todo


    if (node.count > -8 && node.count <= 0) {
        // 0 to 7 leaf children, append to the list
    } else if (node.count == -8)
        // 8 leaf children, subdivide

        // Count how many quadrants we need
        // Allocate four contiguous nodes
        // Link the 8 existing children to the new nodes
        // Add the new aabb to any quadrant it overlaps

        // Problem: we need the aabbs of the existing nodes
        //   Should we reference the original array?
        
    } else {
        // Node children, keep traversing to all quadrants the aabb overlaps
    }

}

int remove(uint32_t id, const AABB& aabb)
{
    // todo
}

int move(uint32_t id, const AABB& old_aabb, const AABB& new_aabb)
{
    // todo
}

HybridVector<uint32_t> query(const AABB& aabb) const
{
    // todo
}

int tidy()
{
    // todo
}