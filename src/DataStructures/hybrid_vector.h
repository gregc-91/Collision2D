#ifndef __HYBRID_VECTOR_H__
#define __HYBRID_VECTOR_H__

#include <stdint.h>

template <class T, size_t N = 64>
class HybridVector {

public:
    HybridVector();
    ~HybridVector();
    T* data();
    const T* data() const;
    uint32_t size() const;
    uint32_t capacity() const;
    T& operator[](uint32_t i);
    const T& operator[](uint32_t i) const;
    void push_back(const T& entry);
    T pop_back();
    void reserve(uint32_t n);
    void clear();

private:
    enum { base_capacity = N };
    T m_buff[base_capacity];
    T* m_data;
    uint32_t m_size;
    uint32_t m_capacity;
};

#include "hybrid_vector.tpp"

#endif
