#ifndef __AABB_H__
#define __AABB_H__

struct AABB
{
    int min[2];
    int max[2];

    AABB();
    AABB(int x1, int y1, int x2, int y2);

    void move(int x, int y);
    bool overlaps(const AABB &other) const;
};

#endif