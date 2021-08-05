#ifndef __ACCELERATION_STRUCTURE_H__
#define __ACCELERATION_STRUCTURE_H__

#include "../DataStructures/hybrid_vector.h"

#include "../DataStructures/aabb.h"

enum class AccelerationStructureType {
    Grid,
    Quadtree
};

class AccelerationStructure {
public:
    virtual uint32_t count() const;
    virtual int add(uint32_t id, const AABB& aabb);
    virtual int remove(uint32_t id, const AABB& aabb);
    virtual int move(uint32_t id, const AABB& old_aabb, const AABB& new_aabb);
    virtual HybridVector<uint32_t> query(const AABB& aabb) const;
    virtual int tidy();

    AccelerationStructureType m_type;
};

#endif