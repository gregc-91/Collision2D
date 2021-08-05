#include "quadtree.h"

#include <cstdio>

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
    m_aabbs(65536),
    m_root(0xFFFFFFFF),
    m_free_node(0),
    m_free_leaf(0),
    m_count(0)
{
    for (unsigned i = 0; i < m_nodes.size(); i++) {
        m_nodes[i].node[0].child = i+1;
    }
    m_nodes[m_nodes.size()-1].node[0].child = 0xFFFFFFFF;

    for (unsigned i = 0; i < m_leaves.size(); i++) {
        m_leaves[i].next = i+1;
    }
    m_leaves[m_leaves.size()-1].next = 0xFFFFFFFF;

    m_root = pop_node();
    m_nodes[m_root].aabb = AABB(m_min_x, m_min_y, m_max_x, m_max_y);
    for (unsigned i = 0; i < 4; i++) {
        m_nodes[m_root].node[i].child = 0xFFFFFFFF;
        m_nodes[m_root].node[i].count = 0;
    }

    AccelerationStructure::m_type = AccelerationStructureType::Quadtree;
}

uint32_t Quadtree::count() const
{
    return m_count;
}

uint32_t Quadtree::pop_node()
{
    uint32_t result = m_free_node;
    m_free_node = m_nodes[m_free_node].node[0].child;

    m_node_population++;
    return result;
}

uint32_t Quadtree::pop_leaf()
{
    if (m_free_leaf == 0xFFFFFFFF) {
        grow_leaves();
    }

    uint32_t result = m_free_leaf;
    m_free_leaf = m_leaves[m_free_leaf].next;

    m_leaf_population++;
    return result;
}

void Quadtree::grow_leaves()
{
    uint32_t old_size = m_leaves.size();
    m_leaves.resize(old_size * 2);

    for (unsigned i = old_size; i < old_size * 2; i++) {
        m_leaves[i].next = i+1;
    }
    m_leaves[m_leaves.size()-1].next = 0xFFFFFFFF;
    m_free_leaf = old_size;
}

void Quadtree::push_node(uint32_t index)
{
    m_node_population--;
    m_nodes[index].node[0].child = m_free_node;
    m_free_node = index;
}

void Quadtree::push_leaf(uint32_t index)
{
    m_leaf_population--;
    m_leaves[index].next = m_free_leaf;
    m_free_leaf = index;
}

Quadtree::QuadAABB get_children(const AABB &parent)
{
    Quadtree::QuadAABB result;
    unsigned i = 0;
    int halfwidth  = (parent.max[0] - parent.min[0]) >> 1;
    int halfheight = (parent.max[1] - parent.min[1]) >> 1;
    for (unsigned y = 0; y < 2; y++)
    for (unsigned x = 0; x < 2; x++, i++) {
        result.aabb[i].min[0] = parent.min[0] + halfwidth*x;
        result.aabb[i].min[1] = parent.min[1] + halfheight*y;
        result.aabb[i].max[0] = x ? parent.max[0] : parent.min[0] + halfwidth;
        result.aabb[i].max[1] = y ? parent.max[1] : parent.min[1] + halfheight;
    }
    return result;
}

bool all_leaves(const Quadtree::QuadNode &qnode)
{
    return qnode.node[0].count >= 0
        && qnode.node[1].count >= 0
        && qnode.node[2].count >= 0
        && qnode.node[3].count >= 0;
}

uint32_t child_sum(const Quadtree::QuadNode &qnode)
{
    return qnode.node[0].count
         + qnode.node[1].count
         + qnode.node[2].count
         + qnode.node[3].count;
}

void Quadtree::validate_leaf(Node &node)
{
    uint32_t idx = node.child;
    for (int j = 0; j < node.count; j++) {
        assert(idx != 0xFFFFFFFF);
        idx = m_leaves[idx].next;
    }
}

void Quadtree::add_leaf(uint32_t id, Node &node, AABB &aabb)
{
    // Add a new leaf to the list
    uint32_t leaf_idx = pop_leaf();

    Leaf &leaf = m_leaves[leaf_idx];
    leaf.id = id;
    leaf.next = node.child;
    node.child = leaf_idx;
    node.count++;

    if (node.count > 8) { // Subdivide the node
        uint32_t idx = pop_node();
        QuadNode &quad = m_nodes[idx];
        QuadAABB children = get_children(aabb);
        quad.aabb = aabb;

        for (unsigned i = 0; i < 4; i++) {
            uint32_t leaf_idx = node.child;
            quad.node[i].count = 0;
            quad.node[i].child = 0xFFFFFFFF;

            for (int j = 0; j < node.count; j++) {
                const AABB& leaf_aabb = m_aabbs[m_leaves[leaf_idx].id];

                if (leaf_aabb.overlaps(children.aabb[i])) {
                    add_leaf(m_leaves[leaf_idx].id, quad.node[i], children.aabb[i]);
                }
                leaf_idx = m_leaves[leaf_idx].next;
            }
        }
        node.child = idx;
        node.count = -1;
    }
}

