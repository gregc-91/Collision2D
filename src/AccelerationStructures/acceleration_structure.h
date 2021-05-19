#include "../DataStructures/hybrid_vector.h"

#include "../DataStructures/aabb.h"

class AccelerationStructure {

    virtual uint32_t count() const;
    virtual int add(uint32_t id, const AABB& aabb);
    virtual int remove(uint32_t id, const AABB& aabb);
    virtual int move(uint32_t id, const AABB& old_aabb, const AABB& new_aabb);
    virtual HybridVector<uint32_t> query(const AABB& aabb) const;
    virtual int tidy();
};