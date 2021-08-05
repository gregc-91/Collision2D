#ifndef __QUADTREE_H__
#define __QUADTREE_H__

#include "../DataStructures/aabb.h"
#include "../DataStructures/hybrid_vector.h"
#include "acceleration_structure.h"

#include <vector>

class Quadtree : public AccelerationStructure
{
public:

    struct Node {
        uint32_t child;
        int32_t count;
    };

    struct QuadNode {
        AABB aabb;
        Node node[4];
    };

    struct QuadAABB {
        AABB aabb[4];
    };

    struct Leaf {
        uint32_t next;
        uint32_t id;
    };

    Quadtree();
    Quadtree(int min_x, int min_y, int max_x, int max_y);
    uint32_t count() const;
    int add(uint32_t id, const AABB& aabb);
    int remove(uint32_t id, const AABB& aabb);
    int move(uint32_t id, const AABB& old_aabb, const AABB& new_aabb);
    HybridVector<uint32_t> query(const AABB& aabb) const;
    int tidy();
private:
    void tidy_node(Node& parent);
    void add_leaf(uint32_t id, Node &node, AABB &aabb);
    void find_leaves(const uint32_t index, const AABB& query, HybridVector<uint32_t> &leaves) const;
    void validate_leaf(Node &node);
    uint32_t pop_node();
    uint32_t pop_leaf();
    void grow_leaves();
    void push_node(uint32_t index);
    void push_leaf(uint32_t index);
    uint32_t m_min_x, m_min_y;
    uint32_t m_max_x, m_max_y;
    std::vector<QuadNode> m_nodes;
    std::vector<Leaf> m_leaves;
    std::vector<AABB> m_aabbs;
    uint32_t m_root;
    uint32_t m_free_node;
    uint32_t m_free_leaf;
    uint32_t m_count;

    uint32_t m_leaf_population = 0;
    uint32_t m_node_population = 0;
};

#endif