int Quadtree::add(uint32_t id, const AABB& aabb)
{
    if (id >= m_aabbs.size()) {
        m_aabbs.resize(2*id);
    }
    m_aabbs[id] = aabb;

    HybridVector<uint32_t> leaves;
    find_leaves(m_root, aabb, leaves);

    for (unsigned i = 0; i < leaves.size(); i++) {
        Node &node = m_nodes[leaves[i] >> 2].node[leaves[i] & 0x3];
        AABB node_aabb = get_children(m_nodes[leaves[i] >> 2].aabb).aabb[leaves[i] & 0x3];

        add_leaf(id, node, node_aabb);
    }

    m_count++;

    return 0;
}

int Quadtree::remove(uint32_t id, const AABB& aabb)
{
    HybridVector<uint32_t> leaves;
    find_leaves(m_root, aabb, leaves);

    for (unsigned i = 0; i < leaves.size(); i++) {
        Node &node = m_nodes[leaves[i] >> 2].node[leaves[i] & 0x3];
        uint32_t *p_idx = &node.child;
        while (*p_idx != 0xFFFFFFFF) {
            if (m_leaves[*p_idx].id == id) {
                uint32_t index = *p_idx;
                assert(index != 0xFFFFFFFF);
                *p_idx = m_leaves[index].next;
                push_leaf(index);
                node.count--;
            } else {
                p_idx = &m_leaves[*p_idx].next;
            }
        }
    }
    m_count--;

    return 0;
}

int Quadtree::move(uint32_t id, const AABB& old_aabb, const AABB& new_aabb)
{
    remove(id, old_aabb);
    add(id, new_aabb);
    return 0;
}

void Quadtree::find_leaves(const uint32_t index, const AABB& query, HybridVector<uint32_t> &leaves) const
{
    const QuadAABB qaabb = get_children(m_nodes[index].aabb);
    const QuadNode &qnode = m_nodes[index];

    for (unsigned i = 0; i < 4; i++) {
        const Node &node = qnode.node[i];
        const AABB &aabb = qaabb.aabb[i];

        if (!aabb.overlaps(query)) {
            continue;
        }

        if (node.count >= 0) {
            leaves.push_back((index << 2) | i);
        }
        else if (node.count == -1) {
            find_leaves(node.child, query, leaves);
        }
    }
}

// Note that this can return multiple references to the same primitive
HybridVector<uint32_t> Quadtree::query(const AABB& aabb) const
{
    HybridVector<uint32_t> result;
    HybridVector<uint32_t> leaves;

    find_leaves(m_root, aabb, leaves);

    for (unsigned i = 0; i < leaves.size(); i++) {
        uint32_t idx = m_nodes[leaves[i] >> 2].node[leaves[i] & 0x3].child;
        while (idx != 0xFFFFFFFF) {
            result.push_back(m_leaves[idx].id);
            idx = m_leaves[idx].next;
        }
    }

    return result;
}

void Quadtree::tidy_node(Node& parent)
{
    uint32_t qnode_index = parent.child;
    QuadNode &qnode = m_nodes[parent.child];
    
    for (unsigned i = 0; i < 4; i++) {
        if (qnode.node[i].count == -1) {
            tidy_node(qnode.node[i]);
        }
    }

    if (all_leaves(qnode) && child_sum(qnode) <= 8) {
        // Link all leaf nodes to the parent of qnode
        parent.count = child_sum(qnode);
        parent.child = 0xFFFFFFFF;

        for (unsigned i = 0; i < 4; i++) {
            uint32_t idx = qnode.node[i].child;
            for (int j = 0; j < qnode.node[i].count; j++) {
                uint32_t next = m_leaves[idx].next;
                m_leaves[idx].next = parent.child;
                parent.child = idx;
                idx = next;
            }
        }
        push_node(qnode_index);
    }
}

int Quadtree::tidy()
{
    for (unsigned i = 0; i < 4; i++) {
        tidy_node(m_nodes[m_root].node[i]);
    }

    return 0;
}