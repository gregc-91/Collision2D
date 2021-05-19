
#include <assert.h>
#include <string.h>

template <class T, size_t N>
HybridVector<T, N>::HybridVector() : m_data(m_buff), m_size(0), m_capacity(base_capacity)
{

}

template <class T, size_t N>
HybridVector<T, N>::~HybridVector()
{
    if (m_capacity != base_capacity) {
        free(m_data);
    }
}

template <class T, size_t N>
T* HybridVector<T, N>::data()
{
    return m_data;
}

template <class T, size_t N>
const T* HybridVector<T, N>::data() const
{
    return m_data;
}

template <class T, size_t N>
uint32_t HybridVector<T, N>::size() const
{
    return m_size;
}

template <class T, size_t N>
uint32_t HybridVector<T, N>::capacity() const
{
    return m_capacity;
}

template <class T, size_t N>
T& HybridVector<T, N>::operator[](uint32_t i)
{
    assert(i < m_size);
    return m_data[i];
}

template <class T, size_t N>
const T& HybridVector<T, N>::operator[](uint32_t i) const
{
    assert(i < m_size);
    return m_data[i];
}

template <class T, size_t N>
void HybridVector<T, N>::push_back(const T& entry)
{
    if (m_size == m_capacity) {
        reserve(m_size*2);
    }
    assert(m_size < m_capacity);
    m_data[m_size++] = entry;
}

template <class T, size_t N>
T HybridVector<T, N>::pop_back()
{
    assert(m_size);
    return m_data[--m_size];
}

template <class T, size_t N>
void HybridVector<T, N>::reserve(uint32_t n)
{
    if (n > m_capacity) {
        if (m_capacity == base_capacity) {
            m_data = static_cast<T*>(malloc(n * sizeof(T)));
            memcpy(m_data, m_buff, m_size*sizeof(T));
        } else {
            m_data = static_cast<T*>(realloc(m_data, n * sizeof(T)));
        }
        m_capacity = n;
    }
}

template <class T, size_t N>
void HybridVector<T, N>::clear()
{
    size = 0;
    if (m_capacity != base_capacity) {
        free(m_data);
        m_data = m_buff;
    }
}