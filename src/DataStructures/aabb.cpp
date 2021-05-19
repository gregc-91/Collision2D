#include "aabb.h"

AABB::AABB() : min{0,0}, max{0,0}
{

}

AABB::AABB(int x1, int y1, int x2, int y2) :
    min{x1, y1},
    max{x2, y2}
{

}

void AABB::move(int x, int y)
{
    min[0] += x;
    max[0] += x;
    min[1] += y;
    max[1] += y;
}

bool AABB::overlaps(const AABB &other) const
{
    return max[0] > other.min[0] && min[0] < other.max[0] &&
           max[1] > other.min[1] && min[1] < other.max[1];
}