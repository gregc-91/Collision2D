#ifndef __QUADTREE_H__
#define __QUADTREE_H__

#include "../DataStructures/aabb.h"
#include "../DataStructures/hybrid_vector.h"

#include <vector>

struct Node {
    uint32_t child;
    int32_t count;
};

struct Leaf {
    uint32_t next;
    uint32_t id;
};

class Quadtree {
public:
    Quadtree();
    Quadtree(int min_x, int min_y, int max_x, int max_y);
    uint32_t count() const;
    int add(uint32_t id, const AABB& aabb);
    int remove(uint32_t id, const AABB& aabb);
    int move(uint32_t id, const AABB& old_aabb, const AABB& new_aabb);
    HybridVector<uint32_t> query(const AABB& aabb) const;
    int tidy();
private:
    uint32_t m_min_x, m_min_y;
    uint32_t m_max_x, m_max_y;
    std::vector<Node> m_nodes;
    std::vector<Leaf> m_leaves;
    uint32_t m_root;
    uint32_t m_free_node;
    uint32_t m_free_leaf;
    uint32_t m_count;
};

#endif